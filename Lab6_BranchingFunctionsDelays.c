// BranchingFunctionsDelays.c Lab 6
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  
// This lab will use the hardware already built into the LaunchPad.
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

// START // Written by Piotr Brodzik
// manipulating DATA_R in a friendly way (only desired bits of the port are manipulated)
#define LED (*((volatile unsigned long *)0x40025010)) // Define bit 2 of DATA_R of port F (PF2) which is blue LED  
#define SW1 (*((volatile unsigned long *)0x40025040)) // Define bit 4 of DATA_R port F (PF4) which is Switch 1
// END //

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// START // Written by Piotr Brodzik
void Delay(void);
// END //

int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
  // initialization goes here
	// START // Written by Piotr Brodzik
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // enable clock for port F
	delay = SYSCTL_RCGC2_R;               // put a delay for clock to make it stable
	GPIO_PORTF_AMSEL_R = 0x00;            // disable analog function for port F
	GPIO_PORTF_AFSEL_R = 0x00;            // disable alternate functions
	GPIO_PORTF_PCTL_R = 0x00000000;       // disable alternate functions
	GPIO_PORTF_PUR_R = 0x10;              // enable pull-up restitors on PF4 (SW1)
	GPIO_PORTF_DEN_R = 0x14;              // enable digital on PF4 and PF2
	GPIO_PORTF_DIR_R |= 0x04;             /* sets PF4 which is bit 4 (SW1) to 0 in order to make SW1 an input and
																				   sets PF2 which is bit 2 (blue LED) to 1 in order to make is an output*/
	
	LED |= 0x04; 													// Sets PF2 (blue LED) to 1 to start with LED ON
	// END //
	
  EnableInterrupts();           // enable interrupts for the grader
  // START // Written by Piotr Brodzik
  while(1){
		Delay();
		if (!(SW1)) {  // check if switch 1 is pressed (negative logic)
			LED ^= 0x04; // if pressed, toggle LED by XOR operation on PF2
		}
		else {	
			LED |= 0x04; // if not pressed, set LED ON
		}
	}
}

void Delay(void){unsigned long volatile time;
  time = 1033333;  // 1333333 means 100ms
  while(time){
		time--;
  }
} 
	// END //
