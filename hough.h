#ifndef HOUGH
#define HOUGH
#include "CImg.h"
#include <string>
#include <cmath>
using namespace std;
using namespace cimg_library;

#define CIRCLE_NUM 1
#define GRADLIMIT 20 // use in edge detection for grad limit
#define PI 3.1415926
#define DIFF 100//judge whether two points are the same
typedef double eleType;//CImg element type

struct Point
{
    int x;
    int y;
    double value;
    Point(int _x,int _y,double _v):x(_x),y(_y),value(_v){}
    bool operator< (const Point& rhs) const
    {
        return value < rhs.value;
    }
    bool operator> (const Point& rhs) const
    {
        return value > rhs.value;
    }
};

class Hough
{
private:
    CImg<eleType> source;
    CImg<eleType> edge;
    CImg<eleType> ans;

    CImg<eleType> hough_space;

    //some parameters use in finding circles
    const int minR = 50;
    const int maxR = 500;
public:
    Hough(string str);
    void edge_detect(void);
    void find_circle(void);
};

#endif // HOUGH
