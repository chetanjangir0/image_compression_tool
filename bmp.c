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

void writeBMP(const char* filename, RGB** pixels, int width, int height){
    FILE* file = fopen(filename, "wb");
    if (!file){
        printf("Could not create file for writing\n");
        return;
    }

    // Calculate file size and padding
    int rowPadding = (4 - ((width * sizeof(RGB) % 4))) % 4;
    int imageSize = (width * sizeof(RGB) + rowPadding) * height;
    int fileSize = imageSize + sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    // prepare file header
    BMPFileHeader fileHeader = {
        .bfType = 0x4D42,  // "BM" in hex
        .bfSize = fileSize,
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader)
    };

    // prepare info header
    BMPInfoHeader infoHeader = {
        .biSize = sizeof(BMPInfoHeader),
        .biWidth = width,
        .biHeight = height,
        .biPlanes = 1,
        .biBitCount = 24,  // 24-bit RGB
        .biCompression = 0, // No compression
        .biSizeImage = imageSize,
        .biXPelsPerMeter = 2835, // 72 DPI
        .biYPelsPerMeter = 2835, // 72 DPI
        .biClrUsed = 0,
        .biClrImportant = 0
    };

    // write headers
    fwrite(&fileHeader, sizeof(BMPFileHeader), 1, file);
    fwrite(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    unsigned char paddingBytes[] = {0, 0, 0};
    // wirte pixel data from bottom to top
    for(int i = height - 1; i >=0; i--){
        fwrite(pixels[i], sizeof(RGB), width, file);

        if (rowPadding > 0){
            fwrite(paddingBytes, 1, rowPadding, file);
        }
    }

    fclose(file);

}

void freeBMP(RGB **pixels, int height){
    for (int i = 0; i < height; i++){
        free(pixels[i]);
    }
    free(pixels);
}