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
int all2white(unsigned char* ptemp);//��ջ���
int all2black(unsigned char* ptemp);
int colorToGray(unsigned char* pDst, unsigned char* ImgData);//ת�Ҷ�ͼ��
int ConnectedArea(int i, int j, unsigned char* ptemp, int lineByte, int* m);//�������
int corrupt(unsigned char* ptemp, unsigned char* ptemp2, int corrupt_scale);//��ʴ
int convert(unsigned char* pDst, unsigned char* ptemp);//չʾ������ͼ��
void dilate(unsigned char* ptemp, unsigned char* ptemp2, int dilate_scale);//����
int img_2_minus_1(unsigned char* out, unsigned char* in);//ͼƬ���
int copyfrom_2_to_1(unsigned char* ptemp, unsigned char* ptemp2);//����ͼƬ
int color(unsigned char* pDst, unsigned char* ptemp, const char* color);//ԭͼ�봦������ͼƬ����

//1. ��ʴ 2. ���� 3.����任 4.������� 5.�Ҷ�ͼת�� 6.�����ͼ���չʾ+ͼƬ���

//int width = 1280;
//int height = 960;
//int biCount = infoHead.biBitCount;

//int lineByte = 1280; //#pragma once
