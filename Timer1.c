// Timer1.c
// Runs on LM4F120/TM4C123
// Use TIMER1 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Last Modified: 3/6/2015 
// You can use this timer only if you learn how it works

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
  Program 7.5, example 7.6

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "entityDefinition.h"
#include "ST7735.h"
#include "Random.h"
#include "Images.h"

uint32_t spawnRate;			//in milliseconds
uint32_t currentTime = 0;
uint8_t currentPosition = 0;
uint8_t currentAnimation = 0;

extern asteroid asteroidObjects[10];
extern int32_t windowLocation;

void (*PeriodicTask1)(void);   // user function

// ***************** TIMER1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 80000;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R |= 1<<21; //0x100000; // 1<<21           // 9) enable IRQ 21 in NVIC
  //TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}


void spawnAsteroid(){
	uint32_t xPos = (Random32() % 12000) + (windowLocation*100) + 2000;
	
	if((xPos/100 - windowLocation) < 80){
		asteroidObjects[currentPosition % 10].yPosition = 8000;
	}else{
		asteroidObjects[currentPosition % 10].yPosition = 11300;
	}
	

	
	asteroidObjects[currentPosition % 10].xPosition = xPos;
	
	int32_t xVel = ((Random() % 5)*10) + 100;
	int32_t yVel = ((Random() % 70) + 1) * -1;
	
	if((xPos/100 - windowLocation) >= 80){
		xVel *= -1;
		asteroidObjects[currentPosition % 10].image = asteroidLeft;
	}else{
		asteroidObjects[currentPosition % 10].image = asteroidRight;
	}
	asteroidObjects[currentPosition % 10].xVelocity = xVel;
	asteroidObjects[currentPosition % 10].yVelocity = yVel;
	
	asteroidObjects[currentPosition % 10].active = 0;
	
	currentPosition = (currentPosition + 1) % 10;
}

void checkToSpawnAsteroid(){
	if(currentTime >= spawnRate){
		const unsigned short *image;
		if(currentTime == spawnRate){
			spawnAsteroid();						//increments currentPosition by 1
		}
		
		uint8_t tempPosition;
		
		if(currentPosition == 0){		//circular buffer
			tempPosition = 9;
		}else{
			tempPosition = currentPosition - 1;
		}
		
		if(asteroidObjects[tempPosition].xVelocity > 0){
			image = asteroidRightWhite;
		}else{
			image = asteroidLeftWhite;
		}
		
		switch (currentAnimation){
			//offsets of -7 and +5 because these indicator images are bigger than actual images
			case 0:
				ST7735_DrawBitmap(asteroidObjects[tempPosition].yPosition/100 - 7, asteroidObjects[tempPosition].xPosition/100 - windowLocation + 5, image, 20, 20);
				if((currentTime - spawnRate) > 350){
					currentAnimation++;
				}					
				break;
			case 1:
				ST7735_DrawBitmap(asteroidObjects[tempPosition].yPosition/100 - 7, asteroidObjects[tempPosition].xPosition/100 - windowLocation + 5, asteroidBlank, 20, 20);
				if((currentTime - spawnRate) > 700){
					currentAnimation++;
				}					
				break;
			case 2:
				ST7735_DrawBitmap(asteroidObjects[tempPosition].yPosition/100 - 7, asteroidObjects[tempPosition].xPosition/100 - windowLocation + 5, image, 20, 20);	
			if((currentTime - spawnRate) > 1050){
					currentAnimation++;
				}					
				break;
			case 3:
				ST7735_DrawBitmap(asteroidObjects[tempPosition].yPosition/100 - 7 , asteroidObjects[tempPosition].xPosition/100 - windowLocation + 5, asteroidBlank, 20, 20);
				if((currentTime - spawnRate) > 1400){
					currentAnimation++;
					asteroidObjects[tempPosition].active = 1;
				}
				break;
		}
	}
	if(currentAnimation == 4){
		currentAnimation = 0; 
		currentTime = 0;
	}
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	
	currentTime++;
	checkToSpawnAsteroid();
}

void setSpawnRate(uint32_t rateOfSpawn){
	spawnRate = rateOfSpawn;
}

