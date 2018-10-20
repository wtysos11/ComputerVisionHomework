#include "hough.h"
#include <algorithm>
#include <iostream>
using namespace std;

Hough::Hough(string str)
{
    source = CImg<eleType>(str.c_str());
    ans = CImg<eleType>(source);
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
    //gaussian_smooth
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
    //edge.display();
}

void Hough::find_circle(void)
{
    vector<pair<int,int> > voting_rank;

// r from minR to max R, theta from 0 to 360, using hough transform to find circle center using vote method.
    for(int r = minR;r < maxR; r += 5)
    {
        CImg<eleType> hough_space(source.width(),source.height());
        cimg_forXY(edge,x,y)
        {
            if(edge(x,y)>0)//´æÔÚ
            {
                for(int theta = 0;theta<360;theta++)
                {
                    //center
                    double rangle = (double) theta*PI/180.0;
                    int x0 = x - r * cos(rangle);
                    int y0 = y - r * sin(rangle);
                    //legal check and voting
                    if(x0 >= 0 && y0 >= 0 && x0 < source.width() && y0 < source.height())
                    {
                        hough_space(x0,y0)++;
                    }
                }
            }
        }

        // relate r with the most voting number , seeing voting number as the relativity to the existing circle.
        int maximum = 0;
        cimg_forXY(hough_space,x,y)
        {
            if(hough_space(x,y)>maximum)
            {
                maximum = hough_space(x,y);
            }
        }
        voting_rank.push_back(make_pair(r,maximum));
    }
// sort the stored_vector by voting number. The top N radius is the most likely circle.
    sort(voting_rank.begin(),voting_rank.end(),[](const pair<int,int>& p1,const pair<int,int>& p2)
         {
            return p1.second > p2.second;
         });

// using the top N circles radius to find center.

    vector<pair<int,int> > center;//store circle center

    for(unsigned int i = 0;i < CIRCLE_NUM;i++)
    {
        //voting center using specific radius
        CImg<eleType> hough_space(source.width(),source.height());
        int radius = voting_rank[i].first;
        cimg_forXY(edge,x,y)
        {
            if(edge(x,y)>0)//´æÔÚ
            {
                for(int theta = 0;theta<360;theta++)
                {
                    //center
                    double rangle = (double) theta*PI/180.0;
                    int x0 = x - radius * cos(rangle);
                    int y0 = y - radius * sin(rangle);
                    //legal check and voting
                    if(x0 >= 0 && y0 >= 0 && x0 < source.width() && y0 < source.height())
                    {
                        hough_space(x0,y0)++;
                    }
                }
            }
        }
        //draw center
        cout<<"Now radius is"<<radius<<endl;

        //hough_space.display();//it can show that the brighest is always the center of one circle.
        vector<Point> circleWeight;

        //search for all points that are not zero, put them into the array, and sort them.
        cimg_forXY(hough_space,x,y)
        {
            if(hough_space(x,y)>0)
            {
                circleWeight.push_back(Point(x,y,hough_space(x,y)));
            }
        }
        sort(circleWeight.begin(),circleWeight.end(),greater<Point>());
        // look for circle center, only one.
        for(unsigned int i = 0;i<circleWeight.size();i++)
        {
            //check whether center (x,y) is available.
            bool available = true;
            for(unsigned int j = 0 ;j < center.size();j++)
            {
                if(pow(circleWeight[i].x-center[j].first,2)+pow(circleWeight[i].y-center[j].second,2)<DIFF)
                {
                    available = false;
                    break;
                }
            }
            if(!available)
            {
                continue;
            }

            const double color[]={0,0,255};
            center.push_back(make_pair(circleWeight[i].x,circleWeight[i].y));
            ans.draw_circle(circleWeight[i].x,circleWeight[i].y,radius,color);
            cout<<"answer is "<<circleWeight[i].x<<" "<<circleWeight[i].y<<endl;
            break;
        }
    }
    ans.display();
}
