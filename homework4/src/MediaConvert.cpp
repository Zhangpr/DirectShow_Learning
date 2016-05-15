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
#include <stdio.h>


#define Clip3(a, b, c) ((c < a) ? a : ((c > b) ? b : c))
#define Clip1(x)       Clip3(0, 255, x)

unsigned char *yFormer = new unsigned char[1000 * 1000];
unsigned char *yLatter = new unsigned char[1000 * 1000];
unsigned char *yOut = new unsigned char[1000 * 1000];
int range = 8;   //�˶�������Χ

//��ȡYUV�е���������Y
unsigned char *getY(unsigned char *in, unsigned char *y, int width, int height) {
	unsigned count = 0;
	for (unsigned i = 0; i < height; ++i) {
        for(unsigned j = 0; j < width / 2; ++j) {
            y[count++] = in [i * width * 2 + j * 4];
			y[count++] = in [i * width * 2 + j * 4 + 2];
		}
	}
    return y;
}

//���������
int SAD(int pos1, int pos2, int width, int height) {
	int result = 0;
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 16; ++j) {
			result += abs(yLatter[pos2 + i * width + j] - yFormer[pos1 + i * width + j]);
		}
	}
	return result;
}

//�˶�����
int motionSearch(int pos, int width, int height) {
	int left, right, top, bot;
	//�������������еķ�Χ
	if (pos / width < range * 16) {
		top = 0;
		bot = pos / width + 16 * range;
	}
	else if  (pos / width <= 16 * (height / 16 - 1 - range)) {
		top = pos / width - 16 * range;
		bot = pos / width + 16 * range;
	}
	else {
		top = pos / width - 16 * range;
		bot = (height / 16 - 1) * 16;
	}

	//�������������еķ�Χ
	if (pos % width < range * 16) {
		left = 0;
		right = pos % width + 16 * range;
	}
	else if (pos % width <= 16 * (width / 16 - 1 - range)) {
		left = pos % width - 16 * range;
		right = pos % width + 16 * range;
	}
	else {
		left = pos % width - 16 * range;
		right = (width / 16 - 1) * 16;
	}

	//ȷ�������������˶�ʸ����С�Ŀ�
	//int minVec = SAD(top * width + left, pos, width, height);
	//int minPos = top * width + left;
	int minVec = SAD(pos, pos, width, height);
	int minPos = pos;
	int tmp;
	for (int i = top; i <= bot; i += 16) {
		for (int j = left; j <= right; j += 16) {
			tmp = SAD(i * width + j, pos, width, height);
			if (tmp < minVec) {
				minVec = tmp;
				minPos = i * width + j;
			}
		}
	}
	return minPos;
}

//������Ϣת��ΪRGB24��ʽ
void toRGB24(unsigned char *out, int width, int height) {
    int  i, j;
    int  r0, g0, b0,
		 r1, g1, b1;
	int count = 0;
    for (i = 0; i < height; ++i) {
        for(j = 0; j < width / 2; ++j) {
           	r0 = g0 = b0 = yOut[count++];
			r1 = g1 = b1 = yOut[count++];

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

//������Ƶ���ݵ�ת������
void transform(unsigned char *out, unsigned char *in, int width, int height) {
	static int count = 0;
	//��һ֡�Ĵ���
	if (!count) {
		getY(in, yFormer, width, height);
		++count;
	}

	//����֡�Ĵ���
	else {
		getY(in, yLatter, width, height);
		int refPos;
		//��ÿ��������˶��������ҵ��ο���
		for (int i = 0; i < height; i += 16) {
			for (int j = 0; j < width; j += 16) {
				refPos = motionSearch(i * width + j, width, height);
				//�����Ȳο��鸴�Ƶ��������
				for (int row = 0; row < 16; ++row) {
					for (int col = 0; col < 16; ++col) {
						yOut[i * width + j + row * width + col] = yFormer[refPos + row * width + col];
					}
				}
			}
		}
		
		toRGB24(out, width, height);    //ת��ΪRGB24��ʽ���

		unsigned char *tmpPtr = yFormer;
		yFormer = yLatter;
		yLatter = tmpPtr;
		++count;
	}
}