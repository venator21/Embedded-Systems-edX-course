// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// START // Written by Piotr Brodzik
// manipulating DATA_R in a friendly way (only desired bits of the port are manipulated)
#define SW  (*((volatile unsigned long *)0x40024004))  // Define bit 0 of DATA_R of port E (PE0) which is SW  
#define LED (*((volatile unsigned long *)0x40024008))  // Define bit 4 of DATA_R port E (PE1) which is LED
// END //

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
// START // Written by Piotr Brodzik
void Init_PortE(void);
void Delay1ms(unsigned long msec);
// END //

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  
	
  EnableInterrupts();           // enable interrupts for the grader
  // START // Written by Piotr Brodzik
	Init_PortE();
	LED |= 0x02;           // start with LED ON (PE1)
  while(1){
		Delay1ms(100);
		if (SW) {            // check if switch(PE0) is pressed (positive logic)
			LED ^= 0x02;       // if pressed, toggle LED by XOR operation on PE1
		}
		else {	
			LED |= 0x02;       // if not pressed, set LED ON
		}
	}
}

void Init_PortE(void){ 
	volatile unsigned long delay;

	SYSCTL_RCGC2_R |= 0x10;               // enable clock for port E
	delay = SYSCTL_RCGC2_R;               // put a delay for clock to make it stable
	GPIO_PORTE_DIR_R |= 0x02;             // PE1 output(LED)
	GPIO_PORTE_DIR_R &= ~0x01;            // PE0 input (SW)
	GPIO_PORTE_AMSEL_R &= 0x03;           // disable analog function
	GPIO_PORTE_AFSEL_R &= 0x03;           // disable alternate functions
	GPIO_PORTE_PCTL_R = 0x00000000;       // disable alternate functions
	GPIO_PORTE_DEN_R |= 0x03;              // enable digital on PE1 and PE0
}

void Delay1ms(unsigned long msec){

	unsigned long count;
	
  while(msec > 0 ) {       // repeat while still halfsecs to delay
    count = 1333333/100;   // this number means 1 msec (approx.)
    while (count > 0) {
      count--;
    } 
    msec--;
  }

}
// END //
