//#include"COLORTOGRAY.h"
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<Windows.h>
#include"pch.h"
#include"afx.h"
#include"func.h"
//1. 腐蚀 2. 膨胀 3.距离变换 4.计算面积 5.灰度图转换 6.处理后图像的展示+图片相减


void inverse(unsigned char* img)
{
    for (int i = 0; i < 960; i++)
    {
        for (int j = 0; j < 1280; j++)
        {
            if (*(img + i * 1280 + j) == 255)
                *(img + i * 1280 + j) = 0;
            else if(*(img + i * 1280 + j) == 0)
                *(img + i * 1280 + j) = 255;
        }
    }
}

void remove(unsigned char* resized_down, const int down_1280, const int down_960)
{
    for (int col = 0; col < down_1280; col++)  //列
    {
        int count = 0;
        for (int row = 0; row < down_960; row++)
        {
            if (*(resized_down + row * down_1280 + col) == 255)
            {
                count++;
            }
        }
        if (count > down_960 / 3 * 2)  //若一列中超过三分之二的像素是白的，则识别其为边界，将其置为黑色
        {
            for (int row = 0; row < down_960; row++)
            {
                *(resized_down + row * down_1280 + col) = 0;
            }
        }
    }

    for (int row = 0; row < down_960; row++)  //行
    {
        int count = 0;
        for (int col = 0; col < down_1280; col++)
        {
            if (*(resized_down + row * down_1280 + col) == 255)
            {
                count++;
            }
        }
        if (count > down_1280 / 10 * 9)  //若一行中超过三分之二的像素是白的，则识别其为边界，将其置为黑色
        {
            for (int col = 0; col < down_1280; col++)
            {
                *(resized_down + row * down_1280 + col) = 0;
            }
        }
    }
}

void binary(unsigned char* ptemp, const int thred)
{
    int binarization_threshold = thred;//80
    for (int i = 0; i < 960; ++i)
    {
        for (int j = 0; j < 1280; ++j)
        {
            if (*(ptemp + i * 1280 + j) > binarization_threshold)
                *(ptemp + i * 1280 + j) = 255;
            else
                *(ptemp + i * 1280 + j) = 0;
        }
    }
}
void distanceTransform(unsigned char* img, unsigned char* dist_map, int width, int height)
{
    unsigned char* up = new unsigned char[1280 * 960];
    unsigned char* down = new unsigned char[1280 * 960];
    unsigned char* left = new unsigned char[1280 * 960];
    unsigned char* right = new unsigned char[1280 * 960];

    for (int a = 0; a < 1280 * 960; a++) *(up + a) = 0;
    for (int a = 0; a < 1280 * 960; a++) *(down + a) = 0;
    for (int a = 0; a < 1280 * 960; a++) *(left + a) = 0;
    for (int a = 0; a < 1280 * 960; a++) *(right + a) = 0;
    int i, j;
    int count;
    //up
    for (j = 0; j < 1280; j++)
    {
        count = 0;
        for (i = 0; i < 960; i++)
        {
            if (*(img + i * 1280 + j) == 0) //0, skip
            {
                count = 0;
                *(up + i * 1280 + j) = count;
            }
            else
            {
                *(up + i * 1280 + j) = ++count;

            }

        }
    }

    //down
    for (j = 0; j < 1280; j++)
    {
        count = 0;
        for (i = 960 - 1; i > 0; i--)
        {
            if (*(img + i * 1280 + j) == 0) //0, skip
            {
                count = 0;
                *(down + i * 1280 + j) = count;
            }
            else
            {
                *(down + i * 1280 + j) = ++count;

            }

        }
    }

    //left
    for (i = 0; i < 960; i++)
    {
        count = 0;
        for (j = 0; j < 1280; j++)
        {
            if (*(img + i * 1280 + j) == 0) //0, skip
            {
                count = 0;
                *(left + i * 1280 + j) = count;
            }
            else
            {
                *(left + i * 1280 + j) = ++count;

            }

        }
    }

    //right
    for (i = 0; i < 960; i++)
    {
        count = 0;
        for (j = 1280 - 1; j > 0; j--)
        {
            if (*(img + i * 1280 + j) == 0) //0, skip
            {
                count = 0;
                *(right + i * 1280 + j) = count;
            }
            else
            {
                *(right + i * 1280 + j) = ++count;

            }

        }
    }

    for (i = 0; i < 960; i++)
    {
        for (j = 0; j < 1280; j++)
        {
            *(dist_map + i * 1280 + j) = 10*min(
                min(*(up + i * 1280 + j), *(down + i * 1280 + j)),
                min(*(left + i * 1280 + j), *(right + i * 1280 + j))
            );
        }
    }
}


int copyfrom_2_to_1(unsigned char* ptemp, unsigned char* ptemp2)//将ptemp2复制给ptemp
{
    for (int i = 0; i < 960; ++i)
    {
        for (int j = 0; j < 1280; ++j)
        {
            *(ptemp + i * 1280 + j) = *(ptemp2 + i * 1280 + j);
        }
    }
    return 0;
}


int img_2_minus_1(unsigned char* out, unsigned char* in)  //图片相减，ptemp - ptemp2
{
    for (int i = 0; i < 960; i++)
    {
        for (int j = 0; j < 1280; j++)
        {
            if (*(in + i * 1280 + j) == 255)
                *(out + i * 1280 + j) = 0;
        }
    }
    return 0;
}



//在界面中显示ptemp的内容
int convert(unsigned char* pDst, unsigned char* ptemp) //将ptemp的内容输入给pDst
{
    //将位图信息转为灰度
    //存储bmp一行的像素点*/
    int i, j, channel = 0;


    //公式 
    int n = 0;
    for (i = 0; i < 960; i++)
    {
        for (j = 0; j < 1280; j++)
        {
            for (channel = 0; channel < 3; channel++)
            {
                *(pDst + n) = *(ptemp + (i)*1280 + j);
                n++;
            }
        }
    }

    return 0;
}


//将原图和处理后的图片叠加
int color(unsigned char* pDst, unsigned char* ptemp, const char* color)//pDst原图，ptemp是处理后
{
    //将位图信息转为灰度
    //存储bmp一行的像素点*/
    int i, j, channel = 0;
    int offset = 0;

    //公式 
    if (strcmp(color, "red") == 0)
    {
        for (i = 0; i < 960; i++)
        {
            for (j = 0; j < 1280; j++)
            {
                if (*(ptemp + (i)*1280 + j) == 255)
                {

                    //*(pDst + (i)*1280 + j) =255;
                    offset = 3 * ((i)*1280 + j);
                    *(pDst + offset) = 255;  //R
                    *(pDst + offset + 1) = 0;  //G
                    *(pDst + offset + 2) = 0;  //B
                }

            }

            //将灰度图信息写入
            //fwrite(ImgData2, j, 1, fp2);
        }
    }
    if (strcmp(color, "green") == 0)
    {
        for (i = 0; i < 960; i++)
        {
            for (j = 0; j < 1280; j++)
            {
                if (*(ptemp + (i)*1280 + j) == 255)
                {

                    //*(pDst + (i)*1280 + j) =255;
                    offset = 3 * ((i)*1280 + j);
                    *(pDst + offset) = 0;  //R
                    *(pDst + offset + 1) = 255;  //G
                    *(pDst + offset + 2) = 0;  //B
                }

            }

            //将灰度图信息写入
            //fwrite(ImgData2, j, 1, fp2);
        }
    }

    return 0;
}



//腐蚀函数
int corrupt(unsigned char* ptemp, unsigned char* ptemp2, int corrupt_scale)
{
    //const int corrupt_scale = 22;//腐蚀程度
    int sign = 0;

    //clearpic_d(ptemp2);


    for (int y = corrupt_scale; y < (960 - corrupt_scale); ++y)
    {
        for (int x = corrupt_scale; x < 1280 - corrupt_scale; ++x)
        {
            sign = 0;
            for (int j = 1; j <= corrupt_scale && sign == 0; j++)//up
                if(*(ptemp + (y + j) * 1280 + x)==0)
                { 
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }


            for (int j = 1; j <= corrupt_scale && sign==0; j++)//down
                if (*(ptemp + (y - j) * 1280 + x) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }
                    

            for (int j = 1; j <= corrupt_scale && sign == 0; j++)//right
                if (  *(ptemp + (y)*1280 + (x + j)) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }

            for (int j = 1; j <= corrupt_scale && sign == 0; j++)//left
                if (*(ptemp + (y)*1280 + (x - j)) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }

            for (int j = 1; j <= corrupt_scale && sign == 0; j++)
                if (*(ptemp + (y + j) * 1280 + (x + j)) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }
             

            for (int j = 1; j <= corrupt_scale && sign == 0; j++)
                if (*(ptemp + (y + j) * 1280 + (x - j)) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }
             

            for (int j = 1; j <= corrupt_scale && sign == 0; j++)
                if (*(ptemp + (y - j) * 1280 + (x + j)) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }
               

            for (int j = 1; j <= corrupt_scale && sign == 0; j++)
                if (*(ptemp + (y - j) * 1280 + (x - j)) == 0)
                {
                    *(ptemp2 + y * 1280 + x) = 0;
                    sign = 1;
                    break;
                }       

            if (sign == 0)
            {
                *(ptemp2 + y * 1280 + x) = 255;
            }

            
        }
    }
    return 0;
}




//将图片所有像素都设置为白色（不属于豆子）
int all2white(unsigned char* ptemp)
{
    for (int i = 0; i < 960; ++i)
    {
        for (int j = 0; j < 1280; ++j)
        {
            *(ptemp + i * 1280 + j) = 255;//默认全部不属于豆子
        }
    }
    return 0;
}

//将图片所有像素都设置为黑色（属于豆子）
int all2black(unsigned char* ptemp)
{
    for (int i = 0; i < 960; ++i)
    {
        for (int j = 0; j < 1280; ++j)
        {
            *(ptemp + i * 1280 + j) = 0;//默认全部属于豆子
        }
    }
    return 0;
}



void dilate(unsigned char* out, unsigned char* in, int scale)
{
    for (int y = 1; y < 960-1; y++)
    {
        for (int x = 1; x < 1280-1; x++)
        {
            if (*(in + y * 1280 + x) ==255)
            {
                for (int y_ = -scale; y_ < scale; y_++)
                {
                    for (int x_ = -scale; x_ < scale; x_++)
                    {
                        if (sqrt(double(y_ * y_ + x_ * x_)) < scale  && (y + y_)>=0 && (y + y_)<960 && (x+x_)>=0 && (x+x_)<1280)
                        {
                            *(out + (y + y_) * 1280 + (x + x_)) = 255;
                        }
                    }
                }
            }
        }
    }
}



int colorToGray(unsigned char *pDst, unsigned char *img2)
{

    //将位图信息转为灰度
    //存储bmp一行的像素点*/
    int i, j, k = 0;
    unsigned char img[3000][3];
    //将灰度图像存到一维数组中
    unsigned char img3[3000];


    //公式 
    int m = 0,n = 0;
    for (i = 0; i < 960; i++)
    {
        for (j = 0; j < 1280; j++)
        {
            for (k = 0; k < 3; k++)
            {
                img[j][k] = *(pDst+n);
                n++;
            }    
        }
        for (j = 0; j < 1280 ; j++)
        {
            img3[j] = int((float)img[j][0] * 0.114 +
                (float)img[j][1] * 0.587 +
                (float)img[j][2] * 0.299);
            img2[m] = img3[j];
            m++;
        }
        //将灰度图信息写入
        //fwrite(ImgData2, j, 1, fp2);
    }

    return 1;
}

int ConnectedArea(int i, int j, unsigned char* ptemp, int width, int *m)
{
    if (i >= 960 || j >= 1280 || i<=0 || j<=0)
    {
        return 0;
    }

    *m += 1;

    //255  0
    *(ptemp + (i)*1280 + j) = 0;
    
    if (*(ptemp + (i)*1280 + j + 1) == 255)
    {
        ConnectedArea(i, j + 1, ptemp, 1280, m);
    }
    if (*(ptemp + (i)*1280 + j - 1) == 255)
    {
        ConnectedArea(i, j - 1, ptemp, 1280, m);
    }
    if (*(ptemp + (i + 1) * 1280 + j) == 255)
    {
        ConnectedArea(i + 1, j, ptemp, 1280, m);
    }
    if (*(ptemp + (i - 1) * 1280 + j) == 255)
    {
        ConnectedArea(i - 1, j, ptemp, 1280, m);
    }
    return 0;
    
}