// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

	//START// Written by Piotr Brodzik
#define GoWest         0
#define WaitWest       1
#define GoSouth        2
#define WaitSouth      3
#define GoPedestrians  4
#define DontWalk1      5
#define Blink1         6
#define DontWalk2      7
#define Blink2         8
 //END//

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
	//START// Written by Piotr Brodzik
void Init_clock(void);
void Init_PortB(void);
void Init_PortF(void);
void Init_PortE(void);
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);

struct State {
  unsigned long OutTraffic;
  unsigned long OutWalking;	
  unsigned long Time;
  unsigned long Next[8];
};

typedef const struct State ST; // define new data type based in struct State;

ST FSM[9] = {	// creating FMS
	{0x0C, 0x02, 50, {GoWest,	GoWest,	WaitWest,	WaitWest,	WaitWest,	WaitWest,	WaitWest,	WaitWest}},    // GoWest
	{0x14, 0x02, 50, {GoSouth,	GoSouth, GoSouth,	GoSouth,	GoPedestrians,	GoPedestrians,	GoPedestrians,	GoSouth}},			// WaitWest
	{0x21, 0x02, 50, {GoSouth,	WaitSouth,	GoSouth,	WaitSouth,	WaitSouth,	WaitSouth,	WaitSouth,	WaitSouth}},			// GoSouth
	{0x22, 0x02, 50, {GoWest,	GoWest,	GoWest,	GoWest,	GoPedestrians,	GoPedestrians,	GoPedestrians,	GoPedestrians}},			// WaitSouth
	{0x24, 0x08, 50, {GoPedestrians,	DontWalk1,	DontWalk1,	DontWalk1,	GoPedestrians,	DontWalk1,	DontWalk1,	DontWalk1}},			// GoPedestrians
	{0x24, 0x02, 20, {Blink1,	Blink1,	Blink1,	Blink1,	Blink1,	Blink1,	Blink1,	Blink1}},			// DontWalk1
	{0x24, 0x00, 20, {DontWalk2,	DontWalk2,	DontWalk2,	DontWalk2,	DontWalk2,	DontWalk2,	DontWalk2,	DontWalk2}},			// Blink1
	{0x24, 0x02, 20, {Blink2,	Blink2,	Blink2,	Blink2,	Blink2,	Blink2,	Blink2,	Blink2}},			// DontWalk2
	{0x24, 0x00, 20, {GoWest,	GoWest,	GoSouth,	GoWest,	GoWest,	GoWest,	GoSouth,	GoWest}}			// Blink2
};
	
	
	
	
unsigned long S;     // index to the current state
unsigned long Input;

//END//

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
		//START// Written by Piotr Brodzik
	SysTick_Init();
	Init_clock();
	Init_PortB();
  Init_PortF();
	Init_PortE();
	S = GoWest; // initialize state of the FMS
	
  EnableInterrupts();
	
  while(1){
		GPIO_PORTB_DATA_R = FSM[S].OutTraffic;  // set traffic lights
		GPIO_PORTF_DATA_R = FSM[S].OutWalking;  // set walking lights
    SysTick_Wait10ms(FSM[S].Time);
		Input = GPIO_PORTE_DATA_R & 0x07; 
    S = FSM[S].Next[Input]; 
  }
}

void Init_clock(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000032;   // Turn on the clock for Port B, E, and F
	delay = SYSCTL_RCGC2_R;         // Allow time for the clock to start
}


void Init_PortB(void){
	GPIO_PORTB_AMSEL_R &= ~0x3F;      // 1) Clear the PB5-0 bits in Port B AMSEL to disable analog. 
	GPIO_PORTB_PCTL_R &= 0x00000000;  // 2) GPIO Clear bit PCTL
	GPIO_PORTB_DIR_R |= 0x3F;         // 3) Set the Port B direction register so (0 = input; 1 = output)
	GPIO_PORTB_AFSEL_R &= ~0x3F;      // 4) Clear the PB5-0 bits in Port B AFSEL to disable alternate functions
	GPIO_PORTB_DEN_R |= 0x3F;         // 5) Set the PB5-0 bits in Port B DEN to enable digital
}


void Init_PortF(void){
	GPIO_PORTF_AMSEL_R &= ~0x0A;     // 1) Clear the PF1 and PF3 in Port F AMSEL to disable analog. 
	GPIO_PORTF_PCTL_R &= 0x00000000; // 2) GPIO Clear bit PCTL
	GPIO_PORTF_DIR_R |= 0x0A;        // 3) Set the Port F direction register so (0 = input; 1 = output)
	GPIO_PORTF_AFSEL_R &= ~0x0A;     // 4) Clear the PF1 and PF3 bits in Port F AFSEL to disable alternate functions
	GPIO_PORTF_DEN_R |= 0x0A;        // 5) Set the PF1 and PF3 bits in Port F DEN to enable digital
}

void Init_PortE(void){
	GPIO_PORTE_AMSEL_R &= ~0x07;      // 1) Clear the PE2-0 bits in Port E AMSEL to disable analog. 
	GPIO_PORTE_PCTL_R &= 0x00000000;  // 2) Clear the PE2-0 bit fields in Port E PCTL to configure as GPIO
	GPIO_PORTE_DIR_R &= ~0x07;        // 3) Set the Port E direction register so (0 = input; 1 = output)
	GPIO_PORTE_AFSEL_R &= ~0x07;      // 4) Clear the PE2-0 bits in Port E AFSEL to disable alternate functions
	GPIO_PORTE_DEN_R |= 0x07;         // 5) Set the PE2-0 bits in Port E DEN to enable digital
}


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
//END//
