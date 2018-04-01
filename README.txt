DISPLAY16: Display images and videos in the Command Prompt.

Purpose of each program:
bmpto16		- Convert 24bpp bitmaps to the display16 image format
defplt		- Restore the Command Prompt to the default palette
display16	- Display an image in the display16 image format
play16		- Play a video in the display16 video format
txtdump		- Dump a text file with a delay between printing each line
The programs will give basic help when run without parameters.

Building:
Install Open Watcom and run build.bat.
Use build_debug.bat to build with debugging symbols.

To do list, for contributors, listed in order of importance:
 - Automatically restore the palette on exit (should work, doesn't now)
 - Add a program to generate videos, ideally from AVI/MP4 input files
 - Improve palette auto-generation
