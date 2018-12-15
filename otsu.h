#ifndef OTSU
#define OTSU

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

#define EDGE 255
#define CHECK 128
#define NOEDGE 0
class otsu
{
private:
    CImg<double> source;
    CImg<double> gray;
    CImg<double> edge;
public:
    otsu(string filename)
    {
        source = CImg<double>(filename.c_str());
    }

    int compute(void)
    {
        //产生灰度图像
        map<int,double> hist;
        gray = source;
        cimg_forXY(gray,x,y)
        {
            double r = gray(x,y,0);
            double g = gray(x,y,1);
            double b = gray(x,y,2);
            gray(x,y,0) = gray(x,y,1) = gray(x,y,2) = 0.299*r+0.587*g+0.114*b;
            source(x,y,0) = source(x,y,1) = source(x,y,2) = 0.299*r+0.587*g+0.114*b;
            int grayNum = gray(x,y,0);
            if(hist.find(grayNum)==hist.end())
            {
                hist[grayNum] = 1;
            }
            else
            {
                hist[grayNum]++;
            }
        }
        int size = gray.height()*gray.width();
        double u = 0;//平均灰度
        for(int i = 0;i<256;i++)
        {
            if(hist.find(i)!=hist.end())
            {
                hist[i]/=size;
                u+=i*hist[i];
            }
        }

        double w0 = 0;//前景灰度
        int threshold=0;//最终阈值
        double average = 0;
        double maxVariance = 0;
        for(int i = 0;i<256;i++)
        {
            if(hist.find(i)==hist.end())
                continue;
            w0 += hist[i];//前景灰度比例
            average += i*hist[i];

            double t = average/w0-u;//u0-u
            double gbVariance = t*t*w0/(1-w0);
            if(gbVariance>maxVariance)
            {
                maxVariance = gbVariance;
                threshold = i;
            }
        }

        return threshold;
    }

    void outputBio(int threshold)
    {
        edge = CImg<double>(gray.width(),gray.height(),1,3,0);
        cimg_forXY(gray,x,y)
        {
            edge(x,y,0) = NOEDGE;
            if(gray(x,y,0)<threshold)
            {
                gray(x,y,0)=gray(x,y,1)=gray(x,y,2) = 0;
            }
            else
            {
                gray(x,y,0)=gray(x,y,1)=gray(x,y,2) = 255;
            }

        }

        //先膨胀，后腐蚀，闭运算
        //膨胀
        for(int x = 1;x<=gray.width()-1;x++)
        {
            for(int y = 1;y<=gray.height()-1;y++)
            {

                source(x,y,0) = source(x,y,1) =source(x,y,2) = 0;
                bool over = false;
                for(int i = x-1;i<=x+1 && !over;i++)
                {
                    for(int j = y-1;j<=y+1 && !over;j++)
                    {
                        if(gray(i,j,0)==255)
                        {
                            source(x,y,0) = source(x,y,1) =source(x,y,2) = 255;
                            over = true;
                            break;
                        }
                    }
                }
            }
        }
        //如果该元素周围3*3全为白色，则变为黑色。其他照常
        for(int x = 1;x<=gray.width()-1;x++)
        {
            for(int y = 1;y<=gray.height()-1;y++)
            {

                gray(x,y,0) = gray(x,y,1) =gray(x,y,2) = source(x,y,0);
                bool over = false;
                for(int i = x-1;i<=x+1 && !over;i++)
                {
                    for(int j = y-1;j<=y+1 && !over;j++)
                    {
                        if(source(i,j,0)==0)
                        {
                            over = true;
                            break;
                        }
                    }
                }
                if(!over)
                {
                    gray(x,y,0) = gray(x,y,1) =gray(x,y,2) = 0;
                }
            }
        }

        gray.display();

    }
};

#endif // OTSU
