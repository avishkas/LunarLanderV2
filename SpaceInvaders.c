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
#include "DAC.h"
#include "trigLookUp.h"
#include "images.h"
#include "Timer0.h"
#include "Sound.h"

//Color Definitions
#define _LCDWidth			 160
#define backgroundColor 0x38A7
#define matrixColor			0x38A8 //just needs to look the same as backgroundColor but different in value

//Canvas and Terrain Properties
#define canvasSize 500
#define maxTerrainHeight 40
#define minTerrainHeight 0
#define environmentStepSize 3

//Spaceship Parameters
#define _SpaceshipHeight 20
#define _SpaceshipWidth 20

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

int32_t TerrainHeight[canvasSize];
int32_t windowLocation = 0;
int16_t previousAngle = 0;
int32_t terrainDeleteOffset = 1;
int8_t	gameOver = 0;

unsigned short bitmapMatrix[30][30];

object entities[10];
uint32_t numberOfEntities = 1;

//initialize Player starting on platform
//input: none
//output: modifies entities[] to add player to 0th index
void createPlayer(){
	entities[0].MASS = 100;
	entities[0].gravityAffected = 1;
	
	entities[0].xPosition = 1000;
	entities[0].yPosition = 1000;
	
	entities[0].xVelocity = 0;
	entities[0].yVelocity = 0;
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
		
		//check if you need to change to addition or subtraction
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
	
	//create random number of blackholes
	uint8_t numberOfBlackHoles = 2 + (Random() % 4);
	uint32_t spacing = canvasSize/(numberOfBlackHoles+1); //add 1 because the intial placing is based on numberrOfBlackHoles so the max i would equal the end border without the + 1
	for(i = 1; i < numberOfBlackHoles + 1; i++){
		int32_t xOffset = Random32() % 3000;
		
		//if the value is odd then make it negative, just to add some more randomness kinda
		if((xOffset & 0x1) == 1){
			xOffset *= -1;
		}
		entities[i].xPosition = (spacing*i) * 100 + xOffset;
		
		//set Y position (has to be higher than max terrain height i.e +5000)
		int32_t yOffset = Random32() % 3000;
		entities[i].yPosition = yOffset + 5000;
		
		entities[i].MASS = ((Random() % 10) + 1) *10000; 
		
		entities[i].image = blackhole;
		//numberOfEntities++;
		
	}
}

//****************************************************** updateWindowLocation ************************************************************************
//input:none
//output:updates windowLocation depending on location of ship
void updateWindowLocation(void){
	terrainDeleteOffset = 1;
	uint32_t windowLocationDelta = 0;
	if(((entities[0].xPosition/100) - windowLocation) > 50 && (windowLocation + 160 + (entities[0].xVelocity)/100) < 499  && entities[0].xVelocity >= 0){
		windowLocationDelta = (entities[0].xVelocity)/100;
		terrainDeleteOffset = windowLocationDelta;
		if(windowLocationDelta == 0){
			windowLocationDelta = 1;
			terrainDeleteOffset = 1;
		}
	}
	windowLocation += windowLocationDelta;
}

int32_t getNewY(int32_t shipAngle, uint8_t currentXPixel, uint8_t currentYPixel){
	int32_t xPortion = (((currentXPixel-(_SpaceshipWidth/2))* sinLookUp[shipAngle]));
	
	if(xPortion >= 0){
		if((xPortion % 10000) >= 5000){
			int32_t add = 10000 - (xPortion % 10000);
			xPortion += add;
		}
	}else{
		int32_t tempNum = xPortion;
		tempNum *= -1;
		if((tempNum % 10000) >= 5000){
			int32_t sub = 10000 - (tempNum % 10000);
			xPortion -= sub;
		}
	}
	
	xPortion /= 10000;
	
	int32_t yPortion = (((currentYPixel-(_SpaceshipHeight/2)) * cosLookUp[shipAngle]));
	
	if(yPortion >=0){
		if((yPortion % 10000) >= 5000){
			int32_t add = 10000 - (yPortion % 10000);
			yPortion += add;
		}
	}else{
		int32_t tempNum = yPortion;
		tempNum *= -1;
		if((tempNum % 10000) >= 5000){
			int32_t sub = 10000 - (tempNum % 10000);
			yPortion -= sub;
		}
	}
	
	yPortion /= 10000;
	
	return yPortion - xPortion;
}

int32_t getNewX(int32_t shipAngle, uint8_t currentXPixel, uint8_t currentYPixel){
	int32_t xPortion = (((currentXPixel - (_SpaceshipWidth/2)) * cosLookUp[shipAngle]));
	
	
	if(xPortion >= 0){
		if((xPortion % 10000) >= 5000){
			int32_t add = 10000 - (xPortion % 10000);
			xPortion += add;
		}
	}else{
		int32_t tempNum = xPortion;
		tempNum *= -1;
		if((tempNum % 10000) >= 5000){
			int32_t sub = 10000 - (tempNum % 10000);
			xPortion -= sub;
		}
	}
	
	xPortion /= 10000;
	
	
	
	
	int32_t yPortion = (((currentYPixel-(_SpaceshipHeight/2)) * sinLookUp[shipAngle]));
	
	if(yPortion >=0){
		if((yPortion % 10000) >= 5000){
			int32_t add = 10000 - (yPortion % 10000);
			yPortion += add;
		}
	}else{
		int32_t tempNum = yPortion;
		tempNum *= -1;
		if((tempNum % 10000) >= 5000){
			int32_t sub = 10000 - (tempNum % 10000);
			yPortion -= sub;
		}
	}
	
	yPortion /= 10000;
	
	
	return xPortion + yPortion;
}

//checks if pixels above, below, and to the side are same, and if so changes pixel to that color if that pixel is not same
void checkBitmapMatrix(){
	uint8_t i;
	uint8_t c;
	for(i = 1; i < 29; i++){
		for(c = 1; c < 29; c++){
			if(bitmapMatrix[c][i] == backgroundColor){
			if((bitmapMatrix[c][i+1] != matrixColor && (bitmapMatrix[c][i-1] != matrixColor) && (bitmapMatrix[c+1][i] != matrixColor && (bitmapMatrix[c-1][i] != matrixColor)))){
				if((bitmapMatrix[c][i+1] != backgroundColor && (bitmapMatrix[c][i-1] != backgroundColor) && (bitmapMatrix[c+1][i] != backgroundColor && (bitmapMatrix[c-1][i] != backgroundColor)))){
					bitmapMatrix[c][i] = 0xBDD7;
					}
				}
			}
		}
	}
}

void drawRotatedShip(int32_t shipAngle, int32_t thrust){
	uint8_t i;
	uint8_t c;
	shipAngle = ((shipAngle * - 1) + 270 ); //need to do this conversion in order for equation to work (positive theta rotates image clockwise not counterclockwise)
	if(shipAngle <0){
		shipAngle += 360;
	}
	//populates ship buffer with rotated coordinates
	for(i = 0; i < _SpaceshipHeight; i++){
		for(c = 0; c < _SpaceshipWidth; c++){
			int32_t newX = getNewX(shipAngle, c, i);
			int32_t newY =  getNewY(shipAngle, c, i);
			if(thrust == 0){
				bitmapMatrix[newX+14][newY+14] = ship_ThrustersDeactivated[(_SpaceshipHeight-1-i)*_SpaceshipHeight + c];
			}else{
				bitmapMatrix[newX+14][newY+14] = spaceship3[(_SpaceshipHeight-1-i)*_SpaceshipHeight + c];
			}
		}
	}
	
	//checks buffer to see if there are pixels missing 
	checkBitmapMatrix();
	
	//prints buffer to screen and sets printed buffer to black
	for(i = 0; i < 30; i++){
		for(c = 0; c < 30; c++){
			ST7735_DrawPixel(i + ((entities[0].yPosition/100)), c + ((entities[0].xPosition/100)) - windowLocation, bitmapMatrix[c][i]);
			bitmapMatrix[c][i] = backgroundColor;
		}
	}
	
}
//**********************************************************  paintShip ******************************************************************************
//input: none
//output: paints position of ship on screen relative to windowLocation
void paintShip(int32_t shipAngle, int32_t yThrust){
	previousAngle = shipAngle;
	drawRotatedShip(shipAngle, yThrust);	
}

//**********************************************************  paintEnvironment  **********************************************************************
//Paints environment on display, also paints black boundry along environment such that when camera pans, you don't have reset screen to black
//inputs: none
//outputs: outputs LCD to frame
void paintEnvironment(uint16_t color){
	int32_t i = windowLocation - terrainDeleteOffset;
	
	//paint Terrain
	if(i < 0)
		i = 0;
	
	for(; i < (windowLocation + _LCDWidth); i++){
		ST7735_DrawPixel(TerrainHeight[i], i - windowLocation, color);							//draw first pixel and boundary around 
		ST7735_DrawPixel(TerrainHeight[i], i - windowLocation + terrainDeleteOffset, backgroundColor);		//that pixel (only top, bot, and right sides)
		ST7735_DrawPixel(TerrainHeight[i] + terrainDeleteOffset, i - windowLocation, backgroundColor);
		ST7735_DrawPixel(TerrainHeight[i] - terrainDeleteOffset, i - windowLocation, backgroundColor);
		
		int32_t currentHeight = TerrainHeight[i];
		while(currentHeight > TerrainHeight[i+1]){
			ST7735_DrawPixel(--currentHeight, i - windowLocation, color);							
			ST7735_DrawPixel(currentHeight, i - windowLocation + terrainDeleteOffset, backgroundColor);
			ST7735_DrawPixel(currentHeight, i - windowLocation - terrainDeleteOffset, backgroundColor);
		}	
		while(currentHeight < TerrainHeight[i+1]){
			ST7735_DrawPixel(++currentHeight, i - windowLocation, color);
			ST7735_DrawPixel(currentHeight, i - windowLocation + terrainDeleteOffset, backgroundColor);
			ST7735_DrawPixel(currentHeight, i - windowLocation - terrainDeleteOffset, backgroundColor);
		}
	}
	
	//paint Space Objects (blackholes, asteroids)
	for(i = 1; i <= numberOfEntities; i++){
		ST7735_DrawBitmap((entities[i].yPosition/100), (entities[i].xPosition/100-windowLocation), entities[i].image, 20, 20);
	}
	
}

//****************************************************  SysTick Init and Handler  ********************************************************************

void SysTick_Init(void){
	NVIC_ST_CTRL_R &= ~0x01;
	NVIC_ST_RELOAD_R = 8000000; //set to interrupt at 40Hz
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;
	NVIC_ST_CTRL_R |= 0x07;
}

void matrixInit(void){
	int8_t i;
	int8_t c;
	for(i = 0; i < 30; i++){
		for(c = 0; c < 30; c++){
			bitmapMatrix[i][c] = matrixColor;
		}
	}
		
}

void SysTick_Handler(void){
	
	int32_t yThrust = getYThrust();
	int32_t shipAngle = getShipAngle();
	
	
	updatePosition(shipAngle, yThrust, numberOfEntities); //size of array, just set 0 for now 
	updateWindowLocation();
	paintShip(shipAngle, yThrust);
	paintEnvironment(ST7735_WHITE);
}

//************************************************************  MAIN  ********************************************************************************
int main(void){
	DisableInterrupts();
	DAC_Init();
	Random_Init(599);
  TExaS_Init();  												//set system clock to 80 MHz
  Random_Init(1);
	ADC_Init();
  Output_Init();
	SysTick_Init();
	
	/*//timer init
	void (*playSound)();
	playSound = &Sound_Play;
	Timer0_Init(playSound, 7256);//11.025kHZ sampling rate
	*/
	ST7735_SetRotation(2);
	
	matrixInit();
	

  ST7735_FillScreen(backgroundColor);    // set screen to backgroundColor
	populateTerrain(canvasSize);					//Populate Environment Array
	
	
	createPlayer();
	
	EnableInterrupts();
	
  while(1){
		if(gameOver == 1){
			break;
		}
  }
	DisableInterrupts();
	ST7735_FillScreen(backgroundColor);
	ST7735_SetCursor(4, 4);
	ST7735_OutString("Game Over");
	
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
