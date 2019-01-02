
#include "Hough.h"
#include <math.h>
#include <algorithm>
#include <iostream>
using namespace std;

Hough::Hough()
{

}
//init CImg edge
Hough::Hough(CImg<eleType> canny)
{
	edge = CImg<int>(canny.width(),canny.height(), 1, 1, 0);
	int maxDistance = (canny.width() + canny.height()) * 2;

	cimg_forXY(edge, x, y) {
		edge(x, y, 0, 0) = canny(x, y, 0, 0);
	}

	hough_space = CImg<int>(180,maxDistance,1,1,0);
}
//compute entrance
vector<Vertex> Hough::compute()
{
    houghSpaceMapping();
    vector<Vertex> vertexs(find4InterchangePoints());
    return getA4Points(vertexs);
}
//make hough space
void Hough::houghSpaceMapping()
{
    int maxDistance = (edge.width() + edge.height()) * 2;
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
        if(edge(x,y,0)==255)//if edge point
        {
            cimg_forX(hough_space,angle)
            {
                int polar = (int)(x*cosCache[angle]+y*sinCache[angle]);
                if( polar >= -0.5*maxDistance && polar < 0.5*maxDistance )
                {
                    hough_space(angle,polar+0.5*maxDistance) ++;
                }
            }
        }
    }
}

//辅助函数，用来维护前13个最大点。保证每次只有最后一个发生了变动
void maintainArray(vector<int>& ranking,vector<pair<double,double>>& topkpoints)
{
    for(int i = ranking.size()-1;i>0;i--)
    {
        if(ranking[i]>ranking[i-1])
        {
            swap(ranking[i],ranking[i-1]);
            swap(topkpoints[i],topkpoints[i-1]);
        }
        else
        {
            break;
        }
    }
}

//vote for 13 lines, lines vote for 4 interchange points
vector<Vertex> Hough::find4InterchangePoints()
{
//first, find top 13 points in hough space using insert sort
    vector<int> ranking;//store weight
    vector<pair<double,double>> topkpoints;//store lines data
    int maxDistance = (edge.width() + edge.height()) * 2;
    cimg_forXY(hough_space,angle,p)
    {
        int polar = p - 0.5*maxDistance;
        if(ranking.size()<KP)
        {
            double ra = (double)angle*PI/180.0;//real angle
            double m = -cos(ra)/sin(ra);
            double b = (double) polar/sin(ra);
            if(sin(ra) - 0 < 1e-6)
            {
                m = 0;
                b = (double) polar / cos(ra);
            }

            ranking.push_back(hough_space(angle,p));
            topkpoints.push_back(make_pair(m,b));
            maintainArray(ranking,topkpoints);
        }
        else if(hough_space(angle,p)>ranking[KP-1])
        {
            double ra = (double)angle*PI/180.0;//real angle
            double m = -cos(ra)/sin(ra);
            double b = (double) polar/sin(ra);
            if(sin(ra) - 0 < 1e-6)
            {
                m = 0;
                b = (double) polar / cos(ra);
            }
            ranking[KP-1] = hough_space(angle,p);
            topkpoints[KP-1] = make_pair(m,b);
            maintainArray(ranking,topkpoints);
        }
    }
    #ifdef DEBUG
    for(int i = 0;i<KP;i++)
    {
        double m1 = topkpoints[i].first;
        double b1 = topkpoints[i].second;
        if(m1==0)
        {
            cout<<"line: x = "<<b1;
        }
        else
        {
            cout<<"line: y = "<<m1<<" * x + "<<b1;
        }
        cout<<" with ranking "<<ranking[i]<<endl;
    }
    #endif // DEBUG

//lines votes for 4 interchange points
/*
每条线对其他所有线求交点。如果交点在范围内，则其权重累加。如果交点距离过近(上下8)，则合并（求和后取平均数）
需要有这么一个数据结构：
1. 能够记录当前位置与影响节点数，并更新输出点数（即平均数）
2. 能够记录权重并输出
*/
//这里有个小改动，为了减少计算，规定了i和j的大小关系
    vector<Vertex> vertexs;
    for(int i = 0;i<KP;i++)
    {
        for(int j = i+1;j<KP;j++)
        {
            double m1 = topkpoints[i].first;
            double b1 = topkpoints[i].second;
            double m2 = topkpoints[j].first;
            double b2 = topkpoints[j].second;

            int xx,yy;
            if(m1!=0 && m2!=0)
            {
                double deltaX = (b2-b1)/(m1-m2);
                xx = round(deltaX);
                yy = round(m1*deltaX+b1);
            }
            else if(m1 == 0 && m2 == 0)
            {
                continue;
            }
            else if(m1 == 0)
            {
                xx = round(b1);
                yy = round(m2*b1+b2);
            }
            else if(m2 == 0)
            {
                xx = round(b2);
                yy = round(m1*b2+b1);
            }


            //check if old points are close to this.
            bool foundVertex = false;
            for(int k = 0;k<vertexs.size();k++)
            {
                if(vertexs[k].checkClosePoint(xx,yy))
                {
                    vertexs[k].addPoint(xx,yy);
                    foundVertex = true;
                    break;
                }
            }
            if(!foundVertex)
            {
                vertexs.push_back(Vertex(xx,yy));
            }
        }
    }
    sort(vertexs.begin(),vertexs.end(),[](const Vertex &v1,const Vertex& v2)
         {
            return v1.weight>v2.weight;
         });

//得到排序后的数组
    return vertexs;

}
//find 4 points in correct order.
vector<Vertex> Hough::getA4Points(vector<Vertex>& vertexs)
{
    #ifdef DEBUG
    cout<<"top4 point"<<endl;
    for(int i = 0;i<vertexs.size();i++)
    {
        cout<<vertexs[i].x<<" "<<vertexs[i].y<<" "<<vertexs[i].weight<<endl;
    }
    #endif // DEBUG
    vector<Vertex> topv;
    int counting = 0;
    int i = 0;
    while(counting<4 && i<vertexs.size())
    {
        if(vertexs[i].x>0 && vertexs[i].x < edge.width() && vertexs[i].y > 0 && vertexs[i].y < edge.height())
        {
            topv.push_back(vertexs[i]);
            counting++;
        }
        i++;
    }
    if(counting<4 && i==vertexs.size())
    {
        cout<<"error! not enough valuable point!"<<endl;
        return topv;
    }

/*
下面矫正A4纸节点的顺序
1. 判断A4纸是正放的还是斜放的。如果A4纸是正放的，则高度为1:2:1，不然是2:2

2.  如果A4纸是正放的，则顺时针来一圈
    如果A4纸是斜放的，则找到最高的点，以其作为第一个候选点。
        这时候，找到距离它最近的点作为短边。
            有两种情况，如果该点在它左边，则这个点为第一个点。不然，这个点自己作为第一个点
    第二个点最近的为第三个点
    其次为第四个点
    如此，完成矫正
*/
    sort(topv.begin(),topv.end(),[](const Vertex& v1,const Vertex& v2)
         {
             return v1.y<v2.y;
         });

    if(abs(topv[0].y-topv[1].y)<HEIGHTDIFF)//正放的
    {
        if(topv[0].x>topv[1].x)
        {
            swap(topv[0],topv[1]);
        }
         /*
        首先确定第一个点和最后一个点
        方法：
            从第一个点分别向其他三个点引直线
            显然，如果剩余两个点在线的同侧，则这条线是合适的。反之则是不合适的。
            这样就拿到了两组对角点
        */
        //找0点的对点
        int oppoPoint=0;
        double delta;
        for(int i = 1;i<=3;i++)
        {
            int status = 0;
            //过两点的直线
            if(topv[i].x!=topv[0].x)
            {
                double linek = (double) (topv[i].y - topv[0].y)/(topv[i].x-topv[0].x);
                double lineb = (double) topv[i].y - linek*topv[i].x;
                for(int j = 1;j<=3;j++)
                {
                    if(i==j)
                        continue;
                    int y = topv[j].y;
                    int x = topv[j].x;
                    delta =(double)linek * x + lineb;
                    if(status == 0)
                    {
                        if(y>delta)
                        {
                            status = 1;
                        }
                        else if(y<delta)
                        {
                            status = -1;
                        }
                    }
                    else if(status==1 && y<delta ||status==-1 && y > delta)
                    {
                        oppoPoint = i;
                    }
                }
            }
            else
            {
                //两点位于同一条直线上，肯定不是对点
                continue;
            }

            if(oppoPoint!=0)
                break;
        }
        if(oppoPoint != 2)
        {
            swap(topv[2],topv[3]);
        }
    }
    else//A4纸是斜着放的
    {
        //以最高的为第一个点
        //决定最上的短边，这两个点为0和1
        if(pow(topv[0].x-topv[1].x,2)+pow(topv[0].y-topv[1].y,2) > pow(topv[0].x-topv[2].x,2)+pow(topv[0].y-topv[2].y,2))//dist 0->1 > dist 0->2
        {
            swap(topv[1],topv[2]);
        }
        //调整短边的顺序
        if(topv[0].x>topv[1].x)
        {
            swap(topv[0],topv[1]);
        }

        if(topv[3].x>topv[2].x)
        {
            swap(topv[2],topv[3]);
        }
    }



    return topv;
}
