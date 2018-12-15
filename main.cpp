#include "CImg.h"
#include "otsu.h"
#include <iostream>
using namespace cimg_library;
using namespace std;


int main(void)
{
    otsu o("test1.bmp");
    int threshold = o.compute();
    cout<<threshold<<endl;
    o.outputBio(threshold);

    return 0;
}
