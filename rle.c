#include "rle.h"
#include <stdlib.h>

RLEPair* encodeChannel(unsigned char* channelData, int size, int* outSize){
    RLEPair* encoded = malloc(size * sizeof(RLEPair)); // worst case size

    int encodeIndex = 0;
    int count = 0;
    unsigned char currentValue = channelData[0];

    for (int i =  0; i < size; i++){
        if(currentValue == channelData[i] && currentValue < 256){
            count ++;
        } else {
            encoded[encodeIndex].count = count;
            encoded[encodeIndex].value = currentValue;
            encodeIndex ++;

            count = 1;
            currentValue = channelData[i];
        }
    }

    // store the last run
    encoded[encodeIndex].count = count;
    encoded[encodeIndex].value = currentValue;
    encodeIndex ++;

    *outSize = encodeIndex;
    return encoded;
}

RLEData* encodeRLE(RGB** pixels, int width, int height){
    RLEData* rleData = malloc(sizeof(RLEData));
    rleData->width = width;// stores original width
    rleData->height = height;// stores original height

    int size = width * height;

    // allocate temp arrays for each channel
    unsigned char* redChannel = malloc(size * sizeof(unsigned char));
    unsigned char* greenChannel = malloc(size * sizeof(unsigned char));
    unsigned char* blueChannel = malloc(size * sizeof(unsigned char));

    // extract individual channels
    int index = 0;
    for(int i = 0; i < height; i ++){
        for (int j = 0; j < width; j++){
            redChannel[index] = pixels[i][j].red;
            greenChannel[index] = pixels[i][j].green;
            blueChannel[index] = pixels[i][j].blue;
            index++;
        }
    }

    // Encode each channel
    rleData->red = encodeChannel(redChannel, size, &rleData->redSize); 
    rleData->green = encodeChannel(greenChannel, size, &rleData->greenSize); 
    rleData->blue = encodeChannel(blueChannel, size, &rleData->blueSize); 

    // free tmp arrays
    free(redChannel);
    free(greenChannel);
    free(blueChannel);

    return rleData;
}

RGB** decodeRLE(RLEData* rleData){
    int width = rleData->width;
    int height = rleData->height;
    RGB** pixels = malloc(height * sizeof(RGB*));

    for(int i = 0; i < height; i++){
        pixels[i] = malloc(width * sizeof(RGB));
    }

    int blueIndex = 0;
    for(int i = 0; i < rleData->blueSize; i++){
        unsigned char cnt = rleData->blue[i].count;
        unsigned char val = rleData->blue[i].value;

        for(int j = 0; j < cnt; j++){
            int row = blueIndex / width;
            int col = blueIndex % width;
            pixels[row][col].blue = val;
            blueIndex++;
        }
    }

    int redIndex = 0;
    for(int i = 0; i < rleData->redSize; i++){
        unsigned char cnt = rleData->red[i].count;
        unsigned char val = rleData->red[i].value;

        for(int j = 0; j < cnt; j++){
            int row = redIndex / width;
            int col = redIndex % width;
            pixels[row][col].red = val;
            redIndex++;
        }
    }

    int greenIndex = 0;
    for(int i = 0; i < rleData->greenSize; i++){
        unsigned char cnt = rleData->green[i].count;
        unsigned char val = rleData->green[i].value;

        for(int j = 0; j < cnt; j++){
            int row = greenIndex / width;
            int col = greenIndex % width;
            pixels[row][col].green = val;
            greenIndex++;
        }
    }

    return pixels;
}


void freeRLEData(RLEData* rleData) {
    if (rleData) {
        free(rleData->red);
        free(rleData->green);
        free(rleData->blue);
        free(rleData);
    }
}
