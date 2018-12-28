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
class Hough{
private:
    CImg<eleType> edge;
    CImg<eleType> ans;
    CImg<eleType> hough_space;
    vector<pair<int,int>> lines;
    vector<pair<int,int>> points;//�����������Ľڵ�
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
