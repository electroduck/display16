#ifndef IMAGE16_H
#define IMAGE16_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define IMAGE16C_COLORCT 16

//#define IMAGE16C_PACKED

#define IMAGE16C_PALETTE_R(c) (uint8_t) ((c) & 0x000000FF)
#define IMAGE16C_PALETTE_G(c) (uint8_t)(((c) & 0x0000FF00) >>  8)
#define IMAGE16C_PALETTE_B(c) (uint8_t)(((c) & 0x00FF0000) >> 16)

#define IMAGE16C_PLTINVALID 0xFF000000

#define IMAGE16C_24BPP_TO_PLTCOLOR(i, x, y, w) \
	 ((uint32_t)(i)[((y) * (w) + (x)) * 3] \
	| (uint32_t)(i)[((y) * (w) + (x)) * 3 + 1] << 8 \
	| (uint32_t)(i)[((y) * (w) + (x)) * 3 + 2] << 16)

#define IMAGE16C_RGB_TO_PLTCOLOR(r, g, b) \
	 ((uint32_t)(r) | (uint32_t)(g) << 8 | (uint32_t)(b) << 16)

#define IMAGE16C_PLTCOLOR_DIFF(a, b) \
	((((a) & 0xFF000000) || ((b) & 0xFF000000)) ? 0xFF : \
	((abs((int)(a & 0x000000FF) - (int)(b & 0x000000FF)) \
	+ abs((int)(a & 0x0000FF00) - (int)(b & 0x0000FF00)) \
	+ abs((int)(a & 0x00FF0000) - (int)(b & 0x00FF0000))) \
	/ 3))

#define IMAGE16C_PLTTHRESH_DEFAULT 32
#define IMAGE16C_PLTMAXITER_DEFAULT 100000
	
int ReadImage16c(FILE* f, uint32_t* w, uint32_t* h, uint32_t* palette,
	uint8_t** data);
	
int DisplayImage16c(uint32_t w, uint32_t h, uint32_t* palette, uint8_t* data);

int WriteImage16c(FILE* f, uint32_t w, uint32_t h, uint32_t* palette,
	uint8_t* data);
	
void Convert24bppTo16c(uint32_t w, uint32_t h, uint8_t* data24bpp,
	uint8_t* data16c, uint32_t* palette);

size_t GetBestColorMatch(uint32_t* palette, uint32_t color, size_t pltSize);

int RestoreConsoleToDefault(void);

void GeneratePaletteFrom24bpp(uint32_t* palette, uint32_t w, uint32_t h,
	uint8_t* data24bpp, int threshold);

#endif
