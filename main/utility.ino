
//Function to readTemperature data
float readTempData()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8] = {0x28,0xBF,0x65,0x79,0xA2,0x19,0x3,0xC0};
  float celsius,fahrenheit;

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion,
  delay(1000); 
   
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0; 
  return celsius;

  /*fahrenheit = celsius * 1.8 + 32.0;   // remove celsius and uncomment this block to change the unit
  return fahrenheit; */
}

//Interrupt to wake up the ATTINY 85
ISR(PCINT0_vect)
{
  cli();
  sleep_disable();
  power_all_enable();
  OzOled.init();
  sei();  
}

void goToSleep()
  {
  OzOled.setPowerOff();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu(); 
  }  // end of goToSleep
