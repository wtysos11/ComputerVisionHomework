#include "CImg.h"
//#include "otsu.h"
#include "meanShift.h"
#include <iostream>
using namespace cimg_library;
using namespace std;


int main(void)
{
    /*
    otsu o("test11.bmp");
    int threshold = o.compute();
    cout<<threshold<<endl;
    o.outputBio(threshold);
*/

    meanShift m("test5.bmp");
    m.compute();


    return 0;
}
