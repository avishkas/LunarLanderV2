#include <stdint.h>
#include "tm4c123gh6pm.h"

void Button_Init(){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x32; //turn on port E and port B and port F
	delay = SYSCTL_RCGCGPIO_R; 	
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO Port F	
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF0
	GPIO_PORTF_DIR_R &= ~(0x11);			// set all as input (0)
	GPIO_PORTF_AFSEL_R &= ~(0x11);		// turn off alternate function
	GPIO_PORTF_PUR_R |= 0x11;					// place pull up resistors for PF4 and PF0
	GPIO_PORTF_DEN_R |= 0x11;					// set digital enable on
	
	GPIO_PORTE_DIR_R &= ~(0x2);					//set pins 1 and 3 to 0 (input)
	GPIO_PORTE_DEN_R |= 0x2;
	GPIO_PORTE_AFSEL_R &= ~(0x2);
	GPIO_PORTE_AMSEL_R &= ~(0x2);
	
	GPIO_PORTB_DIR_R &= ~(0x80);
	GPIO_PORTB_DEN_R |= 0x80;
	GPIO_PORTB_AFSEL_R &= ~(0x80);
	GPIO_PORTB_AMSEL_R &= ~(0x80);
}
