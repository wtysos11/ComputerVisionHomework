#ifndef HOUGH
#define HOUGH
#include "CImg.h"
#include <string>
#include <cmath>
using namespace std;
using namespace cimg_library;

#define CIRCLE_NUM 5
#define GRADLIMIT 20 // use in edge detection for grad limit
#define PI 3.1415926
typedef double eleType;//CImg element type

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
