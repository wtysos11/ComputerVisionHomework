#ifndef HOUGH
#define HOUGH

#include "CImg.h"
#include "area.h"
#include <string>
using namespace cimg_library;
using namespace std;
#define GRAD_LIMIT 60
#define GRADLIMIT 26
#define EDG_NUM 4 //number of edge you want
//hough transform
#define THRESHOLD 150
#define DIFF 300
#define PI 3.1415926
#define SLOPE_FLAG 1
typedef int eleType;//CImg element type

class Hough{
private:
    //以下为从原图转换来的
    CImg<eleType> source;
    CImg<eleType> edge;
    CImg<eleType> ans;
    //以下为辅助计算内容
    CImg<eleType> hough_space;
    string filename;
public:
    Hough();
    Hough(string s);
    Hough(CImg<eleType> source,CImg<eleType> edge2)
    {
        ans = source;
        edge = edge2;
    }
    void find_point();
    void clear(void);
};


#endif // HOUGH
