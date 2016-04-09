/*
	Copyright (C) 2015, Liang Fan
	All rights reverved.
*/

/** \file		MediaConvert.cpp
    \brief		Implements convert functions.
*/

#include <string.h>
#include "MediaConvert.h"

//
#define Clip3(a, b, c) ((c < a) ? a : ((c > b) ? b : c))
#define Clip1(x)       Clip3(0, 255, x)

int yuy2_to_rgb24(unsigned char *out, unsigned char *in, int width, int height)
{
    int  i, j;
    int  r0, g0, b0,
		 r1, g1, b1;
    int  y0, u0, v0, y1;
    unsigned char *ip, *op;

    // initialize buf pointer
    op = out;
    ip = in;

    for (i = 0; i < height; ++i)
    {
        for(j = 0; j < width / 2; ++j)
        {
            // get YUY2 data
            y0 = (int) *(ip + i*width*2 + j*4)     - 16;
			u0 = (int) *(ip + i*width*2 + j*4 + 1) - 128;
			y1 = (int) *(ip + i*width*2 + j*4 + 2) - 16;
			v0 = (int) *(ip + i*width*2 + j*4 + 3) - 128;
            
			//C = Y - 16  
			//D = U - 128  
			//E = V - 128  
			//R = clip(( 298 * C           + 409 * E + 128) >> 8)  
			//G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)  
			//B = clip(( 298 * C + 516 * D           + 128) >> 8)  
			r0 = (298 * y0 + 409 * v0 + 128) >> 8;
			r1 = (298 * y1 + 409 * v0 + 128) >> 8;

			g0 = (298 * y0 - 100 * u0 - 208 * v0 + 128) >> 8;
			g1 = (298 * y1 - 100 * u0 - 208 * v0 + 128) >> 8;

			b0 = (298 * y0 + 516 * u0 + 128) >> 8;
			b1 = (298 * y1 + 516 * u0 + 128) >> 8;

            // RGB24
            // unsigned char: B0, G0, R0, B1, G1, R1, ...
            // note: RGB24 is bottom line first.
            *(op + (height-i-1)*width*3 + j*6 + 2) = (unsigned char)Clip1(r0);
            *(op + (height-i-1)*width*3 + j*6 + 1) = (unsigned char)Clip1(g0);
            *(op + (height-i-1)*width*3 + j*6    ) = (unsigned char)Clip1(b0);
            *(op + (height-i-1)*width*3 + j*6 + 5) = (unsigned char)Clip1(r1);
            *(op + (height-i-1)*width*3 + j*6 + 4) = (unsigned char)Clip1(g1);
            *(op + (height-i-1)*width*3 + j*6 + 3) = (unsigned char)Clip1(b1);
            
        }
    }
    return 0;
}