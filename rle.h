#ifndef RLE_h
#define RLE_h
#include "rle_types.h"
#include "bmp_types.h"

RLEData* encodeRLE(RGB** pixels, int width, int height);
void freeRLEData(RLEData* rleData);
RGB** decodeRLE(RLEData* rleData);


#endif