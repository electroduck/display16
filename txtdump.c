#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int main(int argc, char** argv) {
    FILE* in;
    int c;
	DWORD ms;
	
	if(argc < 3) {
		puts("Usage: txtdump <file.txt> <ms-per-line>");
		return 1;
	}
	
	in = fopen(argv[1], "r");
	if(!in || ferror(in)) {
		puts("Unable to open file.");
		return 1;
	}
	
	ms = (DWORD)strtol(argv[2], 0, 0);
	
	while((c = fgetc(in)) != EOF) {
		putchar(c);
		if(c == '\n')
			Sleep(ms);
	}
	
	return 0;
}
