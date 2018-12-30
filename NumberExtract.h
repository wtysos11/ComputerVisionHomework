#ifndef NUMBEREXTRACT_H
#define NUMBEREXTRACT_H

#include "CImg.h"
#include <vector>
#include <iostream>
#include <utility>
#include <string>
#define DEBUG
//��ֵ��ͼ��ķֽ��
#define BITHRESHOLD 150
//��ֱ�Ҷ�ֱ��ͼ�Ķ�����ֵ
#define VERTICAL_GRAY_THRESHOLD 10
#define MARGIN 0.05//A4��ȡ����Ե

#define VERTICAL_NUM 9//��9����ֱ�����������
using namespace std;
using namespace cimg_library;

struct NumberExtract
{
public:
    CImg<int> a4paper,bipaper,anspaper,cachepaper;
    NumberExtract(CImg<int> a4)
    {
        a4paper = a4;
    }

    /*
    ������ڣ�
    �ֿ���ж�ֵ������
    ��ȡ�ַ���
    ���ַ����ڲ����зָ�
    �����ֱ�Ϊ784ά����

    ������ȡ�ַ��е��뷨��
        ���ڴ�ֱ����ĻҶ�ֱ��ͼ�ض�����ƽ�����������ϱ�׼��9����������3����

        ���ڸ����Ĵ�ֱƽ����ȡ�е㣬������������
            �����������ڽ���ˮƽ�����ϵĻҶ�ֱ��ͼͳ�ƣ�ͬ���ǽض���ƽ��
            ��������λ�ߵĽ���ֱ�ʼ��������չ������25������������;�еĵ����뿼��֮�С�
            ���ڵõ�����ͨ���������ݴ�ֱ��ˮƽ�ı������пհײ�����
    */
    void compute();

    void getBinaryImg();
    vector<int> computeVerticalGrayHist();
    vector<int> computeHorizontalGrayHist(int yl,int yh);//histogram in the range
    vector<int> getVerticallines();//get vertical center lines
    vector<int> findPoint(int x,int y,vector<bool>& isVisited);

//���
    void clear()
    {
        a4paper.clear();
        bipaper.clear();
        anspaper.clear();
        cachepaper.clear();
    }
};

#endif // NUMBEREXTRACT_H
