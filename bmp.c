/* Create a bitmap from raw RGB24 data or vice versa. */

#include "bmp.h"

#include <stdlib.h>
#include <string.h>

#pragma pack(1)

typedef struct {
	uint16_t type;
	uint32_t filesize;
	uint16_t r1, r2;
	uint32_t offset;
} BMPHeader;

typedef struct {
	uint32_t ihsize;
	int32_t  width, height;
	uint16_t planes;
	uint16_t bpp;
	uint32_t comprtype;
	uint32_t imgsize;
	int32_t  xres, yres;
	uint32_t colorct;
	uint32_t colorctimp;
} BMPInfoHeader;

#pragma pack()

#ifdef EXE

int main(int argc, char** argv) {
	int32_t w, h;
	FILE* in;
	FILE* out;
	size_t n_iod, n_pixels, n_bytes;
	uint8_t* databuf;
	char op;
	
	if(argc < 4) {
		puts("Usage: bmp c <w> <h> <in.raw> <out.bmp>");
		puts("       bmp e <in.bmp> <out.raw>");
		return 0;
	}
	
	op = *argv[1];
	if((op == 'c') || (op == 'C')) {
		w = (int32_t)strtol(argv[2], 0, 0);
		h = (int32_t)strtol(argv[3], 0, 0);
		
		in = fopen(argv[4], "rb");
		if(!in || ferror(in)) {
			printf("Error opening %s for read\n", argv[3]);
			return 1;
		}
		
		out = fopen(argv[5], "wb");
		if(!out || ferror(out)) {
			printf("Error opening %s for write\n", argv[4]);
			return 2;
		}
		
		n_pixels = w * h;
		n_bytes = n_pixels * BMP_BYTESPERPIXEL;
		databuf = malloc(n_bytes);
		if(!databuf) {
			printf("Error allocating %u (0x%08X) bytes for data buffer\n", 
				n_bytes, n_bytes);
			return 3;
		}
		
		printf("Reading file... ");
		n_iod = fread(databuf, BMP_BYTESPERPIXEL, n_pixels, in);
		printf("%u pixels read - ", n_iod);
		if(n_iod == n_pixels)
			puts("OK");
		else {
			puts("Failed");
			free(databuf);
			return 4;
		}
		
		printf("Writing file... ");
		n_iod = BMPWrite(out, w, h, databuf);
		if(n_iod)
			printf("%d pixels written - OK\n", n_iod);
		else {
			puts("Failed");
			free(databuf);
			return 5;
		}
		
		free(databuf);
	} else if((op == 'e' || op == 'E')) {
		in = fopen(argv[2], "rb");
		if(!in || ferror(in)) {
			printf("Error opening %s for read\n", argv[3]);
			return 21;
		}
		
		out = fopen(argv[3], "wb");
		if(!out || ferror(out)) {
			printf("Error opening %s for write\n", argv[3]);
			return 22;
		}
		
		printf("Reading file... ");
		databuf = BMPRead(in, &w, &h, 0, 0);
		if(!databuf) {
			puts("Failed");
			return 23;
		}
		puts("Done - OK");
		
		n_pixels = w * h;
		printf("Writing file... ");
		n_iod = fwrite((void*)databuf, BMP_BYTESPERPIXEL, n_pixels, out);
		printf("%u pixels written - ", n_iod);
		if(n_iod == n_pixels)
			puts("OK");
		else {
			puts("Failed");
			free(databuf);
			return 24;
		}
		
		free(databuf);
	}
	
	return 0;
}

#endif

size_t __stdcall BMPWrite(FILE* f, int32_t w, int32_t h, uint8_t* data_orig) {
	BMPHeader hdr;
	BMPInfoHeader ihdr;
	size_t pixelct, i, nbytes;
	uint8_t buf[BMP_BYTESPERPIXEL];
	uint8_t* data;
	uint8_t* data_baseloc;
	
	if((w <= 0) || (h <= 0)) return 0;
	if(!f || !data_orig) return 0;
	if(ferror(f)) return 0;
	
	pixelct = w * h;
	nbytes = pixelct * BMP_BYTESPERPIXEL;
	
	data = malloc(nbytes);
	if(!data) return 0;
	data_baseloc = data;
	
	hdr.type = 0x4D42; // ASCII 'BM'
	hdr.filesize = nbytes + sizeof(BMPHeader) + sizeof(BMPInfoHeader);
	hdr.r1 = hdr.r2 = 0;
	hdr.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);
	fwrite(&hdr, sizeof(BMPHeader), 1, f);
	if(ferror(f)) { free(data_baseloc); return 0; }
	
	ihdr.ihsize = sizeof(BMPInfoHeader);
	ihdr.width = w;
	ihdr.height = h;
	ihdr.planes = 1;
	ihdr.bpp = BMP_BITSPERPIXEL;
	ihdr.comprtype = 0; // no compression
	ihdr.imgsize = nbytes;
	ihdr.xres = BMP_DPM;
	ihdr.yres = BMP_DPM;
	ihdr.colorct = ihdr.colorctimp = 0; // no palette
	fwrite(&ihdr, sizeof(BMPInfoHeader), 1, f);
	if(ferror(f)) { free(data_baseloc); return 0; }
	
	// Must be flipped
	memcpy(data, data_orig, nbytes);
	BMPFlipVert(w, h, data);
	
	// Must be reordered from RGB to BGR
	for(i = 0; i < pixelct; i++) {
		buf[0] = data[2];
		buf[1] = data[1];
		buf[2] = data[0];
		fwrite(buf, BMP_BYTESPERPIXEL, 1, f);
		if(ferror(f)) { free(data_baseloc); return 0; }
		data += BMP_BYTESPERPIXEL;
	}
	
	free(data_baseloc);
	return i;
}

void __stdcall BMPFlipVert(int32_t w, int32_t h, uint8_t* data) {
	int32_t line_a, line_b, x;
	uint8_t buf[BMP_BYTESPERPIXEL];
	uint8_t* pos_a;
	uint8_t* pos_b;
	
	line_a = 0;
	line_b = h - 1;
	
	while(line_a < line_b) {
		for(x = 0; x < w; x++) {
			// Swap pixels
			pos_a = data + ((line_a * w + x) * BMP_BYTESPERPIXEL);
			pos_b = data + ((line_b * w + x) * BMP_BYTESPERPIXEL);
			memcpy(buf, pos_a, BMP_BYTESPERPIXEL);
			memcpy(pos_a, pos_b, BMP_BYTESPERPIXEL);
			memcpy(pos_b, buf, BMP_BYTESPERPIXEL);
		}
		
		line_a++;
		line_b--;
	}
}

uint8_t* __stdcall BMPRead(FILE* f, int32_t* w, int32_t* h, uint8_t* databuf,
	size_t databuf_len) {
	BMPHeader hdr;
	BMPInfoHeader ihdr;
	size_t pixelct, i, nbytes, databuf_len_px;
	uint8_t bgrbuf[BMP_BYTESPERPIXEL];
	uint8_t* databuf_curpos;
	int databuf_allocd = 0;
	
	if(!(f && w && h)) return 0;
	if(ferror(f)) return 0;
	if(databuf_len != 0 && (databuf_len % BMP_BYTESPERPIXEL)) return 0;
	
	fread(&hdr, sizeof(BMPHeader), 1, f);
	if(feof(f) || ferror(f)) return 0;
	
	fread(&ihdr, sizeof(BMPInfoHeader), 1, f);
	if(feof(f) || ferror(f)) return 0;
	
	if(ihdr.bpp != BMP_BITSPERPIXEL) return 0;
	*w = ihdr.width;
	*h = ihdr.height;
	pixelct = *w * *h;
	nbytes = pixelct * BMP_BYTESPERPIXEL;
	
	if(!(databuf && databuf_len)) {
		// Databuf not already allocated
		databuf_len = nbytes;
		databuf = malloc(nbytes);
		if(!databuf) return 0;
		databuf_allocd = 1;
	}
	
	databuf_len_px = databuf_len / BMP_BYTESPERPIXEL;
	if(nbytes > databuf_len) {
		if(databuf_allocd) free(databuf);
		return 0;
	}
	
	fseek(f, hdr.offset, SEEK_SET);
	if(ferror(f) || feof(f)) {
		if(databuf_allocd) free(databuf);
		return 0;
	}
	
	databuf_curpos = databuf;
	for(i = 0; i < pixelct; i++) {
		fread(bgrbuf, BMP_BYTESPERPIXEL, 1, f);
		if(ferror(f) || feof(f)) {
			if(databuf_allocd) free(databuf);
			return 0;
		}
		databuf_curpos[0] = bgrbuf[2];
		databuf_curpos[1] = bgrbuf[1];
		databuf_curpos[2] = bgrbuf[0];
		databuf_curpos += BMP_BYTESPERPIXEL;
	}
	
	BMPFlipVert(*w, *h, databuf);
	
	return databuf;
}
	
	


















