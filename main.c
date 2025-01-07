
#include <stdio.h>
#include "bmp.h"
#include "rle.h"

int main(){
    int width, height;
    const char* filename = "sample_640x426.bmp";

    // Read original image
    RGB** original = readBMP(filename, &width, &height);
    if (!original) {
        printf("Test 1 failed: Could not read BMP file\n");
        return 1;
    }
    
    // Compress
    RLEData* compressed = encodeRLE(original, width, height);
    if (!compressed) {
        printf("Test 1 failed: Compression failed\n");
        freeBMP(original, height);
        return 1;
    }
    
    // Decompress
    RGB** decoded = decodeRLE(compressed);
    if (!decoded) {
        printf("Test 1 failed: Decompression failed\n");
        freeBMP(original, height);
        // Add freeRLEData when implemented
        return 1;
    }
    // Cleanup
    freeBMP(original, height);
    freeBMP(decoded, height);
    freeRLEData(compressed);
    return 0;
}


