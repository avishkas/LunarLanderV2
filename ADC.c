// ADC.c

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

// ADC initialization function Channel 1 (PE2)
// Input: none
// Output: none
void ADC_Init(void){ 
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x10;				//initialze clock for Port E
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTE_DIR_R &= ~(0x04);	//set PE2 to input
	GPIO_PORTE_DEN_R &= ~(0x04);	//set digital enable off for 0x02
	GPIO_PORTE_AFSEL_R |= 0x04;		//set alternate functions on
	GPIO_PORTE_AMSEL_R |= 0x04;		//set analog input on
	
	SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0 
//  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};   // good code, but not yet implemented in simulator
  delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
  delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
  delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
  delay = SYSCTL_RCGCADC_R;
  ADC0_PC_R &= ~0xF;              // 9) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 10) Sequencer 3 is lowest priority
	
  ADC0_ACTSS_R &= ~0x0008;        // 11) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 12) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 13) clear SS3 field
  ADC0_SSMUX3_R += 1;    //     set channel
  ADC0_SSCTL3_R = 0x0006;         // 14) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 15) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 16) enable sample sequencer 3
	
	//intialization for sequencer 2 using channel 9 (PE4)
	ADC0_ACTSS_R &= ~0x04;
	ADC0_EMUX_R &= ~0x0F00;
	ADC0_SSMUX2_R &= ~0x0F;
	ADC0_SSMUX2_R += 9;
	ADC0_SSCTL2_R = 0x06;
	ADC0_IM_R &= ~0x04;
	ADC0_ACTSS_R |= 0x04;
	
	ADC0_SAC_R = 0x04;

}

// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC1_Input(void){
	uint32_t data0;
	
  ADC0_PSSI_R = 0x0008;            
  while((ADC0_RIS_R&0x08)==0){}
		
  data0 = ADC0_SSFIFO3_R&0xFFF; 
		
  ADC0_ISC_R = 0x0008; 
		
  return data0;
		
}

// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC2_Input(void){
	uint32_t data1;
	
  ADC0_PSSI_R = 0x0004;            
  while((ADC0_RIS_R&0x04)==0){}
		
  data1 = ADC0_SSFIFO2_R&0xFFF; 
		
  ADC0_ISC_R = 0x0004; 
		
  return data1;
		
}

int32_t getYThrust(void){
	uint32_t data2 = ADC2_Input();
	if(data2 < 50){
		return 0;
	}else{
		return data2;
	}
}

int32_t getShipAngle(void){
	int32_t data3 = ADC1_Input();
	data3 = (((((((4095-data3)*1000)/4090)*18)/1000)*15)-45); //processes ADC input into value 0-180 in chunks of 18
	if(data3 < 0){
		data3 += 360;
	}
	return data3 ; //return a value between 0 and 180 to 
}
