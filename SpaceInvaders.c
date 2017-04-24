// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "PhysicsEngine.h"
#include "EntityDefinition.h"

//Color Definitions
#define _LCDWidth			 160
#define ST7735_BLUE    0xF800
#define ST7735_RED     0x001F
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0xFFE0
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0x07FF
#define ST7735_WHITE   0xFFFF

//Canvas and Terrain Properties
#define canvasSize 500
#define maxTerrainHeight 40
#define minTerrainHeight 0
#define environmentStepSize 3

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

int32_t TerrainHeight[canvasSize];
int32_t windowLocation = 0;

// *************************** Capture image dimensions out of BMP**********
const unsigned short spaceship[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000,
 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000,

};

// basic structure which all interacting objects are made of (spaceship, black hole, other shit)


object entities[10];
uint32_t numberOfEntities = 0;

//initialize Player starting on platform
//input: none
//output: modifies entities[] to add player to 0th index
void createPlayer(){
	entities[0].MASS = 100;
	entities[0].gravityAffected = 1;
	
	entities[0].xPosition = 2100;
	entities[0].yPosition = 2100;
	
	entities[0].xVelocity = 0;
	entities[0].yVelocity = 0;
	
	numberOfEntities = 1;
}
//**************************************************************  populateTerrain  *******************************************************************
//Populate TerrainHeight[] using terrain generation algorithm
//input: length of canvas
//output: modifies TerrainHeight
void populateTerrain(uint32_t canvasLength){
	uint32_t i;
	uint8_t addORsub = 0; //0 = add, 1 = sub
	
	//make 0-20 flat such that it acts as starting pad
	
	for(i = 0; i <90; i++){
		TerrainHeight[i] = 10;
	}
	
	//generate actual terrain
	for(;i < canvasSize - 20; i++){
		int32_t r = Random();
		r %= environmentStepSize;
		
		//check if you need to add or subtract
		if(TerrainHeight[i-1] == maxTerrainHeight || TerrainHeight[i-1] == minTerrainHeight || i%40 == 24){
			addORsub ^= 0x01;
		}
		
		//populate terrain using random step size 
		if(addORsub == 0){
			if(TerrainHeight[i-1] + r > maxTerrainHeight){
				TerrainHeight[i] = maxTerrainHeight;
			}else{
				TerrainHeight[i] = TerrainHeight[i-1] + r;
			}
		}else{
			if(TerrainHeight[i-1] - r < minTerrainHeight){
				TerrainHeight[i] = 0;
			}else{
				TerrainHeight[i] = TerrainHeight[i-1] - r;
			}
		}
	}
	
	//generate landing platform at end
	for(; i < canvasSize; i++){
		TerrainHeight[i] = 10;
	}
}
//**********************************************************  paintEnvironment  **********************************************************************
//Paints environment on display, also paints black boundry along environment such that when camera pans, you don't have reset screen to black
//inputs: none
//outputs: outputs LCD to frame
void paintEnvironment(uint16_t color){
	int32_t i;
	for(i = windowLocation; i < (windowLocation + _LCDWidth); i++){
		ST7735_DrawPixel(TerrainHeight[i], i - windowLocation, color);							//draw first pixel and boundary around 
		ST7735_DrawPixel(TerrainHeight[i], i - windowLocation +1, ST7735_BLACK);		//that pixel (only top, bot, and right sides)
		ST7735_DrawPixel(TerrainHeight[i] + 1, i - windowLocation, ST7735_BLACK);
		ST7735_DrawPixel(TerrainHeight[i] - 1, i - windowLocation, ST7735_BLACK);
		
		int32_t currentHeight = TerrainHeight[i];
		while(currentHeight > TerrainHeight[i+1]){
			ST7735_DrawPixel(--currentHeight, i - windowLocation, color);							
			ST7735_DrawPixel(currentHeight, i - windowLocation + 1, ST7735_BLACK);
			ST7735_DrawPixel(currentHeight, i - windowLocation - 1, ST7735_BLACK);
		}	
		while(currentHeight < TerrainHeight[i+1]){
			ST7735_DrawPixel(++currentHeight, i - windowLocation, color);
			ST7735_DrawPixel(currentHeight, i - windowLocation + 1, ST7735_BLACK);
			ST7735_DrawPixel(currentHeight, i - windowLocation - 1, ST7735_BLACK);
		}
	}
	ST7735_DrawFastHLine(0,0,128, ST7735_BLACK);
	ST7735_DrawFastHLine(0,159,128, ST7735_BLACK);
}

//****************************************************  SysTick Init and Handler  ********************************************************************

void SysTick_Init(void){
	NVIC_ST_CTRL_R &= ~0x01;
	NVIC_ST_RELOAD_R = 4000000; //set to interrupt at 40Hz
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R |= 0x07;
}
void SysTick_Handler(void){
	paintEnvironment(ST7735_WHITE);
	ST7735_FillRect((entities[0].yPosition/100) - 5, (entities[0].xPosition/100) - 5, 10, 10, ST7735_BLACK);
	updatePosition(numberOfEntities);
	ST7735_DrawBitmap((entities[0].yPosition/100) - 5, (entities[0].xPosition/100) + 5, spaceship, 10, 10);
	collisionDetection(TerrainHeight);
}



//************************************************************  MAIN  ********************************************************************************
int main(void){
	DisableInterrupts();
	Random_Init(599);
  TExaS_Init();  												//set system clock to 80 MHz
  Random_Init(1);
	ADC_Init();
  Output_Init();
	SysTick_Init();
	
  ST7735_FillScreen(0x0000);            // set screen to black
	populateTerrain(canvasSize);					//Populate Environment Array
	
	createPlayer();												
	
	EnableInterrupts();
	
  while(1){
		
		
		
	//	windowLocation--;
  }

}


// You can use this timer only if you learn how it works

void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
