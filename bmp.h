#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdint.h>

#define BMP_BYTESPERPIXEL 3
#define BMP_DPI 96

#define BMP_BITSPERPIXEL (BMP_BYTESPERPIXEL * 8)
#define BMP_DPM (BMP_DPI * 39)

/**
 * BMPWrite: Write a bitmap file.
 * f		File to write to (opened, writable)
 * w		The width of the bitmap
 * h		The height of the bitmap
 * data		Raw image data of the bitmap
 * Returns: Number of pixels written, or 0 on error.
 */
size_t __stdcall BMPWrite(FILE* f, int32_t w, int32_t h, uint8_t* data);

/**
 * BMPRead: Read a bitmap file.
 * f		File to read from (opened, readable)
 * w		Location to place the bitmap's width in
 * h		Location to place the bitmap's height in
 * databuf	Location to place the image data in, or 0 to allocate
 * dbuf_len	Size of databuf, or 0 if databuf is null
 * Returns: Location of image data, or 0 on error.
 */
uint8_t* __stdcall BMPRead(FILE* f, int32_t* w, int32_t* h, uint8_t* databuf,
	size_t dbuf_len);

/**
 * BMPFlipVert: Flip image vertically.
 * w		Image width
 * h		Image height
 * data		Location of the image data
 */
void __stdcall BMPFlipVert(int32_t w, int32_t h, uint8_t* data);

#endif
