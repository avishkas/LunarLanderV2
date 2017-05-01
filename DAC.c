// put implementations for functions, explain how it works
// put your names here, date

#include <stdint.h>
#include "tm4c123gh6pm.h"

void DAC_Init(){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R = 0x02; //turn clock on for port B
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTB_DIR_R |= 0x3F;
	GPIO_PORTB_DEN_R |= 0x3F;
	GPIO_PORTB_AFSEL_R &= ~(0x3F);	
}
