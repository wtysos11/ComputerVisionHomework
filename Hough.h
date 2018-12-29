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
#define VERTEXMAX 64//������ŷʽ�����ƽ��С�����ֵ������Ϊһ����

#define HEIGHTDIFF 50
#define PI 3.1415926
typedef int eleType;//CImg element type
/*
��дHough�任��
����˼·��
1. ӳ�䣨���һ�����ͬ��
2. ��13�����ĵ�
3. ��13�����ĵ���ɵ��������ཻ��ͳ�ƽ��㣨�ںϷ������ڣ������ϲ��������������
4. �ҵ������ĸ�����

�����õ��ĸ�����������

����������ԣ����y����������ԣ�һ����С�������ϴ�һ���ܴ󣩣�˵��A4ֽ��б�����򣻲�ȻΪ���š�
�����Ҷ̱ߣ��ҵ�4��ֱ������̵�������
    �ҵ��������һ�������ڵ�ֱ�ߡ�
    �ҵ���ֱ�������ĵ㣬�����Ҳ�ĵ�
    �Դ�˳��Ѱ�ҵ�������͵��ĸ��㡣

��������ţ����ҵ��������ֱ�ߣ�Ѱ���������ĵ�����Ҳ�ĵ㡣
    Ȼ��������Ͽ�ʼ���
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
