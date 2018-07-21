

void motor()
{
    
  for(land; land == HIGH;)                    // controller changes prop pitch for lndg/tkoff
  {
  digitalWrite(hrelay, LOW);
  digitalWrite(lrelay, HIGH);
  //digitalWrite(relayledl, HIGH);
  land = digitalRead(landSw);
  Serial.println("                            Landing");
  }
  
  if(rpm <= pot)
  {
  if(rpm <= pot-diff)                         // if prop rpm is lower than pitch lever setting 
  {
  digitalWrite(hrelay, LOW);                  // reduce pitch
  digitalWrite(lrelay, HIGH);
  //digitalWrite(relayledl, HIGH);              // turn on relay led
  Serial.println("                          Increasing rpm");
  //pot = map(pitchIn,0,1023,1500,2700);
  //pot = (analogRead(pitchIn)*1.17+1500);       // pot set to pitch lever rpm setting *1.17+1500
  }
  else if(rpm <= pot);
  {
  digitalWrite(hrelay,LOW);                    // remove power from relay
  //digitalWrite(relayledh, LOW);
  }
  }
  
  if(rpm >= pot)
  {
  if(rpm >= pot+diff)                         // if prop rpm is greater than pitch lever setting
  { 
  digitalWrite(lrelay, LOW);                  // increase pitch
  digitalWrite(hrelay, HIGH);
  //digitalWrite(relayledh, HIGH);  
  Serial.println("                            decreasing rpm");
  //pot = map(pitchIn,0,1023,1500,2700);
  //pot = (analogRead(pitchIn)*1.17+1500);       // pot set to pitch lever rpm setting *1.17+1500
  }
  else if(rpm >= pot)
  {
  digitalWrite(lrelay,LOW);                    // remove power from relay
  //digitalWrite(relayledl, LOW);
  }
  Serial.flush();
  }
 
}
