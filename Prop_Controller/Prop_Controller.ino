volatile byte  count = 0;
byte numCount = 6; //number of pulse intervals to measure
 

volatile unsigned long startTime;
volatile unsigned long endTime;
unsigned long copy_startTime;
unsigned long copy_endTime;

volatile boolean finishCount = false;
float period;

unsigned int rpm = 0;

int hrelay = 10;    // power to relay reduce pitch
int lrelay = 9;     // power to relay increase pitch
int powerled = 11;  // power on led
int relayledh = 8;  // led on high relay 
int relayledl = 7;  // low relay led
int hled = 4;       // + limit led
int lled = 13;      // - limit led
int landSw = 12;    // Land/Cruise switch
int pitchIn = A6;   // pitch control in
int efis = A4;       // pitch setting out for efis

int diff = 100;     // difference of pitch control to rev to invoke if statement
int slow = 0;      // delay time
int led = 1000;     // led bright
int dim = 50;       // led dimmed

int pot;
int land;

void setup()
{
 Serial.begin(38400); 
 digitalWrite(hrelay, HIGH);
 digitalWrite(lrelay, HIGH);
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

  attachInterrupt(0, isrCount, FALLING);//interrupt on pin3
}

void loop() {
  {
  pot = (analogRead(pitchIn)*1.17+1500);     // pot set to pitch lever rpm setting *1.17+1500
  land = digitalRead(landSw);                // returns land switch state

  
  Serial.print("         Pitch in:  ");
  Serial.println(pot);
  }

{
  if (finishCount == true)
  {
    finishCount = false;//reset flag
    // disable interrupts, make protected copy of time values
    noInterrupts();
    copy_startTime = startTime;
    copy_endTime = endTime;
    count = 0;
    interrupts();

    period = (copy_endTime - copy_startTime) / 1000.0; //micros to millis
    //debug prints
    Serial.print(period); //total time for numCount
    Serial.print('\t');
    Serial.println(period/numCount);//time between individual pulses
   
    rpm = 60.0 * (1000.0 / period);
 
    Serial.print("RPM = ");
    Serial.println(rpm);
  }


  for(rpm; rpm <=1500;)                       // controller waits while rpm < 1500
  {
  digitalWrite(hrelay, LOW);
  digitalWrite(lrelay, HIGH);
  digitalWrite(relayledh, HIGH); 
  Serial.println("                      waiting for rpm");
  delay(slow); 
  }
  
  for(land; land == HIGH;)                    // controller changes prop pitch for lndg/tkoff
  {
  digitalWrite(hrelay, LOW);
  digitalWrite(lrelay, HIGH);
  digitalWrite(relayledl, HIGH);
  land = digitalRead(landSw);
  Serial.println("                            Landing");
  delay(slow);
  }
  
  if(rpm <= pot)
  {
  if(rpm <= pot-diff)                         // if prop rpm is lower than pitch lever setting 
  {
  digitalWrite(hrelay, LOW);                  // increase pitch
  digitalWrite(lrelay, HIGH);
  digitalWrite(relayledl, HIGH);              // turn on relay led
  Serial.println("                          Increasing rpm");
  pot = (analogRead(pitchIn)*1.17+1500);       // pot set to pitch lever rpm setting *1.17+1500
  delay(slow);
  }
  else if(rpm <= pot);
  {
  digitalWrite(hrelay,LOW);                    // remove power from relay
  digitalWrite(relayledh, LOW);
  delay(slow);
  }
  }
  
  if(rpm >= pot)
  {
  if(rpm >= pot+diff)                         // if prop rpm is greater than pitch lever setting
  { 
  digitalWrite(lrelay, LOW);                  // reduce pitch
  digitalWrite(hrelay, HIGH);
  digitalWrite(relayledh, HIGH);  
  Serial.println("                            decreasing rpm");
  pot = (analogRead(pitchIn)*1.17+1500);       // pot set to pitch lever rpm setting *1.17+1500
  delay(slow);
  }
  else if(rpm >= pot)
  {
  digitalWrite(lrelay,LOW);                    // remove power from relay
  digitalWrite(relayledl, LOW);
  delay(slow);
  }
  Serial.flush();
  }
 
}}


void isrCount()
{
  if (count == 0)//first entry to isr
  {
    startTime = micros();
  }

  if (count == numCount)
  {
    endTime = micros();
    finishCount = true;   
  }
  count++; //increment after test for numCount
}

