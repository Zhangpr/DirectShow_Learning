/*
	Copyright (C) 2015, Liang Fan
	All rights reverved.
*/

/** \file		MediaConvert.h
    \brief		Header file of media convert functions.
*/

#ifndef __MEDIACONVERT_H__
#define __MEDIACONVERT_H__

int yuy2_to_rgb24(unsigned char *out, unsigned char *in, int width, int height);

unsigned char *getY(unsigned char *const in, int width, int height);
double *DCT8 (unsigned char *const in, int width, int height);
double *zig_zag (double *in, int x, int width, int height);
unsigned char *IDCT8 (double *const in, int width, int height);
unsigned char *writeY (unsigned char *in, unsigned char *const processed_y, int width, int height);
unsigned char *process_Y (unsigned char *in, int x, int width, int height);
void transform (unsigned char *out, unsigned char *in, int width, int height);

#endif // __MEDIACONVERT_H__
