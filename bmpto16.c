#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "image16c.h"
#include "bmp.h"

int main(int argc, char** argv) {
	FILE* bmpFile;
	FILE* pltFile;
	FILE* outFile;
	uint32_t pltData[IMAGE16C_COLORCT];
	uint32_t w, h;
	uint8_t* data24bpp;
	uint8_t* data16c;
	size_t nBytes;
	
	if(argc < 4) {
		puts("Usage: bmpto16 <image.bmp> <palette.bin> <output.bin>");
		puts("\t<palette.bin> = ##GEN to generate automatically.");
		return 1;
	}
	
	bmpFile = fopen(argv[1], "rb");
	if(!bmpFile || ferror(bmpFile)) {
		puts("Failed to open bitmap file.");
		return 1;
	}
	
	if(!strcmp(argv[2], "##GEN"))
		pltFile = NULL;
	else {
		pltFile = fopen(argv[2], "rb");
		if(!pltFile || ferror(pltFile)) {
			puts("Failed to open palette file.");
			return 1;
		}
	}
	
	outFile = fopen(argv[3], "wb");
	if(!outFile || ferror(outFile)) {
		puts("Failed to open output file.");
		return 1;
	}
	
	data24bpp = BMPRead(bmpFile, (int32_t*)&w, (int32_t*)&h, NULL, 0);
	if(!data24bpp) {
		puts("Failed to read bitmap.");
		return 1;
	}
	
	if(pltFile) {
		if(fread(pltData, sizeof(uint32_t), IMAGE16C_COLORCT, pltFile)
			!= IMAGE16C_COLORCT) {
			puts("Failed to read palette.");
			return 1;
		}
		fclose(pltFile);
	} else {
		GeneratePaletteFrom24bpp(pltData, w, h, data24bpp,
			IMAGE16C_PLTTHRESH_DEFAULT);
	}
	
	#ifdef IMAGE16_PACKED
	nBytes = (w/2) * h;
	#else
	nBytes = w * h;
	#endif
	
	data16c = malloc(nBytes);
	if(!data16c) {
		puts("Failed to allocate space for converted image.");
		return 1;
	}
	
	Convert24bppTo16c(w, h, data24bpp, data16c, pltData);
	
	if(!WriteImage16c(outFile, w, h, pltData, data16c)) {
		puts("Failed to write output image.");
		return 1;
	}
	
	fclose(outFile);
	return 0;
}
