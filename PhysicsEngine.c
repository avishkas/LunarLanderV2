#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "EntityDefinition.h"

extern object entities[10];
extern int32_t TerrainHeight[500];

uint8_t GRAVITY = 0;
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
//THIS FUNCTION HAS NOT BEEN TESTED, TEST IT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void checkInteractions(uint32_t size){
	uint8_t i;
	for(i = 1; i < size; i++){
		int32_t xDistanceBetween = entities[i].xPosition - entities[0].xPosition;
		int32_t yDistanceBetween = entities[i].yPosition - entities[0].yPosition;
		
		if(xDistanceBetween < 7500){
			int32_t directDistanceBetween = getDistance(entities[0].xPosition, entities[0].yPosition, entities[i].xPosition, entities[i].yPosition);
			int32_t gravityBetween = entities[i].MASS/((directDistanceBetween/61)*(directDistanceBetween/61));		//divide by 61 because it makes it so that the gravity nice
			entities[0].xVelocity += ((1000*gravityBetween)*((xDistanceBetween*1000)/directDistanceBetween))/1000000;		//add x component of gravityBetween to xVelocity
			entities[0].yVelocity += ((1000*gravityBetween)*((yDistanceBetween*1000)/directDistanceBetween))/1000000;		//add y component of gravityBetween to yVelocity
		}
	}
}

//checks if lunar lander has collided with terrain
//input:	none
//output: checks if ship has landed/collided with objects and sets values to if certain conditions are met
void collisionDetection(){
	if(shipOnGround == 0 && ((entities[0].yPosition/100)-6) <= TerrainHeight[(entities[0].xPosition/100)-5] && ((entities[0].yPosition/100) - 6) <= TerrainHeight[(entities[0].xPosition/100) + 5]){
		shipOnGround = 1;
		entities[0].yVelocity = 0;
		entities[0].xVelocity = 0;
	}
	if((entities[0].yPosition/100)-6 > TerrainHeight[(entities[0].xPosition/100)-5]){
		shipOnGround = 0;
	}
}

void updateVelocity(int32_t xThrust, int32_t yThrust, uint32_t numberOfEntities){
	collisionDetection();
	if(numberOfEntities > 1){
		checkInteractions(numberOfEntities);
	}
	if(shipOnGround == 1 && yThrust == 0){				// if ship is on ground and not applying thrust, x and y velocities are 0
		entities[0].yVelocity = 0;
		entities[0].xVelocity = 0;
	}else if(shipOnGround == 1 && yThrust != 0){	// if ship is on ground and thrust is non-zero, then see if thrust is enough to launch, otherwise 
		if(yThrust - GRAVITY < 0){									// set y-velocity to zero
			entities[0].yVelocity = 0;
		}else{
			entities[0].yVelocity = entities[0].yVelocity + yThrust - GRAVITY;
		}
	}else{																				// if ship is not on ground, then change velocity depending on thrust and gravity
		entities[0].xVelocity = entities[0].xVelocity + xThrust;
		entities[0].yVelocity = entities[0].yVelocity + yThrust - GRAVITY;
	}
}

void updatePosition(uint32_t size){
	updateVelocity(getXThruster(), getYThruster(), size);
	entities[0].xPosition += entities[0].xVelocity;
	entities[0].yPosition += entities[0].yVelocity;
}
