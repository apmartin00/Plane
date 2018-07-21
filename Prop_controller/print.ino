

void screen()
{                                               // Is it time to send the data?
   if (millis() - msSinceSend > __WAIT_BETWEEN_SENDS) 
   {
   // Yes: Build the serial output...
   Serial.print(rpm);                           // Current RPMs
   //Serial.print("|");                           // Field Separator
   //Serial.print(peakRPM);                       // Peak RPMs
   Serial.print("|");
   Serial.print(pot);                           // rpm setting

   // Debugging
    Serial.print("|");                           // Field Separator
    Serial.print(period);                       // period for last rpm calc

   // That'll do for bnow
   Serial.println();                            // EOL.

   msSinceSend = millis();                      // Reset the timer 
   }
}

