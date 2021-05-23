// Pezio Sensor Pin interrupt setup
#define INTERRUPT_PIN PCINT1  
#define INT_PIN PB1          

//Set measurement time in milliseconds
#define MEASUREMENT_TIME 15000

#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>

//Library for temperature sensor DS18B20
#include <OneWire.h>
//For I2C
#include <TinyWireM.h>
//For OLED
#include <TinyOzOLED.h>

//Temperature sensor setup
OneWire  ds(4);

void setup() {
  cli();                            // Disable interrupts during setup
  PCMSK |= (1 << INTERRUPT_PIN);    // Enable interrupt handler (ISR) for our chosen interrupt pin (PCINT1/PB1/pin 6)
  GIMSK |= (1 << PCIE);             // Enable PCINT interrupt in the general interrupt mask
  pinMode(INT_PIN, INPUT_PULLUP);   // Set our interrupt pin as input with a pullup to keep it stable   
  sei();

  //setup the oled
  OzOled.init();
  OzOled.clearDisplay();
  OzOled.setBrightness(150);
}
void loop() 
{
  unsigned long lastMeausreTime = 0;
  char charVal[10]; 
  float sum = 0.0;
  byte counter = 0;

  //Splash screen 1
  OzOled.clearDisplay();
  OzOled.printString("  ATTINY 85  ",0,0);
  OzOled.printString(" Thermometer!",0,2); 
  delay(5000);

  //Splash screen 2
  OzOled.clearDisplay();
  OzOled.printString(" Measuring in",0,0);
  OzOled.printString("  3 second  ",0,2);
  delay(3000);

  //Actual Measuring
  OzOled.clearDisplay();
  OzOled.printString("Measuring...",0,2);
  lastMeausreTime = millis();
  while(millis() - lastMeausreTime < 15000)
  {
    sum += readTempData();
    delay(100);
    counter++;
  }

  //Averaging the temperature value
  float avg = sum/counter;
  dtostrf(avg, 3, 2, charVal);

  //Display the value on the OLED
  OzOled.clearDisplay();
  OzOled.printBigNumber(charVal,0, 0, 5);
  OzOled.printChar('c',15,0);
  delay(5000);

  //Go to sleep
  goToSleep();
}
