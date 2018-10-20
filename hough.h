#ifndef HOUGH
#define HOUGH

#include "CImg.h"
#include "area.h"
#include <string>
using namespace cimg_library;
using namespace std;
#define GRAD_LIMIT 60
#define GRADLIMIT 20
//hough transform
#define THRESHOLD 255
#define DIFF 300
#define PI 3.1415926
#define SLOPE_FLAG 1
typedef double eleType;//CImg element type





class Hough{
private:
    //����Ϊ��ԭͼת������
    CImg<eleType> source;
    CImg<eleType> edge;
    CImg<eleType> ans;
    //����Ϊ������������
    CImg<eleType> hough_space;

public:
    Hough();
    Hough(string s);
    void gaussian_smooth(void);
    void edge_detect(void);
    void load_edge();//for debug loading edge from edge.bmp. Must run edge_detect first.
    void find_point();
};


#endif // HOUGH
