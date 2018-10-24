#include "hough.h"
#include <cmath>
#include <iostream>
using namespace std;

Hough::Hough()
{

}

Hough::Hough(string s)
{
    source = CImg<eleType>(s.c_str());
    ans = CImg<eleType>(source.width(),source.height(),1,3);
    filename = s;
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

    CImg<eleType> grayImg(source.width(),source.height());
    cimg_forXY(grayImg, x, y) {
        double r = source(x, y, 0, 0);
        double g = source(x, y, 0, 1);
        double b = source(x, y, 0, 2);
        grayImg(x, y) = (r * 299 + g * 587 + b * 114 + 500) / 1000;
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
            ans(x,y,0) = 255;
            ans(x,y,1) = 255;
            ans(x,y,2) = 255;
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
    string ansFile = "edge_"+filename;
    ans.save(ansFile.c_str());
    //hough_space.display();
    cout<<"edge detect over"<<endl;
    edge.display();
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
        Point point(area.getArea(i));
        int x = point.x;
        int y = point.y;
        int v = point.value;

        //insert them into the vector
            coordinate.push_back(point);
            int pointer = coordinate.size()-2;
            if(pointer<0)
                continue;
            while(coordinate[pointer].value<coordinate[pointer+1].value)
            {
                swap(coordinate[pointer],coordinate[pointer+1]);
                pointer--;
                if(pointer<0)
                {
                    break;
                }
            }

    }
    cout<<"area clusterring over"<<endl;
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
        //legal check, if the line has two intersection with the image boarder, then it pass the check.
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
            ans.draw_line(x0,ymin,x1,ymax,blue);
        }
        else{
            ans.draw_line(xmin,y0,xmax,y1,blue);
        }
        edgeCounting ++;
        if(edgeCounting == EDG_NUM)
            break;
    }
    string I2name = "I2_"+filename;
    ans.save(I2name.c_str());
    cout<<"draw line over"<<endl;
    //intersection point of two lines
    cout<<"intersections"<<endl;
    vector<pair<double,double> > intersections;
    for(unsigned int i = 0 ;i<lines.size();i++)
    {
        int x0 = 0;
        int x1 = 0;
        int y0 = 0;
        int y1 = 0;
        for(unsigned int j = 0 ;j<lines.size();j++)
        {
            if(i == j)
                continue;
            double m0 = lines[i].m;
            double m1 = lines[j].m;
            double b0 = lines[i].b;
            double b1 = lines[j].b;

            double x = -1.0*(b1-b0)/(m1-m0);
            double y = m0*x+b0;
            if(x>=0 && x<source.width() && y>=0 && y<source.height())
            {
                if(x0==0&y0==0)
                {
                    x0 = x;
                    y0 = y;
                }
                else
                {
                    x1 = x;
                    y1 = y;
                }
                bool isRepeat = false;
                for(unsigned int k = 0;k<intersections.size();k++)
                {
                    if(abs((int)(intersections[k].first - x)) < 10.0 && abs((int)intersections[k].second-y)<10.0)
                    {
                        isRepeat = true;
                        break;
                    }
                }
                if(isRepeat)
                    continue;
                cout<<x<<" "<<y<<endl<<endl;

                intersections.push_back(make_pair(x,y));
            }
        }
        const double red[]={255,0,0};
        if(x1!=0 && y1!=0)
            ans.draw_line(x0,y0,x1,y1,red);
    }
    string I3name = "I3_"+filename;
    ans.save(I3name.c_str());

    for(unsigned int i = 0;i<intersections.size();i++)
    {
        int x = intersections[i].first;
        int y = intersections[i].second;
        const double color[]={255,0,255};
        ans.draw_circle(x,y,5,color);
    }
    string I4name = "I4_"+filename;
    ans.save(I4name.c_str());
    cout<<"total over"<<endl;
}


void Hough::clear(void)
{
    source.clear();
    edge.clear();
    ans.clear();
    hough_space.clear();
}
