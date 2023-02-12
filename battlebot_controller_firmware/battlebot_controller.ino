#include <TaskScheduler.h>
#include "Adafruit_LC709203F.h"
#include <SPI.h>
#include <RA8875.h>
#include <RFM69.h>

//LC7092033 fuel gauge stuff
Adafruit_LC709203F lc;
float packVoltage;
float packPercent;

//RA8875 stuff
#define RA8875_PWM 5  //backlight pwm
#define RA8875_INT 2  //any pin that have an INT
#define RA8875_CS 10  //see below...
/*Teensy 3.x can use: 2,6,9,10,15,20,21,22,23 */
#define RA8875_RESET 9  //any pin or nothing!
RA8875 tft = RA8875(RA8875_CS);  //Teensy3/arduino's

//GPIO
#define JOY0_INPUT  A0  //joystick analog in
#define JOY1_INPUT  A1 
#define JOY2_INPUT  A2
#define JOY3_INPUT  A3
#define BTN0_INPUT  20  //button inputs
#define BTN1_INPUT  21
#define BTN2_INPUT  22
#define BTN3_INPUT  7
#define VIBRATE_OUT 6
#define IR_PWM_OUT  23

int IR_PWM_FREQ = 38000;  //38khz

//Variables for touchscreen
static uint16_t tx, ty;  //used as temp
int txArray[6];          //arrays for touch averaging
int tyArray[6];
int touchX;  //averaged touch variables
int touchY;
bool touching;  //is the screen currently being touched
bool tappedScreen;
int tSamples = 5;         //samples to take
int tDelay = 1;           //delay between samples (microsec)
int touchLockDelay = 50;  //ms since last touch until lock can unlock
bool touchLock;
int touchLockMillis;

//variables for graphics
int layerSwitchDelay = 1500;  //delay before drawing after layer switch for RA8875 to figure shit out
int screenNumber = 1;  //screen manager variable
bool drawLayer = false;  //layer that is actively being drawn on, false is 0, true is 1
int buttonPress = 0;  //tracks what button just got pressed

//variables for controls
int armed;
int throttleVar;
int yawVar;
int moveVar;
int strafeVar;
bool L1Var;
bool L2Var;
bool R1Var;
bool R2Var;

int cmdValue;
int cmdValueReturn;
int cmdVarType;
int sentVarInt;
float sentVarFloat;
char sentVarString[30];

int deadZone = 5;  //joystick deadzone

//variables for robot status
int robotArmed;
int robotXaccel;
int robotYaccel;
int robotZaccel;
float robot5V;
float robot33V;
float robotBatV;
int robotRPM;
int robotCalcRPM;
int robotTrimRPM;

//scheduler stuff
void lcdcontrolcallback();
void inputcontrolcallback();
void batterymonitorcallback();
Task lcdcontrol(40, TASK_FOREVER, &lcdcontrolcallback); //lcd control callback
Task inputcontrol(40, TASK_FOREVER, &inputcontrolcallback);  //input control
Task batterymonitor(10000, TASK_FOREVER, &batterymonitorcallback);  //battery fuel gauge monitor
Scheduler runner;   //scheduler object

//Radio Setup
// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      2   // My node ID
#define TONODEID      1   // Destination node ID
#define R_SS          4
#define R_INT         3
#define R_RST         8
// RFM69 frequency, uncomment the frequency of your module:
//#define FREQUENCY   RF69_433MHZ
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


void setup() {
  Serial.begin(38400);
  pinMode(RA8875_CS, OUTPUT);
  pinMode(RA8875_PWM, OUTPUT);  //pwm pin set to output
  pinMode(VIBRATE_OUT, OUTPUT);
  pinMode(BTN0_INPUT,INPUT);
  pinMode(BTN1_INPUT,INPUT);
  pinMode(BTN2_INPUT,INPUT);
  pinMode(BTN3_INPUT,INPUT);
  
  analogWrite(RA8875_PWM, 255);  //dim backlight by default
  
  Serial.println("RESETTING RADIO");
  pinMode(R_RST, OUTPUT);  //reset radio pin
  digitalWrite(R_RST, LOW);
  // manual reset radio
  digitalWrite(R_RST, HIGH);
  delay(10);
  digitalWrite(R_RST, LOW);
  delay(10);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");
  // Initialize the RFM69HCW:
  // radio.setCS(10);  //uncomment this if using Pro Micro
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW
  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
  
  lc.begin();  //start fuel gauge
  lc.setPackSize(LC709203F_APA_3000MAH);  //set pack size
  lc.setAlarmVoltage(3.5);  //set alarm voltage
  
  tft.begin(Adafruit_800x480);  //initialize RA8875
  tft.useINT(RA8875_INT);       //We use generic int helper for Internal Resistive Touch
  tft.touchBegin();             //enable touch support for RA8875
  tft.enableISR(true);
  
  runner.addTask(lcdcontrol);  //scheduler stuff
  runner.addTask(inputcontrol); 
  runner.addTask(batterymonitor);
  lcdcontrol.enable();
  inputcontrol.enable();
  batterymonitor.enable();
  
}


void loop() {

//  tft.clearScreen();
//  touchroutine();
//  delay(10);
runner.execute();  //runs scheduler
  
}
