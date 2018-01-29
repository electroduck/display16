#include <stdio.h>
#include <stdlib.h>
#include "image16c.h"

int main(int argc, char** argv) {
	FILE* inFile;
	size_t imgW, imgH;
	uint32_t imgPlt[IMAGE16C_COLORCT];
	uint8_t* imgData = NULL;
	
	if(argc < 2) {
		puts("Usage: display16 <file>");
		return 1;
	}
	
	inFile = fopen(argv[1], "rb");
	if(!inFile || ferror(inFile)) {
		puts("Failed to open file.");
		return 1;
	}
	
	if(!ReadImage16c(inFile, &imgW, &imgH, imgPlt, &imgData)) {
		puts("Failed to read image.");
		return 1;
	}
	
	fclose(inFile);
	
	if(!DisplayImage16c(imgW, imgH, imgPlt, imgData)) {
		puts("Failed to display image.");
		return 1;
	}
	
	getchar();
	
	free(imgData);
	return 0;
}
