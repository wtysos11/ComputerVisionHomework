#ifndef AREA
#define AREA
#include <cmath>
#include <vector>
using namespace std;
struct Point{
    double x;
    double y;
    double value;
    Point(double _x,double _y,double _mag):x(_x),y(_y),value(_mag){}
    Point():x(0),y(0),value(0){}
    bool operator<(const Point& rhs) const{
        return value<rhs.value;
    }

};

struct Line{
    double m;
    double b;
    Line(double _m,double _b):m(_m),b(_b){};
};
// this class can collect a list of points and divide them into several parts, return the average of x and y
class Area
{
private:
    vector<Point> center_point;
    const int deltax = 8;//pointx - max/minx = 5
    const int deltay = 250;// pointy - min/maxy = 200
public:
    Area()
    {
        center_point.clear();
    }
    double distance(Point a,Point b)
    {
        return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
    }
    void addPoint(Point p)
    {
        //find if this point fit the area in cache.
        /*
        for(unsigned int i = 0; i < center_point.size() ; i++ )
        {
            if(abs(center_point[i].x-p.x)<this->deltax && abs(center_point[i].y-p.y)<this->deltay)
            {
                if(center_point[i].value<p.value)
                    center_point[i] = p;
                return;
            }
        }*/
        center_point.push_back(p);
    }
    int getSize()
    {
        return center_point.size();
    }
    Point getArea(int index)
    {
        return center_point[index];
    }

};

#endif // AREA
