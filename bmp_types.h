#ifndef BMP_TYPES_H
#define BMP_TYPES_H

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

#endif