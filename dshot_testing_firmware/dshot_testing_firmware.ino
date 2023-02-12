#define ESC0_OUT      3
#define ESC1_OUT      4

volatile word frame0 = 0b0000000000000000;  //dshot frame for esc0
volatile word frame1 = 0b0000000000000000;  //dshot frame for esc1

volatile unsigned int throttleSetting;

volatile int bitCount = 16;  //count of bits iterated in dshot sender

volatile bool telemetryCall0 = false;  //bool for calling telemetry
volatile bool telemetryCall1 = false;  //bool for calling telemetry
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
  
  timer2.begin(send_routine, 1000);  //periodic timer to call dshot every 0.1ms
  timer3.begin(read_telemetry, 1000);

  frame0 = generate_dshot_frame(0, false);  //test call 
  frame1 = generate_dshot_frame(0, false);  //test call

  delay(2000);

  //frame0 = generate_dshot_frame(1, true);  //test call 
  //delay(270);
  //frame0 = generate_dshot_frame(2, true);  //test call
  
}

void loop() {
  
  /*
  digitalWrite(LED_BUILTIN, HIGH);  //test rando stuff to occupy normal program space not in ISRs
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  frame0 = generate_dshot_frame(48, telemetryCall0);  //test call
  delay(1000);
  frame0 = generate_dshot_frame(100, telemetryCall0);  //test call
  delay(1000);
  for(int i = 100; i < 500; i++) {
    frame0 = generate_dshot_frame(i, telemetryCall0);  //test call
    delay(2);
  }
  delay(500);
  for(int i = 500; i > 100; i--) {
    frame0 = generate_dshot_frame(i, telemetryCall0);  //test call
    delay(2);
  }
  delay(500);
  frame0 = generate_dshot_frame(75, telemetryCall0);  //test call
  delay(1000);
  frame0 = generate_dshot_frame(48, telemetryCall0);  //test call
  delay(1000);
  //frame0 = frame0 = generate_dshot_frame(21, true);  //test call
  delay(10);
  //test = !test;  //flip test
  */
  
  frame0 = generate_dshot_frame(200, true);  //test call
  delay(1);
  frame0 = generate_dshot_frame(200, false);  //test call
  delay(49);
}

void read_telemetry (bool esc) {
  if(Serial1.available() > 9) {  //if there is 10 bytes available in the buffer
    Serial.println("SERIAL PACKET:");
    char telemBuffer[10];  //create temp buffer
    for(int i = 0; i < 10; i++){
      telemBuffer[i] = Serial1.read(); //read each byte and store in array
      Serial.print(i,DEC); Serial.print(":"); Serial.print(telemBuffer[i],DEC); Serial.print(" "); 
    }
    Serial.println("");

    float v = (telemBuffer[1] << 8) + telemBuffer[2];
    float volts = v / 100;
    float a = (telemBuffer[3] << 8) + telemBuffer[4];
    float amps = a / 100;
    int rpm = (telemBuffer[7] << 8) + telemBuffer[8];
    
    Serial.print("VOLTS: "); Serial.println(volts);
    Serial.print("AMPS: "); Serial.println(amps);
    Serial.print("RPM: "); Serial.println(rpm);
  }
  
}

void test_telemetry() {
  if(Serial1.available()) {
    Serial.println(Serial1.read());
  }
}

word generate_dshot_frame (word thr, bool telem) {  //generate dshot frames
  word dshotFrame = 0;  //zero the frame to start
  thr = constrain(thr,0,2047);  //constrain throttle input to acceptable range
  for (int i = 15; i > 4; i--) {  //loop through first 11 bits
    bitWrite(dshotFrame, i, (bitRead(thr, i - 5)));  //read each throttle bit and attach to frame 
  }
 
  if(telem == true) {  //if telemtry asked for
    bitSet(dshotFrame, 4); //set 00000000000*0000 bit to 1 for requesting telemetry
    telemetryCall0 = false;  //stop calling for telemetry after asking for it
    telemetryCall1 = false;  //stop calling for telemetry after asking for it
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
  /*
  Serial.print("FRAME: ");
  for (int i = 15; i > -1; i--) {
    Serial.print(bitRead(dshotFrame, i));
  }
  Serial.println("");
  */
  
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
  bitCount--;  //decrement bit count
  if(bitCount > -1) {
    //timer1.begin(send_routine, 1);  //start the next bit loop
    send_routine();
  }
  else {
    bitCount = 15;  //reset bit counter
    //timer1.end();  //stop the timer
  }
}