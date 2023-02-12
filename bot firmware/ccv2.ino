//cookie cutter firmware v1.0

#include <TaskScheduler.h>
#include <H3LIS331DL.h>
#include <RFM69.h>
#include <Wire.h>
#include <SPI.h>
#include <movingAvg.h>
#include <MovingAverageFloat.h>
#include <FastLED.h>
#include <filters.h>

//filter stuff
const float cutoff_freq   = 5.0;  //Cutoff frequency in Hz
const float sampling_time = 0.001; //Sampling time in seconds.
IIR::ORDER  order  = IIR::ORDER::OD3; // Order (OD1 to OD4)

// Low-pass filter
Filter f(cutoff_freq, sampling_time, order);

//scheduler stuff
void accelreadcallback();
void voltagereadcallback();
void blinkercallback();
Task accelread(1, TASK_FOREVER, &accelreadcallback);
Task voltageread(100, TASK_FOREVER, &voltagereadcallback);
Task blinker(250, TASK_FOREVER, &blinkercallback);
Scheduler runner;   //scheduler object

//intervaltimer stuff
IntervalTimer controlTimer;

//led stuff
#define NUMLEDS       3
CRGB LEDS_0[NUMLEDS]; // Define the arrays of leds
CRGB LEDS_1[NUMLEDS];

//moving average stuff
movingAvg xAvg(50);
movingAvg yAvg(100);
movingAvg zAvg(100);
movingAvg rpmAvg(50);
movingAvg deltaAvg(10);

//H3LIS331 Accel stuff
//please get these value by running H3LIS331DL_AdjVal Sketch.
#define VAL_X_AXIS  360
#define VAL_Y_AXIS  98
#define VAL_Z_AXIS  371
#define ACCEL_RANGE 100
float CHIP_OFFSET = 14;
H3LIS331DL accel;


//GPIO
#define ESC0_OUT      3
#define ESC1_OUT      4
#define IR0_IN        5    
#define IR1_IN        6
#define LED0_MOSI     23
#define LED0_CLK      22
#define LED1_MOSI     21
#define LED1_CLK      20
#define ADC_VIN       A0  
#define ADC_5V        A1 
#define ADC_33V       A2 

//radio stuff
// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      1   // My node ID
#define TONODEID      2   // Destination node ID
#define R_SS          10
#define R_INT         8
#define R_RST         9
// RFM69 frequency, uncomment the frequency of your module:
#define FREQUENCY     RF69_915MHZ
// AES encryption (or not):
#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes
// Use ACKnowledge when sending messages (or not):
#define USEACK        false // Request ACKs or not
// Create a library object for our RFM69HCW module:
RFM69 radio(R_SS, R_INT, true);
//variables
const byte numChars = 65;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
// variables to hold the parsed data
char messageFromPC[numChars] = {0};
boolean newData = false;

//robot variables
volatile int errorState;
int headingCmd;  //heading you want to drive
int intensityCmd;  //how much did you push the stick
int joyScale = 256;  //scale of joysticks
int armed;
int throttleCmd;
int yawCmd;
int moveCmd;
int strafeCmd;
int leftTrimCmd;
int rightTrimCmd;
int blitzModeCmd;
int flipRobot;
int robotOrientation;  //0 is normal, 1 is flipped

int cmdValue;
int sentVarInt;
float sentVarFloat;
char sentVarString[30];

bool IR0_SENSE;
bool IR1_SENSE;

float VOLTS_5V;
float VOLTS_33V;
float VOLTS_BAT;
float xVel;
float yVel;
float zVel;

bool slowBlink;
bool fastBlink;

volatile int deltaInterval;
volatile int rpmInterval;
volatile int rpmPreviousMicros;
volatile int currentRPM;
volatile float calculatedRPM;
volatile float previousRPM;
volatile float deltaRPM;
int rpmTrim;  //trim to stop drift
bool trimLock;
float filteredRPM;  //rpm low pass filter stuff


int driveOffset = 120;  //how many degrees to wait to start giving power
int driveDuration = 180;  //how many degrees is a power phase
volatile int degreeSlice;  //amount of microseconds per degree at current speed
volatile bool irLedFlag;  //controls ir led color
volatile bool aimLedFlag;  //controls aim led color
volatile int accelInterval;  //control loop uses this to only calculate rpm every 10 loops
volatile int indexMicros;  //start of cycle time
volatile int rotationPeriod;  //calculated rotation length
volatile int futureIndex;  //predicted next centerpoint
volatile int robotMode;
volatile int aimWaypoint;  //waypoint for aim leds
volatile int aimLedStartTime;  //start of time to light up aiming LED
volatile int aimLedEndTime;    //time to turn aim LED back off
volatile int driveWaypoint;  //waypoint for drive
volatile int throttleUpTime0;   //time to throttle motor 0 up
volatile int throttleDownTime0; //time to throttle motor 0 down
volatile int throttleUpTime1;   //time to throttle motor 1 up
volatile int throttleDownTime1; //time to throttle motor 1 down

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  accel.init();
  accel.importPara(VAL_X_AXIS,VAL_Y_AXIS,VAL_Z_AXIS);
  accel.setODR(H3LIS331DL_ODR_1000Hz);
  accel.setFullScale(H3LIS331DL_FULLSCALE_2);
   
  
  //radio init
  pinMode(R_RST, OUTPUT);
  digitalWrite(R_RST, LOW); digitalWrite(R_RST, HIGH); delay(10); digitalWrite(R_RST, LOW); delay(10); // manual reset
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW
  //pinMode(R_INT,INPUT_PULLUP);
  
  //moving average init
  xAvg.begin();
  yAvg.begin();
  zAvg.begin();
  rpmAvg.begin();
  deltaAvg.begin();

  //gpio init
  pinMode(ESC0_OUT, OUTPUT);
  pinMode(ESC1_OUT, OUTPUT);
  pinMode(IR0_IN, INPUT);
  pinMode(IR1_IN, INPUT);
  pinMode(LED0_MOSI, OUTPUT);
  pinMode(LED0_CLK, OUTPUT);
  pinMode(LED1_MOSI, OUTPUT);
  pinMode(LED1_CLK, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(IR0_IN),ir_isr,CHANGE);  //attach interrupt for IR

  controlTimer.priority(200);  //set to lower priority on stuff
  controlTimer.begin(control_loop, 100); //starts interrupt routine 10khz
  
  //leds init
  FastLED.addLeds<DOTSTAR, LED0_MOSI, LED0_CLK, RBG>(LEDS_0, NUMLEDS);  // BGR ordering is typical
  FastLED.addLeds<DOTSTAR, LED1_MOSI, LED1_CLK, RBG>(LEDS_1, NUMLEDS);  // BGR ordering is typical

  //scheduler init
  runner.addTask(accelread);
  runner.addTask(voltageread);
  runner.addTask(blinker);
  accelread.enable();
  voltageread.enable();
  blinker.enable();
}

void loop() {
  runner.execute();  //runs scheduler system

  calc_rpm();
  radio_routine();

  if(calculatedRPM < 150) {
    led_set0(255,255,255);  led_set1(255,255,255);  
  } 
  if(throttleCmd > 300 && calculatedRPM < 150) {
    led_set0(0,0,0);  led_set1(0,0,0);  
  }
  //if(IR0_SENSE == true) {
  //  led_set0(0,255,0);
  //}
  //if(IR1_SENSE == true) {
  //  led_set1(0,255,0);
  //}
  /*
  if(aimLedFlag == true && calculatedRPM > 0) {
    led_set0(0,0,255);
  }
  if(aimLedFlag == false && calculatedRPM > 0) {
    led_set0(0,0,0);
  }
  */
  FastLED.show();
  debug_values();
  //delay(1000);
}
