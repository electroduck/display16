#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include "image16c.h"

static const uint32_t g_invalidPalette[IMAGE16C_COLORCT] = {
	0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
	0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
	0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
	0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000 };

static CONSOLE_SCREEN_BUFFER_INFOEX g_consoleDefault = {
	.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX),
	.dwSize = { .X = 80, .Y = 300 },
	.dwCursorPosition = { .X = 0, .Y = 0 },
	.wAttributes = 0x07,
	.srWindow = { .Top = 0, .Left = 0, .Right = 80, .Bottom = 24 },
	.dwMaximumWindowSize = { .X = 80, .Y = 300 },
	.wPopupAttributes = 0x1F,
	.bFullscreenSupported = FALSE,
	.ColorTable = { 0x00000000, 0x00800000, 0x00008000, 0x00808000,
	                0x00000080, 0x00800080, 0x00008080, 0x00c0c0c0,
				    0x00808080, 0x00ff0000, 0x0000ff00, 0x00ffff00,
	                0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff }
};

static size_t g_zero = 0;

int ReadImage16c(FILE* f, uint32_t* w, uint32_t* h, uint32_t* palette,
	uint8_t** data) {
	uint32_t nBytes;
	
	fread(w, sizeof(uint32_t), 1, f);
	if(ferror(f) || feof(f)) return 0;
	fread(h, sizeof(uint32_t), 1, f);
	if(ferror(f) || feof(f)) return 0;
	
	fread(palette, sizeof(uint32_t), IMAGE16C_COLORCT, f);
	if(ferror(f) || feof(f)) return 0;
	
	nBytes = *w * *h;
	
	#ifdef IMAGE16C_PACKED
	nBytes /= 2;
	#endif
	
	if(!*data) {
		*data = malloc(nBytes);
		if(!*data) return 0;
	}
	
	fread(*data, 1, nBytes, f);
	
	return 1;
}

int DisplayImage16c(uint32_t w, uint32_t h, uint32_t* palette, uint8_t* data) {
	CONSOLE_SCREEN_BUFFER_INFOEX csbi/*, csbiOld*/;
	HANDLE hOut;
	uint32_t x, y;
	uint8_t curColor;
	CHAR_INFO* cbuf;
	
	cbuf = alloca(w * h * sizeof(CHAR_INFO));
	if(!cbuf) return 0;
	
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hOut == INVALID_HANDLE_VALUE) return 0;
	
	ZeroMemory(&csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
	csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	
	GetConsoleScreenBufferInfoEx(hOut, &csbi);
	//memcpy(&csbiOld, &csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
	
	csbi.dwCursorPosition.X = 0;
	csbi.dwCursorPosition.Y = 0;
	csbi.dwSize.X = w;
	csbi.dwSize.Y = h;
	csbi.srWindow.Left = 0;
	csbi.srWindow.Top = 0;
	csbi.srWindow.Right = (SHORT)w;
	csbi.srWindow.Bottom = (SHORT)h;
	memcpy(csbi.ColorTable, palette, sizeof(uint32_t) * IMAGE16C_COLORCT);
	SetConsoleScreenBufferInfoEx(hOut, &csbi);
	
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			#ifdef IMAGE16C_PACKED
			curColor = data[y * (w/2) + (x/2)]
			#else
			curColor = data[y * w + x];
			#endif
			
			#ifdef IMAGE16C_PACKED
			if(x % 2)
				curColor = (curColor & 0x0F) << 4;
			else
				curColor = (curColor & 0xF0);
			#else
			curColor = curColor << 4;
			#endif
			
			//SetConsoleTextAttribute(hOut, curColor);
			//putchar(' ');
			//fflush(stdout);
			
			cbuf[y * w + x].Char.AsciiChar = ' ';
			cbuf[y * w + x].Attributes = curColor;
		}
		//putchar('\n');
		//fflush(stdout);
	}
	
	WriteConsoleOutput(hOut, (CHAR_INFO*)cbuf, csbi.dwSize,
		csbi.dwCursorPosition, &csbi.srWindow);
	
	//putchar('\n');
	
	//SetConsoleScreenBufferInfoEx(hOut, &csbiOld);
	
	return 1;
}

int WriteImage16c(FILE* f, uint32_t w, uint32_t h, uint32_t* palette,
	uint8_t* data) {
	uint32_t nBytes;
	
	fwrite(&w, sizeof(uint32_t), 1, f);
	if(ferror(f) || feof(f)) return 0;
	fwrite(&h, sizeof(uint32_t), 1, f);
	if(ferror(f) || feof(f)) return 0;
	
	fwrite(palette, sizeof(uint32_t), IMAGE16C_COLORCT, f);
	if(ferror(f) || feof(f)) return 0;
	
	nBytes = w * h;
	
	#ifdef IMAGE16C_PACKED
	nBytes /= 2;
	#endif
	
	fwrite(data, 1, nBytes, f);
	
	return 1;
}

void Convert24bppTo16c(uint32_t w, uint32_t h, uint8_t* data24bpp,
	uint8_t* data16c, uint32_t* palette) {
	uint32_t x, y;
	uint8_t curColor16c;
	
	#ifdef IMAGE16C_PACKED
	memset(data16c, 0, (w/2) * h);
	#endif
	
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			curColor16c = (uint8_t)GetBestColorMatch(palette,
				IMAGE16C_24BPP_TO_PLTCOLOR(data24bpp, x, y, w),
				IMAGE16C_COLORCT);
			
			#ifdef IMAGE16C_PACKED
			if(x % 2)
				data16c[y * (w/2) + (x/2)] |= curColor16c;
			else
				data16c[y * (w/2) + (x/2)] |= curColor16c << 4;
			#else
			data16c[y * w + x] = curColor16c;
			#endif
		}
	}
}

size_t GetBestColorMatch(uint32_t* palette, uint32_t color, size_t pltSize) {
	int lowestDiff = 0x7FFFFFFF;
	int curDiff;
	size_t lowestDiffIndex = 0;
	size_t i;
	
	for(i = 0; i < pltSize; i++, palette++) {
		if(!(*palette & IMAGE16C_PLTINVALID)) {
			/*curDiff = abs((int)IMAGE16C_PALETTE_R(*palette)
						  - (int)IMAGE16C_PALETTE_R(color));
			curDiff += abs((int)IMAGE16C_PALETTE_G(*palette)
						  - (int)IMAGE16C_PALETTE_G(color));
			curDiff += abs((int)IMAGE16C_PALETTE_B(*palette)
						  - (int)IMAGE16C_PALETTE_B(color));
			curDiff /= 3;*/
			curDiff = IMAGE16C_PLTCOLOR_DIFF(*palette, color);
			if(curDiff < lowestDiff) {
				lowestDiffIndex = i;
				lowestDiff = curDiff;
			}
		}
	}
	
	return lowestDiffIndex;
}

int RestoreConsoleToDefault(void) {
	HANDLE hOut;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hOut == INVALID_HANDLE_VALUE) return 0;
	
	if(!SetConsoleScreenBufferInfoEx(hOut, &g_consoleDefault))
		return 0;
	
	system("color 07");
	system("cls");
	
	return 1;
}

#define IMAGE16C_PLTGEN_CANCT 1024

/*
void GeneratePaletteFrom24bpp(uint32_t* palette, uint32_t w, uint32_t h,
	uint8_t* data24bpp, int threshold) {
	uint32_t x, y, colorAsPltColor;
	size_t bestMatch;
	size_t i, j, candidateCt;
	uint32_t candidates[IMAGE16C_PLTGEN_CANCT];
	size_t candidateVotes[IMAGE16C_PLTGEN_CANCT];
	uint32_t* bestCandidate;
	size_t* bestCandidateVotes;
	uint8_t diff;
	uint64_t totalR, totalG, totalB;
	uint32_t avgColor;
	
	memset(candidates, 0xFF, sizeof(uint32_t) * IMAGE16C_PLTGEN_CANCT);
	memset(candidateVotes, 0, sizeof(size_t) * IMAGE16C_PLTGEN_CANCT);
	
	srand(time(0));
	
	// Build candidates list
	candidateCt = 0;
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			colorAsPltColor = IMAGE16C_24BPP_TO_PLTCOLOR(data24bpp, x, y, w);
			bestMatch = GetBestColorMatch(candidates, colorAsPltColor,
				candidateCt);
			
			diff = IMAGE16C_PLTCOLOR_DIFF(candidates[bestMatch], colorAsPltColor);
			if(diff < threshold)
				candidateVotes[bestMatch]++;
			else {
				candidates[candidateCt] = colorAsPltColor;
				candidateVotes[candidateCt] = 1;
				candidateCt++;
				if(candidateCt == IMAGE16C_PLTGEN_CANCT)
					goto tooManyCandidates; // double break
			}
		}
	}
	tooManyCandidates:
	
	
	// Find average
	for(i = 0; i < candidateCt; i++) {
		totalR += IMAGE16C_PALETTE_R(candidates[i]);
		totalG += IMAGE16C_PALETTE_G(candidates[i]);
		totalB += IMAGE16C_PALETTE_B(candidates[i]);
	}
	avgColor = IMAGE16C_RGB_TO_PLTCOLOR(totalR / candidateCt,
		totalG / candidateCt, totalB / candidateCt);
	
	// Prioritize candidates away from average
	for(i = 0; i < candidateCt; i++) {
		diff = IMAGE16C_PLTCOLOR_DIFF(candidates[i], avgColor);
		candidateVotes[i] *= diff / 4;
	}
	
	// Get best candidates
	
	for(i = 0; i < IMAGE16C_COLORCT; i++) {
		bestCandidate = NULL;
		bestCandidateVotes = &g_zero;
		for(j = 0; j < candidateCt; j++) {
			if(candidateVotes[j] > *bestCandidateVotes) {
				bestCandidateVotes = &candidateVotes[j];
				bestCandidate = &candidates[j];
			}
		}
		
		if(!(bestCandidate && *bestCandidateVotes))
			break;
		
		palette[i] = *bestCandidate;
		*bestCandidate = IMAGE16C_PLTINVALID;
		*bestCandidateVotes = 0;
	}
	
	// Get random candidates
	for(i = 0; i < IMAGE16C_COLORCT; i++) {
		palette[i] = candidates[rand() % candidateCt];
	}
}
*/


// This is a travesty.
void GeneratePaletteFrom24bpp(uint32_t* palette, uint32_t w, uint32_t h,
	uint8_t* data24bpp, int threshold) {
	uint32_t x, y;
	uint8_t i;
	
	for(i = 0; i < IMAGE16C_COLORCT; i++) {
		x = rand() % w;
		y = rand() % h;
		
		palette[i] = IMAGE16C_24BPP_TO_PLTCOLOR(data24bpp, x, y, w);
	}
}
	





































