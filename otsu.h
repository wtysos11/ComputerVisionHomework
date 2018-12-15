#ifndef OTSU
#define OTSU

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "CImg.h"
#include "hough.h"

using namespace cimg_library;
using namespace std;

#define EDGE 255
#define CHECK 128
#define NOEDGE 0

//腐蚀
void erosion(CImg<int>& dist)
{
    CImg<int> source(dist);

    for(int x = 1;x<=source.width()-1;x++)
    {
        for(int y = 1;y<=source.height()-1;y++)
        {
            dist(x,y,0) = dist(x,y,1) =dist(x,y,2) = 255;
            bool over = false;
            for(int i = x-1;i<=x+1 && !over;i++)
            {
                for(int j = y-1;j<=y+1 && !over;j++)
                {
                    if(source(i,j,0)==0)
                    {
                        dist(x,y,0) = dist(x,y,1) =dist(x,y,2) = 0;
                        over = true;
                        break;
                    }
                }
            }
        }
    }
    source.clear();
}

void dilation(CImg<int>& dist)
{
    CImg<int> source(dist);

    for(int x = 1;x<=source.width()-1;x++)
    {
        for(int y = 1;y<=source.height()-1;y++)
        {
            dist(x,y,0) = dist(x,y,1) =dist(x,y,2) = 0;
            bool over = false;
            for(int i = x-1;i<=x+1 && !over;i++)
            {
                for(int j = y-1;j<=y+1 && !over;j++)
                {
                    if(source(i,j,0)==255)
                    {
                        dist(x,y,0) = dist(x,y,1) =dist(x,y,2) = 255;
                        over = true;
                        break;
                    }
                }
            }
        }
    }
    source.clear();
}


class otsu
{
private:
    CImg<int> gray;
    CImg<int> edge;
public:
    otsu(string filename)
    {
        gray = CImg<int>(filename.c_str());
    }

    int compute(void)
    {
        //产生灰度图像
        map<int,double> hist;
        cimg_forXY(gray,x,y)
        {
            int r = gray(x,y,0);
            int g = gray(x,y,1);
            int b = gray(x,y,2);
            gray(x,y,0) = gray(x,y,1) = gray(x,y,2) =(299*r+587*g+114*b+500)/1000;
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
        edge = CImg<int>(gray.width(),gray.height(),1,3,0);
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
        CImg<int> dst1(gray);
        dilation(dst1);
        erosion(dst1);
        int number1 = 0;
        int number2 = 0;
        cimg_forXY(edge,x,y)
        {
            if(dst1(x,y,0)!=gray(x,y,0))
            {
                edge(x,y,0)=edge(x,y,1)=edge(x,y,2)=255;
                if(dst1(x,y,0)==255)
                    number1++;
                else
                    number2++;
            }
        }

        cout<<number1<<" "<<number2<<endl;
        edge.display();
/*
        Hough hough(edge);
        hough.find_point();

*/
    }
};

#endif // OTSU
