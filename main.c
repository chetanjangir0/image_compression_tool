#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>  // for strerror
#include <errno.h>   // for errno
// first header Bitmap file header
#pragma pack(1) // ensures no padding between structure memebers and compatibility with binary data
typedef struct{
    unsigned short bfType; // file type ("BM" for bitmap)
    unsigned int bfSize; // file size in bytes
    unsigned short bfReserved1; // reserved (unused) by MS
    unsigned short bfReserved2; // reserved (unused) by MS
    unsigned int bfOffBits; // offset to pixel data (where the actual pixel data starts)
} BMPFileHeader;

// second header DIB header/ BITMAPINFOHEADER
typedef struct{
    unsigned int biSize;       // size of this header
    int biWidth;               // image width in pixels
    int biHeight;              // Image height in pixels
    unsigned short biPlanes;    // Number of color planes
    unsigned short biBitCount;  // Bits per pixel (1, 4, 8, 24 for RGB)
    unsigned int biCompression;// Compression type
    unsigned int biSizeImage;  // size of pixel data
    int biXPelsPerMeter;        // Horizontal resolution
    int biYPelsPerMeter;        // vertical resolution
    unsigned int biClrUsed;     // Number of colors in the color table
    unsigned int biClrImportant;// Important color count
}BMPInfoHeader;


// RGB pixel structure
typedef struct{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} RGB;

typedef struct{
    unsigned char count;
    unsigned char value;

} RLEPair;

typedef struct {
    int width;
    int height;
    RLEPair *red;
    RLEPair *green;
    RLEPair *blue;
    int redSize;
    int greenSize;
    int blueSize;
} RLEData;

RGB** readBMP(const char* filename, int* width, int* height);
void freeBMP(RGB **pixels, int height);
void freeRLEData(RLEData* rleData);
RLEPair* encodeChannel(unsigned char* channelData, int size, int* outSize);
RLEData* encodeRLE(RGB** pixels, int width, int height);
RGB** decodeRLE(RLEData* rleData);


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
void freeRLEData(RLEData* rleData) {
    if (rleData) {
        free(rleData->red);
        free(rleData->green);
        free(rleData->blue);
        free(rleData);
    }
}

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