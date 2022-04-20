#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<Windows.h>
#include"pch.h"
#include"afx.h"
void dilate(unsigned char* out, unsigned char* in, double scale);
void inverse(unsigned char* img);
void remove(unsigned char* resized_down, const int down_width, const int down_height);
void binary(unsigned char* ptemp, const int thred);
void distanceTransform(unsigned char* img, unsigned char* dist_map, int width, int height);
int all2white(unsigned char* ptemp);//清空画布
int all2black(unsigned char* ptemp);
int colorToGray(unsigned char* pDst, unsigned char* ImgData);//转灰度图像
int ConnectedArea(int i, int j, unsigned char* ptemp, int lineByte, int* m);//计算面积
int corrupt(unsigned char* ptemp, unsigned char* ptemp2, int corrupt_scale);//腐蚀
int convert(unsigned char* pDst, unsigned char* ptemp);//展示处理后的图像
void dilate(unsigned char* ptemp, unsigned char* ptemp2, int dilate_scale);//膨胀
int img_2_minus_1(unsigned char* out, unsigned char* in);//图片相减
int copyfrom_2_to_1(unsigned char* ptemp, unsigned char* ptemp2);//复制图片
int color(unsigned char* pDst, unsigned char* ptemp, const char* color);//原图与处理过后的图片叠加

//1. 腐蚀 2. 膨胀 3.距离变换 4.计算面积 5.灰度图转换 6.处理后图像的展示+图片相减

//int width = 1280;
//int height = 960;
//int biCount = infoHead.biBitCount;

//int lineByte = 1280; //#pragma once
