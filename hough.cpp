#include "hough.h"
#include <cmath>
#include <iostream>
using namespace std;

void drawLines(CImg<int> &im,double m,double b)
{
    const int ymin = 0;
    const int ymax = im.height()-1;
    const int xmin = 0;
    const int xmax = im.width()-1;

    int counting = 0;
    const int x0 = (double)(ymin-b)/m;
    const int x1 = (double)(ymax-b)/m;
    const int y0 = (double)(xmin*m+b);
    const int y1 = (double)(xmax*m+b);
    const double blue[] = {0,0,255};
    if(abs(m)>1)
        im.draw_line(x0,ymin,x1,ymax,blue);
    else
        im.draw_line(xmin,y0,xmax,y1,blue);
}

Point getIntesection(Line l1,Line l2)
{
    double x = -1 * (l1.b-l2.b)/(l1.m-l2.m);
    double y = l1.m*x+l1.b;
    return Point{x,y,0};
}

Hough::Hough()
{

}

Hough::Hough(string s)
{
    source = CImg<eleType>(s.c_str());
    ans = CImg<eleType>(source.width(),source.height(),1,3);
    filename = s;
}


vector<Point> Hough::find_point(void)
{
    cout<<"hough point"<<endl;
    double maxDistance = (edge.width() + edge.height()) * 2;
    hough_space = CImg<double>(180,maxDistance,1,1,0);
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
                if( polar+0.5*maxDistance >= 0 && polar < 0.5*maxDistance )
                {
                    hough_space(angle,polar+0.5*maxDistance) += 1;
                }
            }
        }
    }

    Area area;
    vector<Point> coordinate = vector<Point>();
    vector<int> numbers = vector<int>();

    //将原空间划分为若干个区域，每个区域只取一个最大值点
    int angle_step = 10;
    int rho_step = 200;
    for(int angle = 0;angle<180;angle+=angle_step)
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
    cout<<"area size"<<area.getSize()<<endl;

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
    //only allow two parallel lines.
    int edgeCounting = 0;
    cout<<"coordinate size"<<coordinate.size()<<endl;
    vector<Line> lines1;
    vector<Line> lines2;
    for(unsigned int i = 0 ;i <coordinate.size();i++)
    {
        //output calculate
        int angle = coordinate[i].x;
        int polar = coordinate[i].y - 0.5*maxDistance;
        cout<<"coordinate"<<i<<" "<<angle<<" "<<polar<<" "<<coordinate[i].value<<endl;
        double ra = (double)angle*PI/180.0;//real angle
        if(sin(ra)==0)
            ra = 0.01;

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
        cout<<"\n";
        cout<<atan(-1/m)<<endl;
        cout<<"line "<<i<<" measured by point ("<<angle<<","<<polar<<") with times "<<coordinate[i].value<<endl;
        cout<<"y = "<<m<<"x+"<<b<<endl;

        bool passParallelCheck = false;
        //平行线检查
        //检查是否属于第一类直线
        double delta = 0.2;

        if(lines1.size()==0)
        {
            lines1.push_back(Line{m,b});
            cout<<"In line1"<<endl;
            edgeCounting ++;
        }
        else if(fabs(atan(-1/lines1[0].m)-atan(-1/m)) < delta || fabs(atan(-1/lines1[0].m)-atan(-1/m) + 3.14) < delta || fabs(atan(-1/lines1[0].m)-atan(-1/m) - 3.14) < delta)
        {
            if(lines1.size()==2)
            {
                continue;
            }
            else if(fabs(lines1[0].m-m)<10 && fabs(b-lines1[0].b)/sqrt(m*m+1) < 700)//平行判定
            {
                continue;
            }
            else
            {
                int ymin = m*0+b;
                int y1min = lines1[0].m * 0 +lines1[0].b;
                if(fabs(y1min-ymin)<500)
                    continue;
                int xmin = -1*b/m;
                int x1min = -1* lines1[0].b / lines1[0].m;
                if(fabs(xmin-x1min)<500)
                    continue;
            }

            lines1.push_back(Line{m,b});
            cout<<"In line1"<<endl;
            edgeCounting ++;
        }
        else if(lines2.size()==0)
        {
            lines2.push_back(Line{m,b});
            cout<<"In line2"<<endl;
            edgeCounting ++;
        }
        else if(fabs(atan(-1/lines2[0].m)-atan(-1/m)) < delta || fabs(atan(-1/lines2[0].m)-atan(-1/m) + 3.14) < delta || fabs(atan(-1/lines2[0].m)-atan(-1/m) - 3.14) < delta)
        {
            if(lines2.size()==2)
            {
                continue;
            }
            else if(fabs(lines2[0].m-m)<10 && fabs(b-lines2[0].b)/sqrt(m*m+1) < 700)
            {
                continue;
            }
            else
            {
                int ymin = m*0+b;
                int y1min = lines2[0].m * 0 +lines2[0].b;
                if(fabs(y1min-ymin)<500)
                    continue;
                int xmin = -1*b/m;
                int x1min = -1* lines2[0].b / lines2[0].m;
                if(fabs(xmin-x1min)<500)
                    continue;
            }

            lines2.push_back(Line{m,b});
            cout<<"In line2"<<endl;
            edgeCounting ++;
        }
        else
        {
            if(edgeCounting == EDG_NUM)
                break;
            continue;
        }

        /*
        const double blue[] = {0,0,255};

        if(abs(m)>1)
        {
            ans.draw_line(x0,ymin,x1,ymax,blue);
        }
        else{
            ans.draw_line(xmin,y0,xmax,y1,blue);
        }
*/


    }

    int l1_index1 = 0;
    int l1_index2 = 1;
    int l2_index1 = 0;
    int l2_index2 = 1;
    //according to back number
    cout<<"y = "<<lines1[l1_index1].m<<"x+"<<lines1[l1_index1].b<<endl;
    cout<<"y = "<<lines1[l1_index2].m<<"x+"<<lines1[l1_index2].b<<endl;
    cout<<"y = "<<lines2[l2_index1].m<<"x+"<<lines2[l2_index1].b<<endl;
    cout<<"y = "<<lines2[l2_index2].m<<"x+"<<lines2[l2_index2].b<<endl;
    drawLines(ans,lines1[l1_index1].m,lines1[l1_index1].b);
    drawLines(ans,lines1[l1_index2].m,lines1[l1_index2].b);
    drawLines(ans,lines2[l2_index1].m,lines2[l2_index1].b);
    drawLines(ans,lines2[l2_index2].m,lines2[l2_index2].b);
    //ans.display();
    ans.save("ans.bmp");
    cout<<"draw line over"<<endl;
    //intersection point of two lines
    vector<Point> intersection;
    intersection.push_back(getIntesection(lines1[l1_index1],lines2[l1_index1]));
    intersection.push_back(getIntesection(lines1[l1_index1],lines2[l1_index2]));
    intersection.push_back(getIntesection(lines1[l1_index2],lines2[l1_index2]));
    intersection.push_back(getIntesection(lines1[l1_index2],lines2[l1_index1]));
    return intersection;
}


void Hough::clear(void)
{
    source.clear();
    edge.clear();
    ans.clear();
    hough_space.clear();
}
