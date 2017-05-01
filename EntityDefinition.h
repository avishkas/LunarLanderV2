#include <stdint.h>

typedef struct {
	uint32_t MASS;
	uint8_t gravityAffected; //0 = not affected by gravity, everything else = yes
	
	int32_t xVelocity;
	int32_t yVelocity;
	
	int32_t xPosition;
	int32_t yPosition;
	
	const unsigned short *image;
	
}object;
