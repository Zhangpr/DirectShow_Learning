/*
	Copyright (C) 2015, Liang Fan
	All rights reverved.
*/

/** \file		MediaConvert.cpp
    \brief		Implements convert functions.
*/

#include <string.h>
#include <math.h>
#include "MediaConvert.h"


#define Clip3(a, b, c) ((c < a) ? a : ((c > b) ? b : c))
#define Clip1(x)       Clip3(0, 255, x)

const double PI = 3.141592654;
unsigned char *y_1 = new unsigned char[1000 * 1000];
unsigned char *y_2 = new unsigned char[1000 * 1000];

//读取YUV中的亮度数据Y
unsigned char *getY(unsigned char * in, unsigned char * y, int width, int height) {
	unsigned count = 0;
	for (unsigned i = 0; i < height; ++i) {
        for(unsigned j = 0; j < width / 2; ++j) {
            y[count++] = in [i * width * 2 + j * 4];
			y[count++] = in [i * width * 2 + j * 4 + 2];
		}
	}
    return y;
}

//计算两帧之间Y的差值
unsigned char *diff_Y (unsigned char *y_first, unsigned char* y_second, int width, int height) {
	for (unsigned int i = 0; i < width * height; ++i) {
		y_first[i] = abs(y_first[i] - y_second[i]);
	}
	return y_first;
}

//差值图像转化为RGB24并输出
void diffY_to_rgb24(unsigned char *out, unsigned char *y, int width, int height) {
    int  i, j;
    int  r0, g0, b0,
		 r1, g1, b1;
	int count = 0;
    for (i = 0; i < height; ++i) {
        for(j = 0; j < width / 2; ++j) {
           	r0 = g0 = b0 = y[count++];
			r1 = g1 = b1 = y[count++];

            // RGB24
            // unsigned char: B0, G0, R0, B1, G1, R1, ...
            // note: RGB24 is bottom line first.
            *(out + (height-i-1)*width*3 + j*6 + 2) = (unsigned char)Clip1(r0);
            *(out + (height-i-1)*width*3 + j*6 + 1) = (unsigned char)Clip1(g0);
            *(out + (height-i-1)*width*3 + j*6    ) = (unsigned char)Clip1(b0);
            *(out + (height-i-1)*width*3 + j*6 + 5) = (unsigned char)Clip1(r1);
            *(out + (height-i-1)*width*3 + j*6 + 4) = (unsigned char)Clip1(g1);
            *(out + (height-i-1)*width*3 + j*6 + 3) = (unsigned char)Clip1(b1);
        }
    }
}

void transform(unsigned char *out, unsigned char *in, int width, int height) {
	static int count = 0;
	if (!count) {
		getY(in, y_1, width, height);
		++count;
		return;
	}
	else {
		getY(in, y_2, width, height);
		diff_Y(y_1, y_2, width, height);
		diffY_to_rgb24(out, y_1, width, height);
		unsigned char* tmp = y_1;
		y_1 = y_2;
		y_2 = tmp;
		++count;
	}
}