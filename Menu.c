#include <stdint.h>
#include "EntityDefinition.h"
#include "ST7735.h"
#include "tm4c123gh6pm.h"
#include "Images.h"
#define backgroundColor 0x38A7

extern gameState currentGameState;
extern difficulty currentDifficulty;
extern uint32_t seed;

void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

void pollForRelease(void){
	while((GPIO_PORTF_DATA_R & 0x80)){
		seed += 1;
	}
}

void pollForRelease2(void){
	while((GPIO_PORTF_DATA_R & 0x08)){
		seed += 1;
	}
}

void pollForInput(void){
	difficulty selectedDifficulty = Easy;
	ST7735_DrawBitmap(106, 70, asteroidRight, 15, 15);
	
	while(1){
		if((GPIO_PORTB_DATA_R & 0x80)){
			Delay100ms(1);
			if((GPIO_PORTB_DATA_R & 0x80)){
				pollForRelease();
				selectedDifficulty = (selectedDifficulty + 1)%3;
				if(selectedDifficulty == Easy){
					ST7735_FillRect(103, 84, 15, 15, backgroundColor);	//is easy is selected, then delete the option the previous one was on
					ST7735_DrawBitmap(106, 70, asteroidRight, 15, 15);
				}else if (selectedDifficulty == Medium){
					ST7735_FillRect(104, 55, 15, 15, backgroundColor);
					ST7735_DrawBitmap(115, 85, asteroidRight, 15, 15);
				}else{
					ST7735_FillRect(115, 70, 15, 15, backgroundColor);
					ST7735_DrawBitmap(103, 99, asteroidRight, 15, 15);
				}
			}
		}
		if((GPIO_PORTE_DATA_R & 0x02)){
				currentDifficulty = selectedDifficulty;
				break;
		}
	}
}

void disableUneededInterrupts(){
	//Disable SysTick (updatePhysics) and Timer1 (asteroid timer)
	NVIC_ST_CTRL_R &= ~(0x01);
	TIMER1_CTL_R = ~(0x01); 
}

void startMenu(void){
	disableUneededInterrupts();
	ST7735_SetRotation(3);
	ST7735_FillScreen(backgroundColor);
	ST7735_DrawBitmap(17,100,menu,125,85);
	
	pollForInput();
}
