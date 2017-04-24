#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "EntityDefinition.h"

extern object entities[10];

uint8_t GRAVITY = 2;
uint8_t shipOnGround = 0; //0 = ship is not on the ground, anything else = ship is on the ground

//get approximate square root of given value
//input: value to get square root of
//output: square rooted value
int32_t getApproximateSquareRoot(int32_t value){
	int32_t guess = 1;
	while(1){
		guess = (guess + value/guess)/2;
		int32_t error = (value/guess) - guess;
		if(error < 0)
			error *= -1;
		if(error <= 1){
			break;
		}
	}
	return guess;
}

//gets distance between two points using pythagorean theorem
//input: 2 points (each with an x and y cord)
//output: distance between points
int32_t getDistance(int32_t x1, int32_t y1, int32_t x2, int32_t y2){
	int32_t deltaX = x1 - x2;
	int32_t deltaY = y1 - y2;
	int32_t distanceSquared;
	
	deltaX *= deltaX;		// x^2
	deltaY *= deltaY;		// y^2
	
	distanceSquared = deltaX + deltaY; //x^2 + y^2
	
	//get approximate square root
	return getApproximateSquareRoot(distanceSquared);
}

//This function is only called when there exists a Black Hole somewhere on the map

//THIS IS NOT NEARLY FINISHED, FINISH IT!!!!!!!!!!!
void checkInteractions(uint32_t size){
	uint8_t i;
	for(i = 1; i < size; i++){
		int32_t approximateDistanceBetween = entities[i].xPosition - entities[0].xPosition;
		if(approximateDistanceBetween < 0){
			approximateDistanceBetween *= -1;
		}
		if(approximateDistanceBetween < 7500){
			int32_t actualDistanceBetween = getDistance(entities[0].xPosition, entities[0].yPosition, entities[i].xPosition, entities[i].yPosition);
			//int32_t gravityBetween = 
		}
	}
}


void updateVelocity(int32_t xThrust, int32_t yThrust, uint32_t numberOfEntities){
	if(numberOfEntities > 1){
		checkInteractions(numberOfEntities);
	}
	if(shipOnGround == 1 && yThrust == 0){
		entities[0].yVelocity = 0;
		entities[0].xVelocity = 0;
	}else if(shipOnGround == 1 && yThrust != 0){
		entities[0].yVelocity = entities[0].yVelocity + yThrust - GRAVITY;
		if(entities[0].yVelocity < 0){
			entities[0].yVelocity = 0;
		}
	}	else{
		//entities[0].xVelocity = entities[0].xVelocity + xThrust;
		entities[0].yVelocity = entities[0].yVelocity + yThrust - GRAVITY;
	}
	
}

void updatePosition(uint32_t size){	
	updateVelocity(getXThruster(), getYThruster(), size);
	entities[0].xPosition += entities[0].xVelocity;
	entities[0].yPosition += entities[0].yVelocity;
}

//checks if lunar lander has collided with terrain
//input: pointer to terrain array
//outpu: checks if ship has landed/collided with objects and sets values to if certain conditions are met
void collisionDetection(int32_t terrainHeight[]){
	if(shipOnGround == 0 && ((entities[0].yPosition/100)-6) <= terrainHeight[(entities[0].xPosition/100)-5] && ((entities[0].yPosition/100) - 6) <= terrainHeight[(entities[0].xPosition/100) + 5]){
		shipOnGround = 1;
	}
	if((entities[0].yPosition/100)-6 > terrainHeight[(entities[0].xPosition/100)-5]){
		shipOnGround = 0;
	}
}

