#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

RGB** readBMP(const char* filename, int* width, int* height){
    FILE* file = fopen(filename, "rb"); // read mode with binary mode instead of text mode
    if (!file){
        printf("could not open file\n");
        return NULL;
    }

    // read file header
    BMPFileHeader fileHeader;
    fread(&fileHeader, sizeof(BMPFileHeader), 1, file);

    // verify if it's a BMP file
    if (fileHeader.bfType != 0x4D42){ // BM in hex
        printf("Not a BMP file\n");
        fclose(file);
        return NULL;
    }

    // read info header
    BMPInfoHeader infoHeader;
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    *width = infoHeader.biWidth;
    *height = infoHeader.biHeight;

    // allocate memory for pixel data
    RGB** pixels = malloc(*height * sizeof(RGB*));
    for (int i = 0; i < *height; i++){
        pixels[i] = malloc(*width * sizeof(RGB));
    }

    int rowPadding = (4 - ((*width * sizeof(RGB)) % 4)) % 4;

    // move the file pointer to starting of pixel data
    fseek(file, fileHeader.bfOffBits, SEEK_SET);

    // read pixel data
    // bmp stores pixels from bottom to top, left to right
    for(int i = *height - 1; i >=0; i--){
        fread(pixels[i], sizeof(RGB), *width, file);

        // skip padding bytes 
        fseek(file, rowPadding, SEEK_CUR);
    }

    fclose(file);
    return pixels;
}

void freeBMP(RGB **pixels, int height){
    for (int i = 0; i < height; i++){
        free(pixels[i]);
    }
    free(pixels);
}