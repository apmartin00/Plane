

void isrCount()
{
  if (count == 0)         //first entry to isr
  {
    startTime = micros();
  }

  if (count == numCount)
  {
    endTime = micros();
    finishCount = true;   
  }
  count++;                //increment after test for numCount
}


