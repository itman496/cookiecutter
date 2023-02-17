#define ESC0_OUT      3
#define ESC1_OUT      4

volatile word frame0 = 0b0000000000000000;  //dshot frame for esc0
volatile word frame1 = 0b0000000000000000;  //dshot frame for esc1

volatile unsigned int throttleSetting;

volatile word cmd0;  //command for esc0
volatile word cmd1;  //command for esc1
volatile bool telemetryCall0 = false;  //bool for calling telemetry on esc0
volatile bool telemetryCall1 = false;  //bool for calling telemetry on esc1
volatile int telemetrySelect = 2;  //var to store what telemetry we are asking for, 2 is off

volatile word rpm0;  //rpm of esc0
volatile word rpm1;  //rpm of esc1
volatile float volts0;  //voltage on esc0
volatile float volts1;  //voltage on esc1
volatile float amps0;  //amp draw on esc0
volatile float amps1;  //amp draw on esc1
volatile int temp0;  //temperature of esc0
volatile int temp1;  //temperature of esc1
volatile int consump0;  //mAh consumed on esc0
volatile int consump1;  //mAh consumed on esc1
 

int test;

IntervalTimer timer1;  //bit timer
IntervalTimer timer2;  //periodic timer
IntervalTimer timer3;  //test timer

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ESC0_OUT, OUTPUT);
  pinMode(ESC1_OUT, OUTPUT);
  
  timer2.begin(send_routine, 1000);  //periodic timer to call dshot every 1ms
  timer3.begin(read_telemetry, 5000);  //call read telemetry every 2ms

  //frame0 = generate_dshot_frame(0, false);  //test call 
  //frame1 = generate_dshot_frame(0, false);  //test call
  cmd0 = 0;
  cmd1 = 0;
  delay(2000);

  telemetrySelect = 0;
  
  //telemetryCall0 = true;  //start telemetry loop

  //frame0 = generate_dshot_frame(1, true);  //test call 
  //delay(270);
  //frame0 = generate_dshot_frame(2, true);  //test call
  
}

void loop() {

  cmd0 = 0;
  cmd1 = 0;
  delay(1000);
  cmd0 = 150;
  cmd1 = 250;
  delay(5000);
  /*
  for(int i = 150; i < 700; i++) {
    cmd0 = i; cmd1 = (i / 2) + 75;
    delay(2);
  }
  delay(500);
  for(int i = 700; i > 150; i--) {
    cmd0 = i; cmd1 = (i / 2) + 75;
    delay(2);
  }
  */
  delay(1000);
  cmd0 = 48;
  cmd1 = 48;
  delay(1000);
}

void read_telemetry (bool esc) {
  //Serial.println(telemetrySelect);
  if(telemetrySelect == 0) {
    telemetryCall0 = true;  //request telemetry 0
  }
  if (telemetrySelect == 1) {
    telemetryCall1 = true;  //request telemetry 1
  }
  if(Serial1.available() > 9) {  //if there is 10 bytes available in the buffer
    Serial.print("ESC: "); Serial.println(telemetrySelect);
    char telemBuffer[10];  //create temp buffer
    for(int i = 0; i < 10; i++){
      telemBuffer[i] = Serial1.read(); //read each byte and store in array
      //Serial.print(i,DEC); Serial.print(":"); Serial.print(telemBuffer[i],DEC); Serial.print(" "); 
    }

    while (Serial1.available()) Serial1.read();  //flush input buffer empty
    
    uint8_t crc = get_crc8(telemBuffer, 9);  //get crc

    if(crc != telemBuffer[9]) {  //if crc isnt equal
      Serial.println("CORRUPT PACKET");
    }
    else {  //if it is
      Serial.println("");

      int temp = telemBuffer[0];  //assemble temperature data ( degrees C)
      float v = (telemBuffer[1] << 8) + telemBuffer[2];  //assemble volt data
      float volts = v / 100;  //make into accurate number
      float a = (telemBuffer[3] << 8) + telemBuffer[4];  //assemble amp data
      float amps = a / 100;  //make into accurate number
      int consump = (telemBuffer[5] << 8) + telemBuffer[6];  //assemble amp data
      int rpm = (((telemBuffer[7] << 8) + telemBuffer[8]) * 100) / 6;  //assemble rpm data, 12 pole

      if(telemetryCall1 == 1) {  //if esc0 selected
        temp0 = temp;  //set temperature of temp0
        volts0 = volts;  //set voltage of volts0
        amps0 = amps;    //set amps of amps0
        consump0 = consump;  //set consumption of consump0
        rpm0 = rpm;       //set rpm of rpm0

        Serial.println("");  Serial.println("ESC0");
        Serial.print("TEMP: "); Serial.print(temp0);  Serial.println(" C");
        Serial.print("VOLTS: "); Serial.print(volts0); Serial.println(" V");
        Serial.print("AMPS: "); Serial.print(amps0); Serial.println(" A");
        Serial.print("CONSUMPTION: "); Serial.print(consump0); Serial.println(" mAh");
        Serial.print("RPM: "); Serial.println(rpm0);
      }
      else {  //if esc1 selected
        temp1 = temp;  //set temperature of temp1
        volts1 = volts;  //set voltage of volts1
        amps1 = amps;    //set amps of amps1
        consump1 = consump;  //set consumption of consump1
        rpm1 = rpm;       //set rpm of rpm1
        
        Serial.println("");  Serial.println("ESC1");
        Serial.print("TEMP: "); Serial.print(temp1);  Serial.println(" C");
        Serial.print("VOLTS: "); Serial.print(volts1); Serial.println(" V");
        Serial.print("AMPS: "); Serial.print(amps1); Serial.println(" A");
        Serial.print("CONSUMPTION: "); Serial.print(consump1); Serial.println(" mAh");
        Serial.print("RPM: "); Serial.println(rpm1);
      }
      
      
    
      
      
      telemetrySelect = !telemetrySelect;  //flip to other esc 
    }
  }
  
}

void test_telemetry() {
  if(Serial1.available()) {
    Serial.println(Serial1.read());
  }
}

word generate_dshot_frame (word thr, bool telem) {  //generate dshot frames
  word dshotFrame = 0b0000000000000000;  //zero the frame to start
  thr = constrain(thr,0,2047);  //constrain throttle input to acceptable range
  for (int i = 15; i > 4; i--) {  //loop through first 11 bits
    bitWrite(dshotFrame, i, (bitRead(thr, i - 5)));  //read each throttle bit and attach to frame 
  }
  
  if(telem == true) {  //if telemtry asked for
    bitWrite(dshotFrame, 4, 1); //set 00000000000*0000 bit to 1 for requesting telemetry
  }
  
  word crc = dshotFrame >> 4;  //initialize crc word matching frame so far, offset so it starts at LSB
  word crc2 = crc;  //make another copy 
  crc = crc >> 4;  //shift right, 4 to put bits at LSB of word, 4 for crc algorithm
  crc = crc ^ crc2;  //bitwise exclusive OR (XOR) with original
  crc2 = crc >> 8;  //shift right 8 bits again, store as a copy
  crc = crc ^ crc2;  //bitwise exclusive OR (XOR) with previous XOR

  for (int i = 3; i > -1; i--) {  //loop first 4 bits (LSB)
    bitWrite(dshotFrame, i, (bitRead(crc,i)));  //write crc to end of packet
  }
  
  return dshotFrame;  //return the frame with crc
}

/*
void endpulse0() {  //end pulse for esc0
  digitalWriteFast(ESC0_OUT, LOW);  // switch off pulse
  timer2.end();  //stop pulse timer
}
void endpulse1() {  //end pulse for esc1
  digitalWriteFast(ESC1_OUT, LOW);  // switch off pulse
  timer3.end();  //stop pulse timer
}
*/

void send_routine() {
  frame0 = generate_dshot_frame(cmd0, telemetryCall0);  //generate dshot frame 0
  frame1 = generate_dshot_frame(cmd1, telemetryCall1);  //generate dshot frame 1
  if (telemetryCall0 == true) {  //if either telem is called, only send 1 then 0 it out
    telemetryCall0 = false;  //set to false so it only sends 1
  }
  if (telemetryCall1 == true) {  //if either telem is called, only send 1 then 0 it out
    telemetryCall1 = false;  //set to false so it only sends 1
  }
  
  noInterrupts();  //stop interrupts during transmission
  for( int bitCount = 15; bitCount > -1; bitCount --) {
    digitalWriteFast(ESC0_OUT, HIGH);  //turn on output for esc0
    digitalWriteFast(ESC1_OUT, HIGH);  //turn on output for esc1
    bool x = bitRead(frame0, bitCount);  //read the bit of esc frame 0
    bool y = bitRead(frame1, bitCount);  //read the bit of esc frame 1
    //timer1.begin(send_routine, 1);  //start the next bit loop
    if (x == 0 && y == 0) {
      delayNanoseconds(545);  //short delay
      digitalWriteFast(ESC0_OUT, LOW);  //turn off pulse 0
      digitalWriteFast(ESC1_OUT, LOW); //turn off pulse 1
      delayNanoseconds(1025);  //delay to finish
    }
    else if (x == 1 && y == 0) {
      delayNanoseconds(545);  //short delay
      digitalWriteFast(ESC1_OUT, LOW); //turn off pulse 1
      delayNanoseconds(525);  //short delay
      digitalWriteFast(ESC0_OUT, LOW);  //turn off pulse 0
      delayNanoseconds(500);  //delay to finish
    }
    else if (x == 0 && y == 1) {
      delayNanoseconds(545);  //short delay
      digitalWriteFast(ESC0_OUT, LOW); //turn off pulse 1
      delayNanoseconds(525);  //short delay
      digitalWriteFast(ESC1_OUT, LOW);  //turn off pulse 0
      delayNanoseconds(500);  //delay to finish
    }
    else {
      delayNanoseconds(1200);  //long delay
      digitalWriteFast(ESC0_OUT, LOW); //turn off pulse 1
      digitalWriteFast(ESC1_OUT, LOW);  //turn off pulse 0
      delayNanoseconds(370);  //delay to finish
    }
  }
  interrupts();  //enable interrupts again
}

uint8_t update_crc8(uint8_t crc, uint8_t crc_seed){
  uint8_t crc_u, i;
  crc_u = crc;
  crc_u ^= crc_seed;
  for ( i=0; i<8; i++) crc_u = ( crc_u & 0x80 ) ? 0x7 ^ ( crc_u << 1 ) : ( crc_u << 1 );
  return (crc_u);
}
uint8_t get_crc8(uint8_t *Buf, uint8_t BufLen){
  uint8_t crc = 0, i;
  for( i=0; i<BufLen; i++) crc = update_crc8(Buf[i], crc);
  return (crc);
}
