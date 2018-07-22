// use at own risk, much testing required with hardware.

volatile byte  count = 0;
byte numCount = 60; //number of pulse intervals to measure
volatile unsigned long startTime;
volatile unsigned long endTime;
unsigned long copy_startTime;
unsigned long copy_endTime;
unsigned long thisMillis;
unsigned long msSinceRPMReading = 0;
const unsigned int __WAIT_BEFORE_ZERO_RPM = 2000;     // Number of mS to wait for an rpm_sense before assunming RPM = 0.
unsigned long msSinceSend;                            // mSec when the last data was sent to the serial port
const unsigned int __WAIT_BETWEEN_SENDS = 1000;       // Number of mS to wait before sending a batch of data.
const unsigned int LOOP_TIME = 100;                   // millis loop time
unsigned int dT = 50000;
byte watchdog = 0;
volatile boolean finishCount = false;
volatile boolean msg1 = false;
volatile boolean msg2 = false;
volatile boolean msg3 = false;
volatile boolean msg4 = false;
volatile boolean msg5 = false;
float period, peakRPM;
unsigned int rpm = 50;
unsigned int time = thisMillis;
unsigned int lastTime = 0;


int hrelay = 10;    // power to relay reduce pitch
int lrelay = 9;     // power to relay increase pitch
int powerled = 11;  // power on led
int relayledh = 8;  // led on high relay 
int relayledl = 7;  // low relay led
int hled = 4;       // + limit led
int lled = 13;      // - limit led
int landSw = 12;    // Land/Cruise switch
int pitchIn = A6;   // pitch control in
int efis = 5;      // pitch setting out for efis

int diff = 100;     // difference of pitch control to rev to invoke if statement
int led = 1000;     // led bright
int dim = 50;       // led dimmed
int erpm;           // pitch setting to efis
int pot;
int land;

void setup()
{
 Serial.begin(38400); 
 digitalWrite(hrelay, HIGH);    // LOW is on
 digitalWrite(lrelay, HIGH);    // LOW is on
 pinMode(hrelay, OUTPUT); 
 pinMode(lrelay, OUTPUT); 
 pinMode(powerled,OUTPUT);
 pinMode(relayledh,OUTPUT);
 pinMode(relayledl,OUTPUT);
 pinMode(landSw, INPUT);
 pinMode(pitchIn, INPUT);
 pinMode(hled,OUTPUT);
 pinMode(lled,OUTPUT);
 pinMode(2,INPUT_PULLUP);
 analogWrite(powerled, dim);
 Serial.println("start...");

attachInterrupt(0, isrCount, FALLING);
}
void isrCount()
{
  if (count == 0)                             //first entry to isr
  {
    startTime = micros();
  }
  if (count == numCount)
  {
    endTime = micros();
    finishCount = true;
  }
  count++;                                     //increment after test for numCount
}

void motor()
{
  if (land == HIGH)
//for(land; land == HIGH;)                    // controller changes prop pitch for lndg/tkoff
  {
  digitalWrite(hrelay, LOW);
  digitalWrite(lrelay, HIGH);
//digitalWrite(relayledl, HIGH);
//land = digitalRead(landSw);
//Serial.println("                            Landing");
  msg5 == true;
  }
  
  if(rpm <= pot && land == LOW)
//if(rpm <= pot)
  {
  if(rpm <= pot-diff)                         // if prop rpm is lower than pitch lever setting 
  {
  digitalWrite(hrelay, LOW);                  // reduce pitch, increasing rpm
  digitalWrite(lrelay, HIGH);
//digitalWrite(relayledl, HIGH);            // turn on relay led
  msg4 = true;
//pot = map(pitchIn,0,1023,1500,2700);
//pot = (analogRead(pitchIn)*1.17+1500);    // pot set to pitch lever rpm setting *1.17+1500
  }
//else if(rpm <= pot);
  else
  {
  digitalWrite(hrelay,LOW);                   // remove power from relay
  //digitalWrite(relayledh, LOW);
  }
  }
  if(rpm >= pot && land == LOW)  
//if(rpm >= pot)
  {
  if(rpm >= pot+diff)                         // if prop rpm is greater than pitch lever setting
  { 
  digitalWrite(lrelay, LOW);                  // increase pitch, decreasing rpm
  digitalWrite(hrelay, HIGH);
//digitalWrite(relayledh, HIGH); 
  msg3 = true; 
//pot = map(pitchIn,0,1023,1500,2700);
//pot = (analogRead(pitchIn)*1.17+1500);      // pot set to pitch lever rpm setting *1.17+1500
  }
//else if(rpm >= pot)
  else {
    digitalWrite(lrelay,LOW);                    // remove power from relay
    digitalWrite(relayledl, LOW);
    }
}} 


void screen()
{                                               // Is it time to send the data?
   if (millis() - msSinceSend > __WAIT_BETWEEN_SENDS) 
   {                                            // Yes: Build the serial output...
   Serial.print(rpm);                           // Current RPMs
   //Serial.print("|");                         // Field Separator
   //Serial.print(peakRPM);                     // Peak RPMs
   Serial.print("|");
   Serial.print(pot);                           // rpm setting

// Debugging
    Serial.print("\t");                          // print a tab 
    Serial.print(period);                        // period for last rpm calc
    Serial.print("|");
    Serial.print(dT);                            // loop time

    if(msg1==true){Serial.print("|" "     Start your engine, or Oh Shit! its gone quiet");}
    if(msg2==true){Serial.print("|" "     JUMP!!!!");}
    if(msg3==true){Serial.print("|" "     increasing rpm");}
    if(msg4==true){Serial.print("|" "     decreasing rpm");}
    if(msg5==true){Serial.print("|" "     Landing");}
    Serial.println();                            // EOL.
    msSinceSend = millis();                      // Reset the timer 
   }
}


void loop() 
{
  thisMillis = millis();                          // Read the time once


  if (thisMillis - lastTime >= LOOP_TIME)
  {   
    dT = thisMillis - lastTime;
    lastTime = thisMillis;
    pot = analogRead(pitchIn);
    erpm = map(pot,0,1023,0,255);
    pot = map(pot,0,1023,1500,2600);
    analogWrite(efis, erpm);
//pot = (analogRead(pitchIn)*1.17+1500);         // pot set to pitch lever rpm setting *1.17+1500
   land = digitalRead(landSw);                     // returns land switch state
    
    msg2 = false;
    msg3 = false;
    msg4 = false;
    msg5 = false;

  if (finishCount == true)
  { 
    finishCount = false;                           //reset flag
    noInterrupts();                                // disable interrupts, make protected copy of time values
    copy_startTime = startTime;
    copy_endTime = endTime;
    count = 0;
    interrupts();
    period = (copy_endTime - copy_startTime) / 1000.0;              //micros to millis
    watchdog = 0;
    rpm = numCount*5.0 * (1000.0 / period);
    msSinceRPMReading = thisMillis;                                 // set time for this reading
    motor();
    if (rpm > peakRPM) {peakRPM = rpm;}
  }
    if (thisMillis - msSinceRPMReading > __WAIT_BEFORE_ZERO_RPM)      // Is rpm = 0
  {                                                                  // At least 2s since last RPM-sense, so assume zero RPMs
     rpm = 0;                             
     msSinceRPMReading = thisMillis;                                 // Reset counter
     if(watchdog < 10)
     {
       digitalWrite(hrelay, LOW);
       digitalWrite(lrelay, HIGH);
       msg1 = true; 
     }
    watchdog++;
   }
    if (watchdog >250){watchdog = 12; }  
    if (watchdog > 10)
    {
    digitalWrite(hrelay, LOW);
    digitalWrite(lrelay, LOW);
    msg2 = true; 
    msg1 = false;
    }
   
   screen(); 
}}













