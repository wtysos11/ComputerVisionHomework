
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

//�õ�����������
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
         /*
        ����ȷ����һ��������һ����
        ������
            �ӵ�һ����ֱ���������������ֱ��
            ��Ȼ�����ʣ�����������ߵ�ͬ�࣬���������Ǻ��ʵġ���֮���ǲ����ʵġ�
            �������õ�������Խǵ�
        */
        //��0��ĶԵ�
        int oppoPoint=0;
        double delta;
        for(int i = 1;i<=3;i++)
        {
            int status = 0;
            //�������ֱ��
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
                //����λ��ͬһ��ֱ���ϣ��϶����ǶԵ�
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
    else//A4ֽ��б�ŷŵ�
    {
        //����ߵ�Ϊ��һ����
        //�������ϵĶ̱ߣ���������Ϊ0��1
        if(pow(topv[0].x-topv[1].x,2)+pow(topv[0].y-topv[1].y,2) > pow(topv[0].x-topv[2].x,2)+pow(topv[0].y-topv[2].y,2))//dist 0->1 > dist 0->2
        {
            swap(topv[1],topv[2]);
        }
        //�����̱ߵ�˳��
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
