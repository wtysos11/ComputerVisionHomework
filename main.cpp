#include "CImg.h"
#include <iostream>
#include "hough.h"
using namespace std;
using namespace cimg_library;

int main(void)
{
    string infilename = "test2.bmp";

    Hough hough(infilename);
    hough.edge_detect();
    hough.find_circle();
    return 0;
}
