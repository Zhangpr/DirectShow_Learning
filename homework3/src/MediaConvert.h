/*
	Copyright (C) 2015, Liang Fan
	All rights reverved.
*/

/** \file		MediaConvert.h
    \brief		Header file of media convert functions.
*/

#ifndef __MEDIACONVERT_H__
#define __MEDIACONVERT_H__

unsigned char *getY(unsigned char * in, unsigned char * y, int width, int height);
unsigned char *diff_Y (unsigned char *y_first, unsigned char* y_second, int width, int height);
void diffY_to_rgb24(unsigned char *out, unsigned char *y, int width, int height);
void transform (unsigned char *out, unsigned char *in, int width, int height);

#endif // __MEDIACONVERT_H__
