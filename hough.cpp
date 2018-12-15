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
}


void Hough::find_point(void)
{
    double maxDistance = sqrt(edge.width()*edge.width()+edge.height()*edge.height());
    hough_space = CImg<double>(360,maxDistance,1,1,0);
    vector<double> cosCache(360);
    vector<double> sinCache(360);

    for(int i = 0;i<360;i++)
    {
        double ra = (double)i*PI/180.0;
        sinCache[i] = sin(ra);
        cosCache[i] = cos(ra);
    }
    cimg_forXY(edge,x,y)
    {
        if(edge(x,y,0)==0)
        {
            cimg_forX(hough_space,angle)
            {
                int polar = (int)(x*cosCache[angle]+y*sinCache[angle]);
                if( polar >= 0 && polar <= hough_space.height() )
                {
                    hough_space(angle,polar) += 1;
                }
            }
        }
    }
    hough_space.display();

    Area area;
    vector<Point> coordinate = vector<Point>();
    vector<int> numbers = vector<int>();
    vector<Line> lines = vector<Line>();

    //将原空间划分为若干个区域，每个区域只取一个最大值点
    int angle_step = 10;
    int rho_step = 300;
    for(int angle = 0;angle<360;angle+=angle_step)
    {
        for(int polar = 0;polar<maxDistance;polar+=rho_step)
        {
            int maxX,maxY;
            double maxCount = 0;
            for(int x = 0;x<angle_step;x++)
            {
                for(int y = 0;y<rho_step;y++)
                {
                    int xx = angle+x;
                    int yy = polar+y;
                    if(xx>=360 || yy>=maxDistance)
                        continue;
                    if(hough_space(xx,yy)>maxCount)
                    {
                        maxCount = hough_space(xx,yy);
                        maxX = xx;
                        maxY = yy;
                    }
                }
            }
            if(maxCount>THRESHOLD)
            {
                area.addPoint(Point(maxX,maxY,hough_space(maxX,maxY)));
            }
        }
    }
    cout<<area.getSize()<<endl;

    for(unsigned int i = 0;i<area.getSize();i++)
    {
        Point point(area.getArea(i));

        //insert them into the vector
            coordinate.push_back(Point(point.x,point.y,point.value));
            numbers.push_back(point.value);
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
    cout<<"area clusterring over"<<endl;
    //draw line
    int edgeCounting = 0;
    cout<<"coordinate size"<<coordinate.size()<<endl;
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
        const int ymax = edge.height()-1;
        const int xmin = 0;
        const int xmax = edge.width()-1;

        int counting = 0;
        const int x0 = (double)(ymin-b)/m;
        const int x1 = (double)(ymax-b)/m;
        const int y0 = (double)(xmin*m+b);
        const int y1 = (double)(xmax*m+b);
        //cout<<x0<<" "<<x1<<" "<<y0<<" "<<y1<<endl;
        //legal check, the line must have two intersections with the boarders
        if(x0>=0 && x0<edge.width())
            counting++;
        if(x1>=0 && x1<edge.width())
            counting++;
        if(y0>=0 && y0<edge.height())
            counting++;
        if(y1>=0 && y1<edge.height())
            counting++;

        if(counting!=2)
            continue;
        int paNumber = 0;;
        for(int j = 0;j<lines.size();j++)
        {
            if(fabs(lines[j].m-m) < 0.1)
            {
                paNumber++;
            }
        }
        if(paNumber > 2)
        {
            continue;
        }

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
    cout<<"draw line over"<<endl;
    //intersection point of two lines
    /*
    cout<<"intersections"<<endl;
    vector<pair<double,double> > intersections;
    for(unsigned int i = 1 ;i<lines.size();i++)
    {
        int x0 = 0;
        int x1 = 0;
        int y0 = 0;
        int y1 = 0;
        for(unsigned int j = 0 ;j<=i-1;j++)
        {
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
        const double red[]={128,0,0};
        if(x1!=0 && y1!=0)
            ans.draw_line(x0,y0,x1,y1,red);
    }

    cimg_forXY(ans,x,y)
    {
        if(ans(x,y,0) == 128 && edge(x,y)>GRADLIMIT)
        {
            ans(x,y,0) = 255;
        }
        else if(ans(x,y,0) == 128)
        {
            ans(x,y,0) = 0;
            ans(x,y,2) = 255;
        }
    }


    for(unsigned int i = 0;i<intersections.size();i++)
    {
        int x = intersections[i].first;
        int y = intersections[i].second;
        const double color[]={255,0,255};
        ans.draw_circle(x,y,5,color);
    }*/


}


void Hough::clear(void)
{
    source.clear();
    edge.clear();
    ans.clear();
    hough_space.clear();
}
