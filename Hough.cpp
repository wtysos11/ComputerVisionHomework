
#include "Hough.h"
#include <math.h>

Hough::Hough()
{

}
//init CImg edge
Hough::Hough(CImg<eleType> canny)
{
	edge = CImg<int>(canny.width(),canny.height, 1, 1, 0);
	int maxDistance = (canny.width() + canny.height()) * 2;

	cimg_forXY(edge, x, y) {
		edge(x, y, 0, 0) = canny(x, y, 0, 0);
	}

	hough_space = CImg<int>(180,maxDistance,1,1,0);
}
//compute entrance
vector<pair<int,int>> Hough::compute()
{
    houghSpaceMapping();
    find4InterchangePoints();
    return getA4Points();
}
//make hough space
void Hough::houghSpaceMapping()
{
    vector<double> cosCache(180);
    vector<double> sinCache(180);

    for(int i = 0;i<180;i++)
    {
        double ra = (double)i*PI/180.0;
        sinCache[i] = sin(ra);
        cosCache[i] = cos(ra);
    }
    cimg_forXY(edge,x,y)
    {
        if(edge(x,y,0)==255)
        {
            cimg_forX(hough_space,angle)
            {
                int polar = (int)(x*cosCache[angle]+y*sinCache[angle]);
                if( polar >= 0 && polar < maxDistance )
                {
                    hough_space(angle,polar) ++;
                }
            }
        }
    }
}
//vote for 13 lines, lines vote for 4 interchange points
void Hough::find4InterchangePoints();
//find 4 points in correct order.
vector<pair<int,int>> Hough::getA4Points();
