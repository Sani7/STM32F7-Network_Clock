#include "clock.h"

const char *number[12] =
{ "6", "5", "4", "3", "2", "1", "12", "11", "10", "9", "8", "7" };

const char *days[7] =
{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *months[12] =
{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const uint16_t offset[] = {
		3, 10,	//6
		6, 8,	//5
		6, 8,	//4
		6, 4,	//3
		6, 0,	//2
		6, 2,	//1
		6, 0,	//12
		0, 0,	//11
		0, 0,	//10
		0, 5,	//9
		0, 10,	//8
		0, 12	//7
};

void Clock_Draw_Outline(uint16_t xCenter, uint16_t yCenter, double radius)
{
	uint16_t x1, y1, x2, y2;
	double angle;
	// draw the outline of the clock
	BSP_LCD_DrawCircle(xCenter, yCenter, 2);
	// draw minute's ticks (60 lines)
	for (int j = 1; j <= 60; j++)
	{
		angle = j * 6;
		angle = angle * M_PI / 180;

		x1 = xCenter + (sin(angle) * radius);
		y1 = yCenter + (cos(angle) * radius);
		x2 = xCenter + (sin(angle) * (radius));
		y2 = yCenter + (cos(angle) * (radius));
		BSP_LCD_DrawLine(x1, y1, x2, y2);
	}
	// draw hour's ticks (12 lines)
	for (int j = 0; j < 12; j++)
	{
		angle = j * 30 * M_PI / 180;

		x1 = xCenter + (sin(angle) * radius);
		y1 = yCenter + (cos(angle) * radius);
		x2 = xCenter + (sin(angle) * (radius - 4));
		y2 = yCenter + (cos(angle) * (radius - 4));
		BSP_LCD_DrawLine(x1, y1, x2, y2);
		// draw hour digits(12 lines)
		x2 = xCenter + (sin(angle) * (radius - 8));
		y2 = yCenter + (cos(angle) * (radius - 8));
		BSP_LCD_DisplayStringAt(x2 - offset[2*j], y2 - offset[2*j+1], (uint8_t*) number[j], LEFT_MODE);
	}
}

void Clock_Draw_Hands(uint16_t xCenter, uint16_t yCenter, double radius,
		uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	uint16_t x2, y2;
	double angle;
	angle = seconds * 6;
	angle = angle * M_PI / 180;
	x2 = xCenter + (sin(angle) * (radius - 30));
	y2 = yCenter - (cos(angle) * (radius - 30));
	BSP_LCD_DrawLine(xCenter, yCenter, x2, y2);

	angle = minutes * 6 + (seconds / 10);
	angle = angle * M_PI / 180;
	x2 = xCenter + (sin(angle) * (radius - 50));
	y2 = yCenter - (cos(angle) * (radius - 50));
	BSP_LCD_DrawLine(xCenter, yCenter, x2, y2);

	angle = hours * 30 + ((minutes / 12) * 6);
	angle = angle * M_PI / 180;
	x2 = xCenter + (sin(angle) * (radius / 2));
	y2 = yCenter - (cos(angle) * (radius / 2));
	BSP_LCD_DrawLine(xCenter, yCenter, x2, y2);
}

void Clock_Write_Date(uint16_t xCenter, uint16_t xOffset, uint16_t yCenter, uint16_t yOffset, uint8_t Wday, uint8_t Day, uint8_t Month, uint16_t Year)
{
	char buff[16];
	memset(buff, 0, 16);
	sprintf(buff, "%s %2u", days[Wday - 1], Day);
	BSP_LCD_DisplayStringAt(xCenter - xOffset, yCenter - yOffset, (uint8_t*)buff, CENTER_MODE);
	memset(buff, 0, 16);
	sprintf(buff, "%s %4u", months[Month - 1], Year);
	BSP_LCD_DisplayStringAt(xCenter + xOffset, yCenter + yOffset, (uint8_t*)buff, CENTER_MODE);
}
