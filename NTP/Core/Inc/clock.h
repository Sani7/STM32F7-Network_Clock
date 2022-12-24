#pragma once

#define __USE_MISC
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32746g_discovery_lcd.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 260
#define min(x, y) ((x) < (y) ? (x) : (y))
#define Radius (min(SCREEN_HEIGHT, SCREEN_WIDTH) / 2 - 1)

#define X_CENTER (SCREEN_WIDTH / 2)
#define Y_CENTER (SCREEN_HEIGHT / 2) + 5

void Clock_Draw_Outline(uint16_t xCenter, uint16_t yCenter, double radius);
void Clock_Draw_Hands(uint16_t xCenter, uint16_t yCenter, double radius, uint8_t hours, uint8_t minutes, uint8_t seconds);
void Clock_Write_Date(uint16_t xCenter, uint16_t xOffset, uint16_t yCenter, uint16_t yOffset, uint8_t Wday, uint8_t Day, uint8_t Month, uint16_t Year);
