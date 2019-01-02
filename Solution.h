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
����������Ҫ��ѵ������һ�£�������ɫ���ݶȲ��֣���ɫΪ���ڣ���ɫ�б仯��
ѵ��������������������ִ�ϸ������Ϊһ��

��ֵ����С��ͼ������Ч����ȽϺ�
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
        source = CImg<int>(filename.c_str());
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
        //cannyImage.display();
#endif

        a4Image = transformToA4(source,downsampleImg.width(),downsampleImg.height(),top4);
        NumberExtract num(a4Image);
        num.compute();

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
