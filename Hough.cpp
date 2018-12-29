
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

	hough_space = CImg<int>(360,maxDistance,1,1,0);
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
        if(edge(x,y,0)==255)//if edge point
        {
            cimg_forX(hough_space,angle)
            {
                int polar = (int)(x*cosCache[angle]+y*sinCache[angle]);
                if( polar >= 0 && polar < maxDistance )
                {
                    hough_space(angle,polar) ++;
                }
            }
        }
    }
}

//��������������ά��ǰ13�����㡣��֤ÿ��ֻ�����һ�������˱䶯
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
    cimg_forXY(hough_space,angle,polar)
    {
        if(ranking.size()<KP)
        {
            double ra = (double)angle*PI/180.0;//real angle
            if(sin(ra)==0)
                ra = 0.01;
            double m = -cos(ra)/sin(ra);
            double b = (double) polar/sin(ra);

            ranking.push_back(hough_space(angle,polar));
            topkpoints.push_back(make_pair(m,b));
            maintainArray(ranking,topkpoints);
        }
        else if(hough_space(angle,polar)>ranking[KP-1])
        {
            double ra = (double)angle*PI/180.0;//real angle
            if(sin(ra)==0)
                ra = 0.01;
            double m = -cos(ra)/sin(ra);
            double b = (double) polar/sin(ra);

            ranking[KP-1] = hough_space(angle,polar);
            topkpoints[KP-1] = make_pair(m,b);
            maintainArray(ranking,topkpoints);
        }
    }

//lines votes for 4 interchange points
/*
ÿ���߶������������󽻵㡣��������ڷ�Χ�ڣ�����Ȩ���ۼӡ��������������(����8)����ϲ�����ͺ�ȡƽ������
��Ҫ����ôһ�����ݽṹ��
1. �ܹ���¼��ǰλ����Ӱ��ڵ����������������������ƽ������
2. �ܹ���¼Ȩ�ز����
*/
//�����и�С�Ķ���Ϊ�˼��ټ��㣬�涨��i��j�Ĵ�С��ϵ
    vector<Vertex> vertexs;
    for(int i = 0;i<KP;i++)
    {
        for(int j = i+1;j<KP;j++)
        {
            double m1 = topkpoints[i].first;
            double b1 = topkpoints[i].second;
            double m2 = topkpoints[j].first;
            double b2 = topkpoints[j].second;

            double deltaX = (b2-b1)/(m1-m2);
            int xx = round(deltaX);
            int yy = round(m1*deltaX+b1);

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

//�õ�����������
    return vertexs;

}
//find 4 points in correct order.
vector<Vertex> Hough::getA4Points(vector<Vertex>& vertexs)
{
    vector<Vertex> topv;
    for(int i = 0;i<4;i++)
    {
        topv.push_back(vertexs[i]);
    }
/*
����ȷ����һ��������һ����
������
    �ӵ�һ����ֱ���������������ֱ��
    ��Ȼ�����ʣ�����������ߵ�ͬ�࣬���������Ǻ��ʵġ���֮���ǲ����ʵġ�
    �������õ�������Խǵ�
*/
    int oppoPoint=0;
    for(int i = 1;i<3;i++)
    {
        int status = 0;
        //�������ֱ��
        double linek = (double) (topv[i].y - topv[0].y)/(topv[i].x-topv[0].x);
        double lineb = (double) topv[i].y - linek*topv[i].x;
        for(int j = 1;j<3;j++)
        {
            if(i==j)
                continue;

            double deltaY = topv[i].y - (linek * topv[i].x + lineb);
            if(status == 0)
            {
                status = deltaY>0?1:-1;
            }
            else if(status==1 && deltaY<0||status==-1 && deltaY>0)
            {
                oppoPoint = i;
            }
        }
        if(oppoPoint!=0)
            break;
    }
/*
�������A4ֽ�ڵ��˳��
1. �ж�A4ֽ�����ŵĻ���б�ŵġ����A4ֽ�����ŵģ���߶�Ϊ1:2:1����Ȼ��2:2

2.  ���A4ֽ�����ŵģ���˳ʱ����һȦ
    ���A4ֽ��б�ŵģ����ҵ���ߵĵ㣬������Ϊ��һ����ѡ�㡣
        ��ʱ���ҵ�����������ĵ���Ϊ�̱ߡ�
            ���������������õ�������ߣ��������Ϊ��һ���㡣��Ȼ��������Լ���Ϊ��һ����
    �ڶ����������Ϊ��������
    ���Ϊ���ĸ���
    ��ˣ���ɽ���
*/
    sort(topv.begin(),topv.end(),[](const Vertex& v1,const Vertex& v2)
         {
             return v1.y<v2.y;
         });

    if(abs(topv[0].y-topv[1].y)<HEIGHTDIFF)//���ŵ�
    {
        if(topv[0].x>topv[1].x)
        {
            swap(topv[0],topv[1]);
        }

        if(oppoPoint != 2)
        {
            swap(topv[2],topv[3]);
        }
    }
    else//A4ֽ��б�ŷŵ�
    {
        //����ߵ�Ϊ��һ����
        if(pow(topv[0].x-topv[1].x,2)+pow(topv[0].y-topv[1].y,2) > pow(topv[0].x-topv[2].x,2)+pow(topv[0].y-topv[2].y,2))//dist 0->1 > dist 0->2
        {
            swap(topv[1],topv[2]);
            if(oppoPoint == 1)
                oppoPoint = 2;
            else if(oppoPoint == 2)
                oppoPoint = 1;
        }
        //���վ��������У�Ӧ�ð��ճ�����
        if(oppoPoint !=2)
        {
            swap(topv[2],topv[3]);
        }
    }
    return topv;
}
