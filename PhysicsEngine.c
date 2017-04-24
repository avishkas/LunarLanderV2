#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "EntityDefinition.h"

extern object entities[10];

uint8_t GRAVITY = 2;

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
	
	//get approximate square root s
	return getApproximateSquareRoot(distanceSquared);
}

void checkInteractions(uint32_t size){
	
}

void updateVelocity(int32_t xThrust, int32_t yThrust, uint32_t numberOfEntities){
	if(numberOfEntities > 1){
		checkInteractions(numberOfEntities);
	}
	entities[0].xVelocity = entities[0].xVelocity + xThrust;
	entities[0].yVelocity = entities[0].yVelocity + yThrust - GRAVITY;
}

void updatePosition(uint32_t size){	
	updateVelocity(getXThruster(), getYThruster(), size);
	entities[0].xPosition += entities[0].xVelocity;
	entities[0].yPosition += entities[0].yVelocity;
}

