#ifndef MEANSHIFT
#define MEANSHIFT

#include "CImg.h"
#include "hough.h"
#include <vector>
#include <string>
#include <iostream>
#include <utility>
using namespace std;
using namespace cimg_library;
/*
    遍历图像的所有点
    对于每一个点
        从这一点开始展开窗口
        搜索x-h ~ x+h,y-h~y+h中颜色距离颜色中心欧氏距离小于一定要求的点
        将点记录下来。
            如果没有入队，则压入队列中
        计算颜色中心与空间中心

        判断是否找到终止点（空间、颜色一个满足要求，即停止）

        没有则重复。
        有则停止，并将所有队列中的点重新赋值。
*/

class meanShift
{
private:
    CImg<int> source;
public:
    meanShift(string filename)
    {
        source = CImg<int>(filename.c_str());
    }

    void compute(void)
    {
        int width = source.width(),height = source.height();
        const int h = 30;//窗口
        const double dist = 100;//颜色空间

        const double maxSpaceDist = 3;
        const double colorSpaceDist = 5;
        vector<bool> isVisited(width*height,false);
        vector<pair<int,int>> ready;
        const int maxIteration = 5;

        cimg_forXY(source,x,y)
        {
            if(isVisited[y*width+x])
                continue;

            bool over = false;
            int centerX = x,centerY = y;
            double centerR = source(x,y,0),centerG = source(x,y,1),centerB = source(x,y,2);
            ready.push_back(make_pair(x,y));
            int counting = 0;

            while(!over)
            {
                int xmin = centerX-h>=0?centerX-h:0;
                int xmax = centerX+h<width?centerX+h:width-1;
                int ymin = centerY-h>=0?centerY-h:0;
                int ymax = centerY+h<height?centerY+h:height-1;
                vector<double> space(2,0);
                vector<double> color(3,0);
                int number = 0;

                for(int i = xmin;i<=xmax;i++)
                {
                    for(int j = ymin;j<=ymax;j++)
                    {
                        if(isVisited[j*width+i])
                            continue;

                        if(pow(source(i,j,0)-centerR,2)+pow(source(i,j,1)-centerG,2)+pow(source(i,j,2)-centerB,2) < pow(dist,2))
                        {
                            number++;
                            space[0] += i;
                            space[1] += j;
                            color[0] += source(i,j,0);
                            color[1] += source(i,j,1);
                            color[2] += source(i,j,2);
                            isVisited[j*width+i] = true;
                            ready.push_back(make_pair(i,j));
                        }
                    }
                }
                if(number==0)
                {
                    over=true;
                    break;
                }
                space[0] /= number;
                space[1] /= number;
                color[0] /= number;
                color[1] /= number;
                color[2] /= number;
                //判断终止
                if( (fabs(space[0]-centerX) < maxSpaceDist && fabs(space[1]-centerY)<maxSpaceDist) || (pow(color[0]-centerR,2)+pow(color[1]-centerG,2)+pow(color[2]-centerB,2) < pow(colorSpaceDist,2)))
                {
                    over = true;
                    break;
                }

                centerX = space[0];
                centerY = space[1];
                centerR = color[0];
                centerG = color[1];
                centerB = color[2];
                counting ++;
                if(counting>5)
                {
                    over=true;
                    break;
                }
            }
            for(int i = 0;i<ready.size();i++)
            {
                source(ready[i].first,ready[i].second,0) = centerR;
                source(ready[i].first,ready[i].second,1) = centerG;
                source(ready[i].first,ready[i].second,2) = centerB;
            }
            ready.clear();
        }
        //差值
        int number = 0;
        CImg<int> edge(width,height,1,3,0);
        const double controlMax = 80;
        for(int x = 1; x<width-1;x++)
        {
            for(int y = 1;y<height-1;y++)
            {
                if(source(x,y,0) == source(x-1,y,0) && source(x,y,0) == source(x+1,y,0) && source(x,y,0) == source(x,y-1,0) && source(x,y,0) == source(x,y+1,0))
                    continue;
                double dist1 = pow(source(x,y,0)-source(x-1,y,0),2)+pow(source(x,y,1)-source(x-1,y,1),2)+pow(source(x,y,1)-source(x-1,y,1),2);
                double dist2 = pow(source(x,y,0)-source(x+1,y,0),2)+pow(source(x,y,1)-source(x+1,y,1),2)+pow(source(x,y,1)-source(x+1,y,1),2);
                double dist3 = pow(source(x,y,0)-source(x,y-1,0),2)+pow(source(x,y,1)-source(x,y-1,1),2)+pow(source(x,y,1)-source(x,y-1,1),2);
                double dist4 = pow(source(x,y,0)-source(x,y+1,0),2)+pow(source(x,y,1)-source(x,y+1,1),2)+pow(source(x,y,1)-source(x,y+1,1),2);
                if(dist1>pow(controlMax,2) || dist2>pow(controlMax,2) || dist3>pow(controlMax,2) || dist4>pow(controlMax,2))
                {
                    edge(x,y,0) = edge(x,y,1) = edge(x,y,2) = 255;
                    number++;
                }
            }
        }
        cout<<number<<endl;

        Hough hough(edge,edge);
        hough.find_point();
    }
};


#endif // MEANSHIFT
