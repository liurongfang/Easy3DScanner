// stdafx.h: Header file for standard system include files.
//
// Overview:
//   This is the header file included by default in Visual Studio 2005.
//
// Details:
//   Please read the SIGGRAPH 2009 course notes for additional details.
//
//     Douglas Lanman and Gabriel Taubin
//     "Build Your Own 3D Scanner: 3D Photography for Beginners"
//     ACM SIGGRAPH 2009 Course Notes
//
// Author:
//   Douglas Lanman
//   Brown University
//   July 2009

#pragma once

// Exclude rarely-used items from Windows headers.
#define WIN32_LEAN_AND_MEAN

// Define commonly included files.
#include <stdio.h>
#include <tchar.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "MX500IPL.h"
#include <math.h>
#include <direct.h>
#include <conio.h>
#include <windows.h>