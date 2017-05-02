#include <stdint.h>

typedef struct {
	uint32_t MASS;
	int32_t shipAngle;
	
	int32_t xVelocity;
	int32_t yVelocity;
	
	int32_t xPosition;
	int32_t yPosition;
	
	
	const unsigned short *image;
	
	
}object;

typedef struct{
	int32_t xPosition;
	int32_t yPosition;
	
	int32_t xVelocity;
	int32_t yVelocity;
	
	uint8_t active;
	
	const unsigned short *image;
}asteroid;

typedef enum {Menu, Gameplay} gameState;
typedef enum {Easy, Medium, Hard} difficulty;
