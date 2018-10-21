#include "hough.h"
#include <cmath>
#include <iostream>
using namespace std;
// Polar coordinate intersection at x
const int CrossX(int theta, int distance, int x) {
    double angle = (double)theta*PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)distance / sin(angle);
    return m*x + b;
}

// Polar coordinate intersection at y
const int CrossY(int theta, int distance, int y) {
    double angle = (double)theta*PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)distance / sin(angle);
    return ((double)(y - b) / m);
}
double calculatedistance(double x, double y) {
    return sqrt(x*x + y*y);
}
Hough::Hough()
{

}

Hough::Hough(string s)
{
    source = CImg<eleType>(s.c_str());
    cout<<"width:"<<source.width()<<endl;
    cout<<"height:"<<source.height()<<endl;

}
void Hough::gaussian_smooth(void)
{
    source.blur(2.0);
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

    edge = CImg<eleType>(source.width(),source.height());
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
            edge(x,y) = grad;
            /*
            edge(x,y,0) = 255;
            edge(x,y,1) = 255;
            edge(x,y,2) = 255;*/
            //change hough space
            cimg_forX(hough_space,angle)
            {
                double ra = (double)angle*PI/180.0;
                int p = (int)(x*cos(ra)+y*sin(ra));
                if( p >= 0 && p <= hough_space.height() )
                {
                    //if(hough_space(angle,p)<255)
                        hough_space(angle,p) += 1;
                }
            }
        }
    }
    edge.display();
    hough_space.display();
}

void Hough::load_edge(void)
{
    edge = CImg<eleType>("edge.bmp");
    hough_space = CImg<eleType>("hough.bmp");
}

void Hough::find_point(void)
{
    Area area;
    vector<Point> coordinate = vector<Point>();
    vector<int> numbers = vector<int>();
    vector<Line> lines = vector<Line>();

    cimg_forXY(hough_space,angle,p)
    {
        if(hough_space(angle,p)>=THRESHOLD)
        {
            area.addPoint(Point(angle,p,hough_space(angle,p)));
        }
    }
    cout<<area.getSize()<<endl;
    for(unsigned int i = 0;i<area.getSize();i++)
    {
        vector<Point> point(area.getArea(i));
        int x = 0,y = 0,v = 0;
        for(unsigned int j = 0 ;j < point.size();j++)
        {
            x+=point[j].x;
            y+=point[j].y;
            v+=point[j].value;
        }
            coordinate.push_back(Point((int)x/point.size(),(int)y/point.size(),(int)v/point.size()));
            numbers.push_back(point.size());
            int pointer = coordinate.size()-2;
            if(pointer<0)
                continue;
            while(numbers[pointer]<numbers[pointer+1])
            {
                swap(numbers[pointer],numbers[pointer+1]);
                swap(coordinate[pointer],coordinate[pointer+1]);
                pointer--;
                if(pointer<0)
                {
                    break;
                }
            }

    }

    CImg<eleType> result(source);
    //draw line
    int edgeCounting = 0;
    for(unsigned int i = 0 ;i <coordinate.size();i++)
    {
        //output calculate
        int angle = coordinate[i].x;
        int polar = coordinate[i].y;
        double ra = (double)angle*PI/180.0;//real angle
        double m = -cos(ra)/sin(ra);
        double b = (double) polar/sin(ra);

        //draw line
        const int ymin = 0;
        const int ymax = source.height()-1;
        const int xmin = 0;
        const int xmax = source.width()-1;

        int counting = 0;
        const int x0 = (double)(ymin-b)/m;
        const int x1 = (double)(ymax-b)/m;
        const int y0 = (double)(xmin*m+b);
        const int y1 = (double)(xmax*m+b);
        //cout<<x0<<" "<<x1<<" "<<y0<<" "<<y1<<endl;
        //legal check
        if(x0>=0 && x0<source.width())
            counting++;
        if(x1>=0 && x1<source.width())
            counting++;
        if(y0>=0 && y0<source.height())
            counting++;
        if(y1>=0 && y1<source.height())
            counting++;

        if(counting!=2)
            continue;

        cout<<"line "<<i<<" measured by point ("<<angle<<","<<polar<<") with times "<<coordinate[i].value<<endl;
        cout<<"y = "<<m<<"x+"<<b<<endl;
        lines.push_back(Line(m,b));
        const double blue[] = {0,0,255};

        if(abs(m)>1)
        {
            result.draw_line(x0,ymin,x1,ymax,blue);
        }
        else{
            result.draw_line(xmin,y0,xmax,y1,blue);
        }
        edgeCounting ++;
        if(edgeCounting == EDG_NUM)
            break;

    }

    //intersection point of two lines
    cout<<"intersection"<<endl;
    for(unsigned int i = 0 ;i<lines.size();i++)
    {
        for(unsigned int j = i+1 ;j<lines.size();j++)
        {
            double m0 = lines[i].m;
            double m1 = lines[j].m;
            double b0 = lines[i].b;
            double b1 = lines[j].b;

            double x = -1.0*(b1-b0)/(m1-m0);
            double y = m0*x+b0;
            if(x>=0 && x<result.width() && y>=0 && y<result.height())
            {
                cout<<x<<" "<<y<<endl;
                const double red[]={255,0,0};
                result.draw_circle(x,y,5,red);
            }
        }
    }
    result.save("ans.bmp");
}

