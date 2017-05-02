#include <stdint.h>
#include "tm4c123gh6pm.h"

void Button_Init(){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO Port F	
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF0
	GPIO_PORTF_DIR_R &= ~(0x11);			// set all as input (0)
	GPIO_PORTF_AFSEL_R &= ~(0x11);		// turn off alternate function
	GPIO_PORTF_PUR_R |= 0x11;					// place pull up resistors for PF4 and PF0
	GPIO_PORTF_DEN_R |= 0x11;					// set digital enable on
}
