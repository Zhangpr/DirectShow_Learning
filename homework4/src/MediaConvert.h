/*
	Copyright (C) 2015, Liang Fan
	All rights reverved.
*/

/** \file		MediaConvert.h
    \brief		Header file of media convert functions.
*/

#ifndef __MEDIACONVERT_H__
#define __MEDIACONVERT_H__

unsigned char *getY(unsigned char *in, unsigned char *y, int width, int height);
int SAD(int pos1, int pos2, int width, int height);
int motionSearch(int pos, int width, int height);
void toRGB24(unsigned char *out, int width, int height);
void transform(unsigned char *out, unsigned char *in, int width, int height);

#endif // __MEDIACONVERT_H__
