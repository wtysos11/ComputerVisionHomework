#ifndef SIMPLEIMGPROCESS_H
#define SIMPLEIMGPROCESS_H

#include "CImg.h"
using namespace cimg_library;

//��ɫͼ��ת��Ϊ�Ҷ�ͼ
CImg<int> makeGrayImage(const CImg<int>& src);
//�ԻҶ�ͼ�����²���
CImg<int> downSample(const CImg<int>& SrcGrayImg, float sampleSquareSize);


#endif
