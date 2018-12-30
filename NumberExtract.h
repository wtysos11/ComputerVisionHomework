#ifndef NUMBEREXTRACT_H
#define NUMBEREXTRACT_H

#include "CImg.h"
#include <vector>
#include <iostream>
#include <utility>
#include <string>
#define DEBUG
//二值化图像的分界点
#define BITHRESHOLD 150
//垂直灰度直方图的顶点阈值
#define VERTICAL_GRAY_THRESHOLD 10
#define MARGIN 0.05//A4提取误差边缘

#define VERTICAL_NUM 9//有9条垂直方向的中心线
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
    计算入口：
    分块进行二值化处理
    提取字符行
    对字符行内部进行分割
    将数字变为784维向量

    对于提取字符行的想法：
        对于垂直方向的灰度直方图截顶，求平均数（理论上标准有9个，测试有3个）

        对于给出的垂直平均线取中点，计算搜索区域
            在搜索区域内进行水平方向上的灰度直方图统计，同样是截顶求平均
            从两条中位线的交点分别开始向两边扩展，进行25邻域搜索，将途中的点纳入考量之中。
            对于得到的连通分量，根据垂直与水平的比例进行空白补正。
    */
    void compute();

    void getBinaryImg();
    vector<int> computeVerticalGrayHist();
    vector<int> computeHorizontalGrayHist(int yl,int yh);//histogram in the range
    vector<int> getVerticallines();//get vertical center lines
    vector<int> findPoint(int x,int y,vector<bool>& isVisited);

//清空
    void clear()
    {
        a4paper.clear();
        bipaper.clear();
        anspaper.clear();
        cachepaper.clear();
    }
};

#endif // NUMBEREXTRACT_H
