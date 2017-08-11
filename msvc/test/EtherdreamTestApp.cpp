// EtherdreamTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define NOMINMAX

// Laser Include
#include <windows.h>
#include <j4cDAC.h>
#include <iostream>
#include <limits>
#include <stdint.h>

// some globals
#define CIRCLE_POINTS						600
#define SQUARE_POINTS						400					// Multiple of 4!
#define DOT_POINTS							400
struct EAD_Pnt_s circle[CIRCLE_POINTS];
struct EAD_Pnt_s square[SQUARE_POINTS];
struct EAD_Pnt_s dot[DOT_POINTS];
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/**
@brief Calculate color value
**/
int16_t colorsin(float pos)
{
	int max_value = std::numeric_limits<int16_t>::max();
	int min_value = std::numeric_limits<int16_t>::min();

	// Get color value
	int res = (sin(pos) + 1) * max_value;
	res = res > max_value ? max_value : res;
	res = res < min_value ? min_value : res;
	return res;
}



/**
@brief Simple lerp
**/
int Lerp(int16_t start, int16_t end, float percent)
{
	return (int16_t)((float)start + percent*((float)end - (float)start));
}
float LerpFloat(float inStart, float inEnd, float inPercent)
{
	return (inStart + inPercent*(inEnd - inStart));
}



/**
@brief Creates a simple drawable circle
**/
void FillCircle(float phase, int mode) {
	int i;
	int max_value = std::numeric_limits<int16_t>::max();

	for (i = 0; i < CIRCLE_POINTS; i++) {
		struct EAD_Pnt_s *pt = &circle[i];
		float ip = (float)i * 2.0 * M_PI / (float)CIRCLE_POINTS;
		float ipf = fmod(ip + phase, 2.0 * M_PI);;

		switch (mode) {
		default:
		case 0: {
			float cmult = .05 * sin(30 * (ip - phase / 3));
			pt->X = sin(ip) * 20000 * (1 + cmult);
			pt->Y = cos(ip) * 20000 * (1 + cmult);
			break;
		}
		case 1: {
			float cmult = .10 * sin(10 * (ip - phase / 3));
			pt->X = sin(ip) * 20000 * (1 + cmult);
			pt->Y = cos(ip) * 20000 * (1 + cmult);
			break;
		}
		case 2: {
			ip *= 3;
			float R = 5;
			float r = 3;
			float D = 5;

			pt->X = 2500 * ((R - r)*cos(ip + phase) + D*cos((R - r)*ip / r));
			pt->Y = 2500 * ((R - r)*sin(ip + phase) - D*sin((R - r)*ip / r));
			break;
		}
		case 3: {
			int n = 5;
			float R = 5 * cos(M_PI / n) / cos(fmod(ip, (2 * M_PI / n)) - (M_PI / n));
			pt->X = 3500 * R*cos(ip + phase);
			pt->Y = 3500 * R*sin(ip + phase);
			break;
		}
		case 4: {
			float Xo = sin(ip);
			pt->X = 20000 * Xo * cos(phase / 4);
			pt->Y = 20000 * Xo * -sin(phase / 4);
			ipf = fmod(((Xo + 1) / 2.0) + phase / 3, 1.0) * 2 * M_PI;
		}
		}

		pt->R = colorsin(ipf);
		pt->G = colorsin(ipf + (2.0 * M_PI / 3.0));
		pt->B = colorsin(ipf + (4.0 * M_PI / 3.0));
		pt->I = max_value;
	}
}



/**
@brief Creates a simple drawable square
**/
void FillSquare(float inLocX, float inLocY, float inBrightNess, float inSize)
{
	if (SQUARE_POINTS % 4 != 0)
	{
		std::cout << "Square points not a multiple of 4!";
		return;
	}

	// Clamp location
	float loc_x = inLocX > 1.0f ? 1.0f : inLocX < 0.0f ? 0.0f : inLocX;
	float loc_y = inLocY > 1.0f ? 1.0f : inLocY < 0.0f ? 0.0f : inLocY;

	// min / max values
	int16_t min_value = std::numeric_limits<int16_t>::min();
	int16_t max_value = std::numeric_limits<int16_t>::max();

	// Calculate location in units
	int x_center = (int)Lerp(min_value, max_value, inLocX);
	int y_center = (int)Lerp(min_value, max_value, inLocY);

	// Get corner points
	int global_max = ((int)max_value - (int)min_value) / 2;
	int offset = (int)((float)global_max * inSize);

	int x_min_int = x_center - offset;
	int x_max_int = x_center + offset;
	int y_min_int = y_center - offset;
	int y_max_int = y_center + offset;

	int16_t x_min = (int16_t)(x_min_int < (int)min_value ? (int)min_value : x_min_int);
	int16_t x_max = (int16_t)(x_max_int > (int)max_value ? (int)max_value : x_max_int);
	int16_t y_min = (int16_t)(y_min_int < (int)min_value ? (int)min_value : y_min_int);
	int16_t y_max = (int16_t)(y_max_int > (int)max_value ? (int)max_value : y_max_int);

	int16_t color_value = Lerp(0, max_value, inBrightNess);

	// Get side points and increment
	int side_points = SQUARE_POINTS / 4;
	float inc = 1.0f / (float)(side_points);

	int idx(0);
	int counter(0);
	for (int i = side_points * 0; i < side_points * 1; i++)		//< Fill up left > up right
	{
		EAD_Pnt_s* current_point = &square[idx];
		current_point->X = Lerp(x_min, x_max, (float)counter * inc);
		current_point->Y = y_max;
		current_point->R = color_value;
		current_point->B = color_value;
		current_point->G = color_value;
		current_point->I = color_value;
		idx++;
		counter++;
	}

	counter = 0;
	for (int i = side_points * 1; i < side_points * 2; i++)		//< Top right to bottom right
	{
		EAD_Pnt_s* current_point = &square[idx];
		current_point->X = x_max;
		current_point->Y = Lerp(y_max, y_min, (float)counter * inc);
		current_point->R = color_value;
		current_point->B = color_value;
		current_point->G = color_value;
		current_point->I = color_value;
		idx++;
		counter++;
	}

	counter = 0;
	for (int i = side_points * 2; i < side_points * 3; i++)		//< bottom right to bottom left
	{
		EAD_Pnt_s* current_point = &square[idx];
		current_point->X = Lerp(x_max, x_min, (float)counter * inc);
		current_point->Y = y_min;
		current_point->R = color_value;
		current_point->B = color_value;
		current_point->G = color_value;
		current_point->I = color_value;
		idx++;
		counter++;
	}

	counter = 0;
	for (int i = side_points * 3; i < side_points * 4; i++)		//< bottom left to top left
	{
		EAD_Pnt_s* current_point = &square[idx];
		current_point->X = x_min;
		current_point->Y = Lerp(y_min, y_max, (float)counter * inc);
		current_point->R = color_value;
		current_point->B = color_value;
		current_point->G = color_value;
		current_point->I = color_value;
		idx++;
		counter++;
	}
}



/**
@brief Fills point buffer
**/
void FillDot(float inLocX, float inLocY)
{
	// Clamp location
	float loc_x = inLocX > 1.0f ? 1.0f : inLocX < 0.0f ? 0.0f : inLocX;
	float loc_y = inLocY > 1.0f ? 1.0f : inLocY < 0.0f ? 0.0f : inLocY;

	// min / max values
	int16_t min_value = std::numeric_limits<int16_t>::min();
	int16_t max_value = std::numeric_limits<int16_t>::max();

	// Calculate location in units
	int x_center = (int)Lerp(min_value, max_value, inLocX);
	int y_center = (int)Lerp(min_value, max_value, inLocY);

	// Now fill buffer
	for (int i = 0; i < DOT_POINTS; i++)
	{
		EAD_Pnt_s* current_point = &(dot[i]);
		current_point->X = x_center;
		current_point->Y = y_center;
		current_point->R = max_value;
		current_point->G = max_value;
		current_point->B = max_value;
		current_point->I = max_value;
	}
}



/**
@brief Draws a square
**/
void DrawSquare(int inDacNumber)
{
	int size = sizeof(EAD_Pnt_s) * SQUARE_POINTS;
	float lv(0);
	while (true)
	{
		int status = EtherDreamGetStatus(&inDacNumber);
		if (status == GET_STATUS_BUSY)
			continue;

		// Normalize and lerp
		float sx = (sin(lv) + 1.0f) / 2.0f;
		sx = LerpFloat(0.5f, 0.7f, sx);

		float sy = (cos(lv) + 1.0f) / 2.0f;
		sy = LerpFloat(0.4f, 0.6f, sy);

		float cv = (sin(lv) + 1.0f) / 2.0f;

		FillSquare(sy, sx, 1.0f, 0.25f);
		bool write = EtherDreamWriteFrame(&inDacNumber, square, size, 30000, 1);

		lv += 0.01f;
	}
}



/**
@brief Draws a circle
**/
void DrawCircle(int inDacNumber, int inShape)
{

	int i(0);
	int size = sizeof(EAD_Pnt_s) * CIRCLE_POINTS;

	while (true)
	{
		int status = EtherDreamGetStatus(&inDacNumber);
		if (status == GET_STATUS_BUSY)
			continue;

		// Fill the circle
		FillCircle((float)i / 50, inShape);

		// Write circle
		bool write = EtherDreamWriteFrame(&inDacNumber, circle, size, 30000, 1);
		i++;
	}
}



/**
@brief Draws a point
**/
void DrawDot(int inDacNumber)
{
	// Calculate draw size in bytes
	int size = sizeof(EAD_Pnt_s) * DOT_POINTS;

	float lv(0);
	while (true)
	{
		int status = EtherDreamGetStatus(&inDacNumber);
		if (status == GET_STATUS_BUSY)
			continue;

		// Normalize and lerp
		float sx = (sin(lv) + 1.0f) / 2.0f;
		sx = LerpFloat(0.25f, 0.75f, sx);

		float sy = (cos(lv) + 1.0f) / 2.0f;
		sy = LerpFloat(0.25f, 0.75f, sy);

		// Populate dot
		FillDot(sx, sy);

		// Write to ether dac
		bool write = EtherDreamWriteFrame(&inDacNumber, dot, size, 30000, 1);

		lv += 0.01f;
	}
}




/**
@brief Exit app with message and string
**/
int Exit(const std::string& inMessage, int inExitCode)
{
	std::cout << inMessage.c_str() << "\n";
	std::cout << "Press any key to quit: ";
	char v;
	std::cin >> v;
	return inExitCode;
}


/**
@brief Main function
**/
int _tmain(int argc, _TCHAR* argv[])
{
	// Successfully loaded and sampled dll
	std::cout << "Successfully loaded EtherDream library\n";

	// Get the card number
	int no_cards = EtherDreamGetCardNum();
	if (no_cards == 0)
		return Exit("Unable to detect any ether dream dac", -2);
	std::cout << "Detected: " << no_cards << " EtherDream dacs\n";
	int number(0);

	// Get name
	char buffer[256];
	EtherDreamGetDeviceName(&number, buffer, 256);
	std::cout << "DAC Name: " << std::string(buffer).c_str() << "\n";

	// Get status
	int status = EtherDreamGetStatus(&number);
	std::cout << "DAC Status: " << (int)status << "\n";

	// Open Device
	if (!EtherDreamOpenDevice(&number))
		return Exit("Unable to open ether dream dac", -3);

	// Select shape
	int shape(0);
	std::cout << "Select Shape: 0-3 = sphere, 4 = line, 5 = square, 6 = dot \n";
	std::cin >> shape;
	shape = shape < 0 ? 0 : shape > 6 ? 6 : shape;
	std::cout << "Selected shape: " << shape << "\n";

	switch (shape)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		DrawCircle(number, shape);
		break;
	case 5:
		DrawSquare(number);
		break;
	case 6:
		DrawDot(number);
		break;
	}


	// Close device
	bool success = EtherDreamStop(&number);

	// Stop
	success = EtherDreamCloseDevice(&number);

	return 0;
}

