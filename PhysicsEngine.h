#include <stdint.h>

void updatePosition(uint32_t numberOfEntities, int32_t shipAngle, int32_t yThrust);

int32_t getDistance(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

void collisionDetection(void);

void updateAsteroids(void);
