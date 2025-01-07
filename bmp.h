#ifndef BMP_H
#define BMP_H
#include "bmp_types.h"

RGB** readBMP(const char* filename, int* width, int* height);
void freeBMP(RGB **pixels, int height);

#endif