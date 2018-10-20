#ifndef AREA
#define AREA
#include <cmath>
#include <vector>
using namespace std;
struct Point{
    int x;
    int y;
    int value;
    Point(int _x,int _y,int _mag):x(_x),y(_y),value(_mag){}
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
    vector<vector<Point>> cache;
    vector<int> num;
    const int deltax = 8;//pointx - max/minx = 5
    const int deltay = 250;// pointy - min/maxy = 200
public:
    Area()
    {
        cache.clear();
    }
    double distance(Point a,Point b)
    {
        return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
    }
    void addPoint(Point p)
    {
        //find if this point fit the area in cache.
        for(unsigned int i = 0; i < cache.size() ; i++ )
        {
            if(abs(cache[i][0].x-p.x)<this->deltax && abs(cache[i][0].y-p.y)<this->deltay)
            {
                cache[i].push_back(p);
                num[i]++;
                return;
            }
        }
        vector<Point> another = vector<Point>();
        another.push_back(p);
        cache.push_back(another);
        num.push_back(1);
    }
    int getSize()
    {
        return cache.size();
    }
    vector<Point> getArea(int index)
    {
        return cache[index];
    }
    int getNum(int index)
    {
        return num[index];
    }

};

#endif // AREA
