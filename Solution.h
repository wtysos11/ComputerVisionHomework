#ifndef SOLUTION_H
#define SOLUTION_H

#include "CImg.h"
#include "Canny.h"
#include "Process.h"
#include "Hough.h"
#include "A4Correct.h"
#include "NumberExtract.h"
#include <string>
#include <iostream>

using namespace std;
using namespace cimg_library;
#define DownSampledSquareSize 500.0
/*
测试样本需要与训练样本一致，保留白色的梯度部分（黑色为纯黑，白色有变化）
训练样本与测试样本的数字粗细尽量变为一致

二值化在小的图像上做效果会比较好
*/
struct Solution{
    CImg<int> source,gray,downsampleImg,cannyImage,a4Image;
    string filename;
    Solution(string filename)
    {
        this->filename = filename;
    }

    void mainProcess()
    {
        source = CImg<int>("test.bmp");
        gray = makeGrayImage(source);
        downsampleImg = downSample(gray, DownSampledSquareSize);
        cannyImage = canny(downsampleImg, downsampleImg.width(), downsampleImg.height());
        Hough hough(cannyImage);
        vector<Vertex> top4(hough.compute());
#ifdef DEBUG
        cout<<"four vertexs in downsample graph"<<endl;
        for(int i = 0;i<4;i++)
        {
            cout<<top4[i].x<<"\t"<<top4[i].y<<endl;
        }
#endif
        a4Image = transformToA4(source,downsampleImg.width(),downsampleImg.height(),top4);
        NumberExtract num(a4Image);
        num.compute();
        a4Image.save("a4.bmp");

        num.clear();
    }

    void clear()
    {
        source.clear();
        gray.clear();
        downsampleImg.clear();
        cannyImage.clear();
        a4Image.clear();
    }
};

#endif // SOLUTION_H
