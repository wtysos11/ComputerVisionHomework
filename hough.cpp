#include "hough.h"

Hough::Hough(string str)
{
    CImg<eleType> source = CImg<eleTYpe>(str);
}


void Hough::edge_detect(void)
{
    //using sobel to detect edge
    double maxDistance = sqrt(source.width()*source.width()+source.height()*source.height());

    //turn to gray
    CImg<eleType> grayImg(source);
    cimg_forXY(grayImg, x, y) {
        double R = grayImg(x, y, 0, 0);
        double G = grayImg(x, y, 0, 1);
        double B = grayImg(x, y, 0, 2);
        double Gray = (R * 299 + G * 587 + B * 114 + 500) / 1000;
        grayImg(x, y, 0, 0) = Gray;
        grayImg(x, y, 0, 1) = Gray;
        grayImg(x, y, 0, 2) = Gray;
    }
    grayImg.blur(2);

    edge = CImg<eleType>(source.width(),source.height(),1,3,0);
    hough_space = CImg<eleType>(360,maxDistance,1,1,0);
    CImg_3x3(I,eleType);
    cimg_for3x3(grayImg,x,y,0,0,I,eleType)
    {
        //double gx = sobelX[0][0]*Ipp + sobelX[1][0] * Ipc + sobelX[2][0] * Ipn + sobelX[0][2]*Inp + sobelX[1][2] * Inc + sobelX[2][2] * Inn;
        //double gy = sobelY[0][0]*Ipp + sobelY[1][0] * Ipc + sobelY[2][0] * Ipn + sobelY[0][2]*Inp + sobelY[1][2] * Inc + sobelY[2][2] * Inn;
        const double gx = Inc - Ipc;
        const double gy = Icp - Icn;
        double grad = sqrt(gx*gx+gy*gy);
        if(grad>GRADLIMIT)
        {
            //edge(x,y) = grad;
            edge(x,y,0) = 255;
            edge(x,y,1) = 255;
            edge(x,y,2) = 255;
            //change hough space
            cimg_forX(hough_space,angle)
            {
                double ra = (double)angle*PI/180.0;
                int p = (int)(x*cos(ra)+y*sin(ra));
                if( p >= 0 && p <= hough_space.height() )
                {
                    if(hough_space(angle,p)<255)
                        hough_space(angle,p) += 1;
                }
            }
        }
    }
    edge.display();
}

void Hough::find_circle(void)
{
// r from minR to max R, theta from 0 to 360, using hough transform to find circle center using vote method.
// relate r with the most voting number , seeing voting number as the relativity to the existing circle.
// sort the stored_vector by voting number. The top N radius is the most likely circle.
// using the top N circles radius to find center.
}
