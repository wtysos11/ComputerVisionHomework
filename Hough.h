#ifndef HOUGH
#define HOUGH

#include "CImg.h"
#include <string>
#include <vector>
#include <utility>
using namespace cimg_library;
using namespace std;

typedef int eleType;//CImg element type
/*
重写Hough变换：
大致思路：
1. 映射（与我基本相同）
2. 找13个最大的点
3. 对13个最大的点组成的线两两相交，统计交点（在合法区域内），并合并相近的两个交点
4. 找到最大的四个交点

结果会得到四个点与四条线

顶点输出策略：如果y坐标相差明显（一个很小，两个较大，一个很大），说明A4纸有斜放倾向；不然为正放。
首先找短边，找到4条直线中最短的两条。
    找到最上面的一个点所在的直线。
    找到该直线最左侧的点，与最右侧的点
    以此顺序寻找第三个点和第四个点。

如果是正放，则找到最上面的直线，寻找其最左侧的点和最右侧的点。
    然后从其左上开始输出
*/
class Hough{
private:
    CImg<eleType> edge;
    CImg<eleType> ans;
    CImg<eleType> hough_space;
    vector<pair<int,int>> lines;
    vector<pair<int,int>> points;//保存的是乱序的节点
public:
    Hough();
    //init CImg edge
    Hough(CImg<eleType> canny);
    //compute entrance
    vector<pair<int,int>> compute();
    //make hough space
    void houghSpaceMapping();
    //vote for 13 lines, lines vote for 4 interchange points
    void find4InterchangePoints();
    //find 4 points in correct order.
    vector<pair<int,int>> getA4Points();


};


#endif // HOUGH
