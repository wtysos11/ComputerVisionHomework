#ifndef SOLUTION_H
#define SOLUTION_H

#include "CImg.h"
#include "Canny.h"
#include "Process.h"
#include "Hough.h"
#include <string>
using namespace std;
using namespace cimg_library;
#define DownSampledSquareSize 500.0
struct Solution{
    CImg<int> source,gray,downsampleImg,cannyImage,HoughSpaceImg,Edge_Point_Img,SrcImgWithVertexAndEdge;
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
        cannyImage.display("EdgeImg");

    }

    void clear()
    {
        source.clear();
        gray.clear();
        downsampleImg.clear();
        cannyImage.clear();
    }
};

#endif // SOLUTION_H
