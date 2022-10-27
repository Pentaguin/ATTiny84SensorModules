#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

// Must be defined before AttinySerialOut.hpp
#define TX_PIN PB1
#include <ATtinySerialOut.hpp>

//for setting register bits with AVR code
//cbi and sbi are standard (AVR) methods for setting, or clearing, bits in PORT (and other) variables. 
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define reed_sensor PB0

ISR(PCINT1_vect) // Interrupt service routine     
{
  // Do nothing. just wake up.
}

void set_port_pins_direction()
{
  initTXPin(); // Set TXpin as output
  cbi(DDRB,reed_sensor);  // set reed_sensor as input.
  sbi(PORTB,reed_sensor); // and set as pull up.
}

void init_interrupt()
{
  sei();     // Enabling global interrupt.
  sbi(GIMSK,PCIE1); // Enable Pin Change Interrupt for PCINT 11 to 8.
  sbi(PCMSK1,PCINT8); // Set Pin Change Interrupt for PB0.
}

void go_sleep()
{
  cbi(ADCSRA,ADEN); // Disable ADC.
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set deep sleep mode.
  sleep_mode(); // Put chip to sleep.
  
  // Program starts here after waking from sleep
  sbi(ADCSRA,ADEN); //enable ADC
}

void read_sensor()
{
  if(PINB & (1 << reed_sensor)) // Read value from Reed sensor. Door is open.
    { 
      Serial.write('1');
    }
    else
    {
      Serial.write('0');
    }
}

int main(void)
{
  set_port_pins_direction();
  init_interrupt();
  
  while(1)
  {
    go_sleep();
    read_sensor();
  }
}

