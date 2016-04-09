/*
	Copyright (C) 2015, Liang Fan
	All rights reverved.
*/

/** \file		MediaConvert.cpp
    \brief		Implements convert functions.
*/

#include <string.h>
#include <cmath>
#include "MediaConvert.h"

//
#define Clip3(a, b, c) ((c < a) ? a : ((c > b) ? b : c))
#define Clip1(x)       Clip3(0, 255, x)

const double PI = 3.141592654;

//读取YUV中的亮度数据Y
unsigned char *getY(unsigned char *const in, int width, int height) {
	unsigned char *y = new unsigned char[width * height];
	unsigned count = 0;
	for (unsigned i = 0; i < height; ++i) {
        for(unsigned j = 0; j < width / 2; ++j) {
            y[count++] = in [i * width * 2 + j * 4];
			y[count++] = in [i * width * 2 + j * 4 + 2];
		}
	}
    return y;
}

//对数据分块进行8*8 DCT
double *DCT8 (unsigned char *const in, int width, int height) {
	double *DCT_in = new double[width * height];
	//对数据进行分块
	unsigned char *row_block = in, *first = row_block;
	for (/* 空 */; row_block < in + height * width; row_block += 8 * width) {
		for (first = row_block; first < row_block + width; first += 8) {
			//对每个分块分别进行DCT
			for (unsigned u = 0; u < 8; ++u) {
				for (unsigned v = 0; v < 8; ++v) {
					double Cu = !u ? 1 / sqrt(2.0) : 1;
					double Cv = !v ? 1 / sqrt(2.0) : 1;
					double sum = 0;
					for (unsigned i = 0; i < 8; ++i)
						for (unsigned j = 0; j < 8; ++j)
							sum += first[i * width + j] * 
							       cos((2 * i + 1) * u * PI / 16) * 
								   cos((2 * j + 1) * v * PI / 16);
					DCT_in[first - in + u * width + v] = 0.25 * Cu * Cv * sum;
				}
			}
		}
	}
	delete []in;
	return DCT_in;
}

//按zig-zag顺序保留前x个系数
double *zig_zag (double *in, int x, int width, int height) {
	if (x >= 1 && x <= 64) {
		//对数据进行分块
		double *row_block = in, *first = row_block;
		for (/* 空 */; row_block < in + height * width; row_block += 8 * width) {
			for (first = row_block; first < row_block + width; first += 8) {
				//对每个分块按zig-zag顺序保留前x个系数
				unsigned u = 0, v = 0;
				bool flag = false;
				for (unsigned i = 1; i < 64; ++i) {
					if (flag) {
						if (!v && u < 7) {
							++u; flag = false;
						}
						else if (u == 7) {
							++v; flag = false;
						}
						else {
							++u; --v;
						}
					}
					else {
						if (!u && v < 7) {
							++v; flag = true;
						}
						else if (v == 7) {
							++u; flag = true;
						}
						else {
							--u; ++v;
						}
					}
					if (i >= x)
						first[u * width + v] = 0;
				}
			}
		}
	}
	return in;
}

//对数据分块进行8*8 IDCT
unsigned char *IDCT8 (double *const in, int width, int height) {
	unsigned char *IDCT_in = new unsigned char[width * height];
	//对数据进行分块
	double *row_block = in, *first = row_block;
	for (/* 空 */; row_block < in + height * width; row_block += 8 * width) {
		for (first = row_block; first < row_block + width; first += 8) {
			//对每个分块分别进行IDCT
			for (unsigned i = 0; i < 8; ++i) {
				for (unsigned j = 0; j < 8; ++j) {
					double sum = 0;
					for (unsigned u = 0; u < 8; ++u) {
						for (unsigned v = 0; v < 8; ++v) {
							double Cu = !u ? 1 / sqrt(2.0) : 1;
							double Cv = !v ? 1 / sqrt(2.0) : 1;
							sum += Cu * Cv * first[u * width + v] * 
							       cos((2 * i + 1) * u * PI / 16) * 
								   cos((2 * j + 1) * v * PI / 16);
						}
					}
					IDCT_in[first - in + i * width + j] = int(0.25 * sum + 0.5);
				}
			}
		}
	}
	delete []in;
	return IDCT_in;
}

//将变换后的亮度数据Y写到YUY2数据流中
unsigned char *writeY (unsigned char *in, unsigned char *const processed_y, int width, int height) {
	unsigned count = 0;
	for (unsigned i = 0; i < height; ++i) {
        for(unsigned j = 0; j < width / 2; ++j) {
            in[i * width * 2 + j * 4] = processed_y[count++];
			in[i * width * 2 + j * 4 + 2] = processed_y[count++];
		}
	}
	delete []processed_y;
	return in;
}

//对YUY2格式的的亮度数据Y进行DCT、zig-zag保留和IDCT处理
unsigned char *process_Y (unsigned char *in, int x, int width, int height) {
	unsigned char *tmp_char = getY(in, width, height);       //读取亮度数据Y
	double *tmp_double = DCT8(tmp_char, width, height);      //对亮度数据Y进行DCT
	tmp_double = zig_zag(tmp_double, x, width, height);      //按zig-zag顺序保留x个系数
	tmp_char = IDCT8(tmp_double, width, height);             //对亮度数据Y进行IDCT
	return writeY(in, tmp_char, width, height);              //将变换后的亮度数据Y写到YUY2数据流中
}

//yuy2格式到rgb24格式的转换
int yuy2_to_rgb24(unsigned char *out, unsigned char *in, int width, int height) {
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

//YUY2格式文件进行DCT压缩后再转换成RGB24输出
void transform (unsigned char *out, unsigned char *in, int width, int height) {
	int x = 32;  //设置zig-zag保留的系数个数
	unsigned char *tmp = process_Y(in, x, width, height);
	yuy2_to_rgb24(out, tmp, width, height);
}