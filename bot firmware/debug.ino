void debug_values() {
  Serial.print("ERROR STATE: "); Serial.print(irLedFlag);  Serial.println("");
  Serial.print("X: "); Serial.print(xAvg.getAvg());  Serial.println("");
  Serial.print("Y: "); Serial.print(yAvg.getAvg());  Serial.println("");
  Serial.print("Z: "); Serial.print(zAvg.getAvg());  Serial.println("");
  Serial.println("");
  Serial.print("IR0: ");  Serial.print(IR0_SENSE);  Serial.println("");
  Serial.print("IR1: ");  Serial.print(IR1_SENSE);  Serial.println("");
  Serial.println("");
  Serial.print("3.3V RAIL:  ");  Serial.print(VOLTS_33V);  Serial.println("");
  Serial.print("5V RAIL:    ");  Serial.print(VOLTS_5V);  Serial.println("");
  Serial.print("BATT INPUT: ");  Serial.print(VOLTS_BAT);  Serial.println("");
  Serial.println("");
  Serial.print("RPM: ");  Serial.print(rpmAvg.getAvg());  Serial.println("");
  Serial.print("cRPM: ");  Serial.print(calculatedRPM);  Serial.println("");
  Serial.println("");
  
}
