#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EntityDefinition.h"
#include "ST7735.h"

#define backgroundColor 0x38A7

extern object entities[10];
extern asteroid asteroidObjects[10];
extern int32_t TerrainHeight[500];
extern int8_t crashed;
extern int32_t windowLocation;
extern int16_t fuel;
extern uint32_t numberOfEntities;

//lookup tables instantiated

const int16_t sinLookUp[] = {
0, 174, 348, 523, 697, 871, 1045, 1218, 1391, 1564, 1736, 1908, 2079, 2249, 2419, 2588, 2756, 2923, 3090, 3255, 
3420, 3583, 3746, 3907, 4067, 4226, 4383, 4539, 4694, 4848, 5000, 5150, 5299, 5446, 5591, 5735, 5877, 6018, 6156, 6293, 
6427, 6560, 6691, 6820, 6946, 7071, 7193, 7313, 7431, 7547, 7660, 7771, 7880, 7986, 8090, 8191, 8290, 8386, 8480, 8571, 
8660, 8746, 8829, 8910, 8987, 9063, 9135, 9205, 9271, 9335, 9396, 9455, 9510, 9563, 9612, 9659, 9702, 9743, 9781, 9816, 
9848, 9876, 9902, 9925, 9945, 9961, 9975, 9986, 9993, 9998, 10000, 9998, 9993, 9986, 9975, 9961, 9945, 9925, 9902, 9876, 
9848, 9816, 9781, 9743, 9702, 9659, 9612, 9563, 9510, 9455, 9396, 9335, 9271, 9205, 9135, 9063, 8987, 8910, 8829, 8746, 
8660, 8571, 8480, 8386, 8290, 8191, 8090, 7986, 7880, 7771, 7660, 7547, 7431, 7313, 7193, 7071, 6946, 6819, 6691, 6560, 
6427, 6293, 6156, 6018, 5877, 5735, 5591, 5446, 5299, 5150, 4999, 4848, 4694, 4539, 4383, 4226, 4067, 3907, 3746, 3583, 
3420, 3255, 3090, 2923, 2756, 2588, 2419, 2249, 2079, 1908, 1736, 1564, 1391, 1218, 1045, 871, 697, 523, 348, 174, 
1, -174, -348, -523, -697, -871, -1045, -1218, -1391, -1564, -1736, -1908, -2079, -2249, -2419, -2588, -2756, -2923, -3090, -3255, 
-3420, -3583, -3746, -3907, -4067, -4226, -4383, -4539, -4694, -4848, -5000, -5150, -5299, -5446, -5591, -5735, -5877, -6018, -6156, -6293, 
-6427, -6560, -6691, -6819, -6946, -7071, -7193, -7313, -7431, -7547, -7660, -7771, -7880, -7986, -8090, -8191, -8290, -8386, -8480, -8571, 
-8660, -8746, -8829, -8910, -8987, -9063, -9135, -9205, -9271, -9335, -9396, -9455, -9510, -9563, -9612, -9659, -9702, -9743, -9781, -9816, 
-9848, -9876, -9902, -9925, -9945, -9961, -9975, -9986, -9993, -9998, -10000, -9998, -9993, -9986, -9975, -9961, -9945, -9925, -9902, -9876, 
-9848, -9816, -9781, -9743, -9702, -9659, -9612, -9563, -9510, -9455, -9396, -9335, -9271, -9205, -9135, -9063, -8987, -8910, -8829, -8746, 
-8660, -8571, -8480, -8386, -8290, -8191, -8090, -7986, -7880, -7771, -7660, -7547, -7431, -7313, -7193, -7071, -6946, -6819, -6691, -6560, 
-6427, -6293, -6156, -6018, -5877, -5735, -5591, -5446, -5299, -5150, -5000, -4848, -4694, -4539, -4383, -4226, -4067, -3907, -3746, -3583, 
-3420, -3255, -3090, -2923, -2756, -2588, -2419, -2249, -2079, -1908, -1736, -1564, -1391, -1218, -1045, -871, -697, -523, -348, -174, 0};
	
const int16_t cosLookUp[] = {
10000, 9998, 9993, 9986, 9975, 9961, 9945, 9925, 9902, 9876, 9848, 9816, 9781, 9743, 9702, 9659, 9612, 9563, 9510, 9455, 
9396, 9335, 9271, 9205, 9135, 9063, 8987, 8910, 8829, 8746, 8660, 8571, 8480, 8386, 8290, 8191, 8090, 7986, 7880, 7771, 
7660, 7547, 7431, 7313, 7193, 7071, 6946, 6819, 6691, 6560, 6427, 6293, 6156, 6018, 5877, 5735, 5591, 5446, 5299, 5150, 
5000, 4848, 4694, 4539, 4383, 4226, 4067, 3907, 3746, 3583, 3420, 3255, 3090, 2923, 2756, 2588, 2419, 2249, 2079, 1908, 
1736, 1564, 1391, 1218, 1045, 871, 697, 523, 348, 174, 6, -174, -348, -523, -697, -871, -1045, -1218, -1391, -1564, 
-1736, -1908, -2079, -2249, -2419, -2588, -2756, -2923, -3090, -3255, -3420, -3583, -3746, -3907, -4067, -4226, -4383, -4539, -4694, -4848, 
-4999, -5150, -5299, -5446, -5591, -5735, -5877, -6018, -6156, -6293, -6427, -6560, -6691, -6819, -6946, -7071, -7193, -7313, -7431, -7547, 
-7660, -7771, -7880, -7986, -8090, -8191, -8290, -8386, -8480, -8571, -8660, -8746, -8829, -8910, -8987, -9063, -9135, -9205, -9271, -9335, 
-9396, -9455, -9510, -9563, -9612, -9659, -9702, -9743, -9781, -9816, -9848, -9876, -9902, -9925, -9945, -9961, -9975, -9986, -9993, -9998, 
-10000, -9998, -9993, -9986, -9975, -9961, -9945, -9925, -9902, -9876, -9848, -9816, -9781, -9743, -9702, -9659, -9612, -9563, -9510, -9455, 
-9396, -9335, -9271, -9205, -9135, -9063, -8987, -8910, -8829, -8746, -8660, -8571, -8480, -8386, -8290, -8191, -8090, -7986, -7880, -7771, 
-7660, -7547, -7431, -7313, -7193, -7071, -6946, -6819, -6691, -6560, -6427, -6293, -6156, -6018, -5877, -5735, -5591, -5446, -5299, -5150, 
-5000, -4848, -4694, -4539, -4383, -4226, -4067, -3907, -3746, -3583, -3420, -3255, -3090, -2923, -2756, -2588, -2419, -2249, -2079, -1908, 
-1736, -1564, -1391, -1218, -1045, -871, -697, -523, -348, -174, -1, 174, 348, 523, 697, 871, 1045, 1218, 1391, 1564, 
1736, 1908, 2079, 2249, 2419, 2588, 2756, 2923, 3090, 3255, 3420, 3583, 3746, 3907, 4067, 4226, 4383, 4539, 4694, 4848, 
5000, 5150, 5299, 5446, 5591, 5735, 5877, 6018, 6156, 6293, 6427, 6560, 6691, 6819, 6946, 7071, 7193, 7313, 7431, 7547, 
7660, 7771, 7880, 7986, 8090, 8191, 8290, 8386, 8480, 8571, 8660, 8746, 8829, 8910, 8987, 9063, 9135, 9205, 9271, 9335, 
9396, 9455, 9510, 9563, 9612, 9659, 9702, 9743, 9781, 9816, 9848, 9876, 9902, 9925, 9945, 9961, 9975, 9986, 9993, 9998, 10000};

uint8_t GRAVITY = 4;
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

//this method checks the interactions between blackholes and the ship and changes the velocity accordingly, also checks if ship has collided with blackhole
//input: number of entities on map
//output: modifies ship xVelocity and ship yVelocity
void checkInteractions(uint32_t numberOfEntities){
	uint8_t i;
	
	//collision has occured
	for(i = 0; i < numberOfEntities; i++){
		if(((entities[0].xPosition/100 + 22) < entities[i].xPosition/100 - 7 && (entities[0].xPosition/100 + 22) > (entities[i].xPosition/100 - 25)) || 
			((entities[0].xPosition/100 + 8) < entities[i].xPosition/100 - 7 && (entities[0].xPosition/100 + 8) > (entities[i].xPosition/100 - 25))){
			
			if(((entities[0].yPosition/100 + 21) > entities[i].yPosition/100 + 3 && (entities[0].yPosition/100 + 21) < entities[i].yPosition/100 + 22) ||
				((entities[0].yPosition/100 + 6) > entities[i].yPosition/100 + 3 && (entities[0].yPosition/100 + 6) < entities[i].yPosition/100 + 22)){
					crashed = 1;			
				}
				
		}
	}
	
	//if ship has not crashed, then change velocity based on blackhole and ship position 
	for(i = 1; i < numberOfEntities; i++){
		int32_t xDistanceBetween = (entities[i].xPosition+1000) - (entities[0].xPosition + 1500);
		int32_t yDistanceBetween = (entities[i].yPosition+1000) - (entities[0].yPosition + 1500);
		
		if(xDistanceBetween < 7500){
			int32_t directDistanceBetween = getDistance(entities[0].xPosition + 1500, entities[0].yPosition + 1500, entities[i].xPosition, entities[i].yPosition);
			int32_t gravityBetween = entities[i].MASS/((directDistanceBetween/61)*(directDistanceBetween/61));		//divide by 61 because it makes it so that the gravity nice
			entities[0].xVelocity += ((1000*gravityBetween)*((xDistanceBetween*1000)/directDistanceBetween))/1000000;		//add x component of gravityBetween to xVelocity
			entities[0].yVelocity += ((1000*gravityBetween)*((yDistanceBetween*1000)/directDistanceBetween))/1000000;		//add y component of gravityBetween to yVelocity
		}
	}
	
	
	
}

//checks if lunar lander has collided with terrain
//input:	none
//output: checks if ship has landed/collided with objects and sets values to if certain conditions are met
void collisionDetection(int32_t shipAngle){
	//if ship has landed
	if(shipOnGround == 0 && ((entities[0].yPosition/100)+7) <= TerrainHeight[(entities[0].xPosition/100)+8] && ((entities[0].yPosition/100) + 7) <= TerrainHeight[(entities[0].xPosition/100) + 22] && shipOnGround == 0){
		if(shipAngle == 90 && entities[0].yVelocity <= 50 && entities[0].yVelocity >= -60){
			shipOnGround = 1;
			entities[0].yVelocity = 0;
			entities[0].xVelocity = 0;
		}else{
			crashed = 1;
		}
	}
	
	//ships y and x position is at botttom left corner of buffer
	//ship has crashed, if a bottom corner of the ship is below the terrain and the other isnt, then you're ship has crashed
	if(entities[0].xPosition/100 + 22 < 500){
		if((entities[0].yPosition/100 + 6 <= TerrainHeight[(entities[0].xPosition/100) + 8]) && entities[0].yPosition/100 + 6 > TerrainHeight[(entities[0].xPosition)/100 + 22]){
			crashed = 1;
		}
		if((entities[0].yPosition/100 + 6 <= TerrainHeight[(entities[0].xPosition/100) + 22]) && entities[0].yPosition/100 + 6 > TerrainHeight[(entities[0].xPosition)/100 + 8]){
			crashed = 1;
		}
	}
	
	//ship is about to take off
	if((entities[0].yPosition/100)+7 > TerrainHeight[(entities[0].xPosition/100) + 15] && shipOnGround == 1){
		shipOnGround = 0;
	}
	
	//check if ship has flown off map
	if(entities[0].yPosition > 14000 || entities[0].xPosition > 500000){
		crashed = 2;
	}
	
}

int32_t getXComponentVelocity(int32_t angle, int32_t yThrust){
	return (yThrust*cosLookUp[angle])/4000000;
}

int32_t getYComponentVelocity(int32_t angle, int32_t yThrust){
	return (yThrust*sinLookUp[angle]/4000000);
}

void updateVelocity(int32_t shipAngle, int32_t yThrust, uint32_t numberOfEntities){
	checkInteractions(numberOfEntities);
	if(shipOnGround == 0){
		entities[0].xVelocity += getXComponentVelocity(shipAngle, yThrust);
		entities[0].yVelocity += getYComponentVelocity(shipAngle, yThrust)- GRAVITY;
	}else{
		entities[0].yVelocity += getYComponentVelocity(shipAngle, yThrust);
		entities[0].xVelocity += getXComponentVelocity(shipAngle, yThrust);
	}
}


void updateAsteroids(){
	uint8_t i;
	
	//check if asteroid has collided with player ship, if not update that Ateroid's velocity
	for(i = 0; i < 10; i++){
		if(asteroidObjects[i].active == 1){
			ST7735_FillRect((asteroidObjects[i].yPosition/100), (asteroidObjects[i].xPosition/100) - 15 - windowLocation, 15, 15, backgroundColor);
			asteroidObjects[i].xPosition += asteroidObjects[i].xVelocity;
			asteroidObjects[i].yPosition += asteroidObjects[i].yVelocity;
			
			if(((asteroidObjects[i].yPosition/100 > (entities[0].yPosition/100 + 6)) && (asteroidObjects[i].yPosition/100 < entities[0].yPosition/100 + 22))||
				(asteroidObjects[i].yPosition/100 + 15 > entities[0].yPosition/100 + 6 && asteroidObjects[i].yPosition/100 + 15 < entities[0].yPosition/100 + 22)){
				if((asteroidObjects[i].xPosition/100) > (entities[0].xPosition/100 + 8) && ((asteroidObjects[i].xPosition/100) < (entities[0].xPosition/100 + 22))){
					asteroidObjects[i].active = 0;
					crashed = 1;
				}
				if(((asteroidObjects[i].xPosition/100 - 15) < (entities[0].xPosition/100 + 22)) && asteroidObjects[i].xPosition/100 - 15 > (entities[0].xPosition/100 + 8)){
					asteroidObjects[i].active = 0;
					crashed = 1;
				
			}
			if(asteroidObjects[i].yPosition/100 < TerrainHeight[asteroidObjects[i].xPosition/100]){
				asteroidObjects[i].active = 0;
			}
			asteroidObjects[i].yVelocity -= GRAVITY;
		}
	//check If Asteroids have collided with Black Hole

			uint8_t c;
			for(c = 1; c < numberOfEntities; c++){
				if((asteroidObjects[i].xPosition/100 > entities[c].xPosition/100 - 25 && asteroidObjects[i].xPosition/100 < entities[c].xPosition/100 - 7) || 
					(asteroidObjects[i].xPosition/100 - 15 > entities[c].xPosition/100 - 25 && asteroidObjects[i].xPosition/100 < entities[c].xPosition/100 -7)){
						
						if(asteroidObjects[i].yPosition/100 < entities[c].yPosition/100 + 22 && asteroidObjects[i].yPosition/100 + 15 > entities[c].yPosition/100){
							asteroidObjects[i].active = 0;
						}
					} 
			}			
		}
	}
}
void checkShipLanded(){
	if(shipOnGround == 1 && entities[0].xPosition > 40000){
		crashed = 3;
	}
}

void updateFuel(uint32_t yThrust){
	fuel = fuel - (yThrust*3)/2000;
}

void updatePosition(int32_t shipAngle, int32_t yThrust, uint32_t numberOfEntities){
	checkShipLanded();
	updateVelocity(shipAngle, yThrust, numberOfEntities);
	updateAsteroids();
	updateFuel(yThrust);
	collisionDetection(shipAngle);
	entities[0].xPosition += entities[0].xVelocity;
	entities[0].yPosition += entities[0].yVelocity;
	
}
