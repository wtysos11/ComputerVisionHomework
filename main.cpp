#include <iostream>
#include <string>
#include <exception>
#include "code0plus.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include "hough.h"
using namespace std;

#define VERBOSE 0
#define BOOSTBLURFACTOR 90.0

int main(int argc,char** argv)
{
   string infilename="test6.bmp";  /* Name of the input image */
   string dirfilename=""; /* Name of the output gradient direction image */
   string outfilename;    /* Name of the output "edge" image */
   string composedfname;  /* Name of the output "direction" image */
 //  unsigned char *image;     /* The input image */
 //  unsigned char *edge;      /* The output edge image */
   int rows, cols;           /* The dimensions of the image. */
   float sigma = 2.0,              /* Standard deviation of the gaussian kernel. */
	 tlow = 0.5,               /* Fraction of the high threshold in hysteresis. */
	 thigh = 0.9;              /* High hysteresis threshold control. The actual
			        threshold is the (100 * thigh) percentage point
			        in the histogram of the magnitude of the
			        gradient image that passes non-maximal
			        suppression. */

    Hough hough(infilename);
    hough.gaussian_smooth();
    hough.edge_detect();
    //hough.load_edge();
    hough.find_point();

    return 0;
}
