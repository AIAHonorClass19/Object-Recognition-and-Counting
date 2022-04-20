#ifndef COUNTNUMBER_H
#define COUNTNUMBER_H

int countnumber(unsigned char* pDst, unsigned char* pBmpBuf, unsigned char* pBmpBuf2, unsigned char* pBmpBuf3,int *number1,int*number2,int*m);
int colorToGray(unsigned char* pDst, unsigned char* ImgData);
int ConnectedArea(int i, int j, unsigned char* pBmpBuf, int lineByte, int* m);
#endif
#pragma once
