#ifndef SIMPLEIMGPROCESS_H
#define SIMPLEIMGPROCESS_H

#include "CImg.h"
using namespace cimg_library;

//��ɫͼ��ת��Ϊ�Ҷ�ͼ
CImg<int> grayTheImg(const CImg<int>& SrcImg);
//�ԻҶ�ͼ�����²���
CImg<int> downSampleTheImg(const CImg<int>& SrcGrayImg, float sampleSquareSize);


#endif
