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
byte watchdog = 0;
volatile boolean finishCount = false;
float period, peakRPM;
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

 attachInterrupt(0, isrCount, FALLING);
}



void loop() {
  thisMillis = millis();                     // Read the time once
  pot = map(pitchIn,0,1023,1500,2700);
  //pot = (analogRead(pitchIn)*1.17+1500);     // pot set to pitch lever rpm setting *1.17+1500
  land = digitalRead(landSw);                // returns land switch state
  if(watchdog++ >250) watchdog = 100;

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

    if(copy_endTime > copy_startTime)         // check if endTime recycled
    {
    period = (copy_endTime - copy_startTime) / 1000.0; //micros to millis
    watchdog = 0;
    rpm = numCount*5.0 * (1000.0 / period);
    motor();
    msSinceRPMReading = thisMillis;           // set time for this reading
    if (rpm > peakRPM)
    {
      peakRPM = rpm;
    }
   
 }}

   else if (thisMillis - msSinceRPMReading > __WAIT_BEFORE_ZERO_RPM)      // Is rpm = 0
   {
     rpm = 0;                              // At least 2s since last RPM-sense, so assume zero RPMs
     msSinceRPMReading = thisMillis;           // Reset counter
    digitalWrite(hrelay, LOW);
    digitalWrite(lrelay, HIGH); 
    Serial.println("  Start your Engine, or Oh shit! its gone quiet"); 
 
   }
}
   if (watchdog >= 100)
   {
    digitalWrite(hrelay, LOW);
    digitalWrite(lrelay, LOW); 
    Serial.println("  JUMP ");
   }
   screen();
}













