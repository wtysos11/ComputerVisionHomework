#ifndef HOUGH
#define HOUGH

#include "CImg.h"
#include <string>
#include <vector>
#include <utility>
#include <iostream>

using namespace cimg_library;
using namespace std;
#define KP 13 //K max point
#define VERTEXMAX 64//如果点的欧式距离的平方小于这个值，则视为一个点

#define HEIGHTDIFF 50
#define PI 3.1415926
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

struct Vertex
{
    int tx,ty;//total
    int num;//influence number
    int x,y;//output value
    int weight;
    Vertex(int xx,int yy)
    {
        x = xx,y = yy;
        tx = x,ty = y;

        num = 1;
        weight = 1;
    }
    void addPoint(int xx,int yy)
    {
        tx += xx;
        ty += yy;
        num ++;
        x = tx/num;
        y = ty/num;
        weight++;
    }
    bool checkClosePoint(int xx,int yy)
    {
        return pow(xx-x,2)+pow(yy-y,2) <= VERTEXMAX;
    }

};

class Hough{
private:
    CImg<eleType> edge;
    CImg<eleType> ans;
    CImg<eleType> hough_space;
public:
    Hough();
    //init CImg edge
    Hough(CImg<eleType> canny);
    //compute entrance
    vector<Vertex> compute();
    //make hough space
    void houghSpaceMapping();
    //vote for 13 lines, lines vote for 4 interchange points
    vector<Vertex> find4InterchangePoints();
    //find 4 points in correct order.
    vector<Vertex> getA4Points(vector<Vertex>& vertexs);


};


#endif // HOUGH
