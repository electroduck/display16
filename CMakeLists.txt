cmake_minimum_required(VERSION 3.0)

project("display16") 

#set c++ Standard to use
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "x64 is not supported yet!")
endif()

if(NOT WIN32)
	message(FATAL_ERROR "This Platform is not supported yet!" )
endif()


set(bmpto16_SRC 
		"image16c.h"
		"image16c.c"
		"bmpto16.c"
		"bmp.h"
		"bmp.c"
	)

set(defplt_SRC 
		"defplt.c"
		"image16c.h"
		"image16c.c"
	)
	
set(display16_SRC
	"display16.c"
	"image16c.h"
	"image16c.c"
	)

set(play16_SRC
	"play16.c"
	"image16c.h"
	"image16c.c"
	)	


add_executable(bmpto16 ${bmpto16_SRC})
add_executable(defplt  ${defplt_SRC})
add_executable(display16  ${display16_SRC})

link_libraries(winmm.lib)
add_executable(play16  ${play16_SRC})

