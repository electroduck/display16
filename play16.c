#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "image16c.h"

//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Format:
		uint16_t	Milliseconds per frame
		uint32_t	Location of WAV audio within file (zero: no audio)
		uint32_t	Audio length in bytes
		uint32_t	Number of frames
	Nx	uint8_t		Images
	Nx	uint8_t		WAV audio data
*/

void CtrlCHandler(int n);
uint32_t millis(void);

int main(int argc, char** argv) {
	FILE* inFile;
	size_t imgW, imgH, j;
	uint32_t imgPlt[IMAGE16C_COLORCT];
	uint8_t* imgData = NULL;
	uint16_t msPerFrame;
	uint32_t msLast, msCur, audioLoc, audioLen, i, frameCt;
	COORD zeroPos = { .X=0, .Y=0 };
	HANDLE hOut;
	fpos_t lastPos;
	uint8_t* audioData = NULL;
	
	if(argc < 2) {
		puts("Usage: play16 <file>");
		return 1;
	}
	
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if(!hOut) {
		puts("Failed to get handle to console output.");
		return 1;
	}
	
	inFile = fopen(argv[1], "rb");
	if(!inFile || ferror(inFile)) {
		printf("Failed to open `%s'.\n", argv[1]);
		return 1;
	}
	
	if(!fread(&msPerFrame, sizeof(uint16_t), 1, inFile)) {
		puts("Error reading video FPS.");
		return 1;
	}
	
	if(!fread(&audioLoc, sizeof(uint32_t), 1, inFile)) {
		puts("Error reading audio position.");
		return 1;
	}
	
	if(!fread(&audioLen, sizeof(uint32_t), 1, inFile)) {
		puts("Error reading audio length.");
		return 1;
	}
	
	if(!fread(&frameCt, sizeof(uint32_t), 1, inFile)) {
		puts("Error reading frame count.");
		return 1;
	}
	
	system("cls");
	
	signal(SIGINT, CtrlCHandler);
	
	if(audioLoc && audioLen) {
		fgetpos(inFile, &lastPos);
		
		if(fseek(inFile, (long int)audioLoc, SEEK_SET)) {
			if(MessageBoxA(NULL,
				"Unable to seek to audio location.\nContinue in silence?",
				NULL, MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				goto noaudio;
			else
				return 1;
		}
		
		if(!(audioData = malloc(audioLen))) {
			if(MessageBoxA(NULL,
				"Unable to allocate space for audio.\nContinue in silence?",
				NULL, MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				goto noaudio;
			else
				return 1;
		}
		
		if(fread(audioData, 1, audioLen, inFile) != audioLen) {
			if(MessageBoxA(NULL,
				"Failed to read audio entirely.\nContinue in silence?",
				NULL, MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				goto noaudio;
			else
				return 1;
		}
		
		if(!PlaySoundA((LPCSTR)audioData, NULL, SND_ASYNC | SND_MEMORY)) {
			if(MessageBoxA(NULL,
				"Failed to read audio entirely.\nContinue in silence?",
				NULL, MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				goto noaudio;
			else
				return 1;
		}
		
		noaudio:
		fsetpos(inFile, &lastPos);
	}
	
	msLast = millis();
	for(i = 0; i < frameCt; i++) {
		if(!SetConsoleCursorPosition(hOut, zeroPos)) {
			printf("Failed to set console cursor pos on frame %u.\n", i);
			return 1;
		}
		
		if(!ReadImage16c(inFile, &imgW, &imgH, imgPlt, &imgData)) {
			printf("Error reading frame %u.\n", i);
			return 1;
		}
		
		if(!DisplayImage16c(imgW, imgH, imgPlt, imgData)) {
			printf("Error displaying frame %u.\n", i);
			return 1;
		}
		
		while((millis() - msLast) < msPerFrame) {
		}
		msLast = millis();
	}
	
	return 0;
}

void CtrlCHandler(int n) {
	RestoreConsoleToDefault();
	exit(0);
}

static LARGE_INTEGER g_perfFreq = { .QuadPart = 0 };
uint32_t millis(void) {
	LARGE_INTEGER curPerfCtr;
	
	if(!g_perfFreq.QuadPart) {
		if(!QueryPerformanceFrequency(&g_perfFreq)) {
			MessageBoxA(NULL, "No time-interval counter installed.", NULL,
				MB_ICONERROR);
			exit(1);
		}
	}
	
	QueryPerformanceCounter(&curPerfCtr);
	
	return (uint32_t)((curPerfCtr.QuadPart * 1000) / g_perfFreq.QuadPart);
}










