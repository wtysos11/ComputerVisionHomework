#include "CImg.h"
#include "Canny.h"
#include "Process.h"
#include "Solution.h"
#include <string>
#include <iostream>

using namespace std;
using namespace cimg_library;

int main(void)
{
    Solution s("test.bmp");
    s.mainProcess();
    s.clear();
    return 0;
}
