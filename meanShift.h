#ifndef MEANSHIFT
#define MEANSHIFT

#include "CImg.h"
#include "hough.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <utility>
#include <ctime>
using namespace std;
using namespace cimg_library;
/*
    ����ͼ������е�
    ����ÿһ����
        ����һ�㿪ʼչ������
        ����x-h ~ x+h,y-h~y+h����ɫ������ɫ����ŷ�Ͼ���С��һ��Ҫ��ĵ�
        �����¼������
            ���û����ӣ���ѹ�������
        ������ɫ������ռ�����

        �ж��Ƿ��ҵ���ֹ�㣨�ռ䡢��ɫһ������Ҫ�󣬼�ֹͣ��

        û�����ظ���
        ����ֹͣ���������ж����еĵ����¸�ֵ��
*/

//�����ĶԶ�ά����������from->to�ķ���������������
vector<double> affine_fit(vector<vector<double>> from,vector<vector<double>> to)
{
    int dim = from[0].size();
    //create dim * dim+1 matrix, fill it
    vector<vector<double>> c(dim);

    for(int j = 0;j<dim;j++)
    {
        c[j] = vector<double>(dim+1,0);
        for(int k = 0;k<dim+1;k++)
        {
            for(int i = 0;i<from.size();i++)
            {
                vector<double> qi(from[i]);
                qi.push_back(1);

                c[j][k] += qi[k]*to[i][j];

            }
        }
    }
    cout<<"c"<<endl;
    cout<<c.size()<<" "<<c[0].size()<<endl;
    for(int j = 0;j<c.size();j++)
    {
        for(int k = 0;k<c[j].size();k++)
        {
            cout<<c[j][k]<<" ";
        }
        cout<<endl;
    }

    vector<vector<double>> Q;
    for(int i = 0;i<dim+1;i++)
    {
        vector<double> qq(dim+1,0);
        Q.push_back(qq);
    }
    for(int qi = 0;qi<from.size();qi++)
    {
        vector<double> qt(from[qi]);
        qt.push_back(1);
        for(int i = 0;i<dim+1;i++)
        {
            for(int j = 0;j<dim+1;j++)
            {
                Q[i][j] += qt[i]*qt[j];
            }
        }
    }
    cout<<"Q"<<endl;
    for(int j = 0;j<dim+1;j++)
    {
        for(int k = 0;k<dim+1;k++)
        {
            cout<<Q[j][k]<<" ";
        }
        cout<<endl;
    }

    vector<vector<double>> m(dim+1);
    for(int i = 0;i<dim+1;i++)
    {
        m[i]=vector<double>(2*dim+1,0);
        for(int j = 0;j<2*dim+1;j++)
        {
            if(j<dim+1)
            {
                m[i][j] = Q[i][j];
            }
            else
            {
                m[i][j] = c[j-dim-1][i];
            }
        }

    }
    cout<<"m"<<endl;
    cout<<m.size()<<" "<<m[0].size()<<endl;
    for(int j = 0;j<dim+1;j++)
    {
        for(int k = 0;k<dim*2+1;k++)
        {
            cout<<m[j][k]<<" ";
        }
        cout<<endl;
    }

    double eps = 1.0/pow(10,10);
    int h = m.size(),w = m[0].size();
    for(int y = 0 ;y<h;y++)
    {
        int maxrow = y;
        for(int y2=y+1;y2<h;y2++)
        {
            if(fabs(m[y2][y])>fabs(m[maxrow][y]))
                maxrow = y2;
        }
        swap(m[maxrow],m[y]);
        if(fabs(m[y][y])<=eps)
        {
            return vector<double>();
        }
        for(int y2=y+1;y2<h;y2++)
        {
            double cc = m[y2][y]/m[y][y];
            for(int x = y;x<w;x++)
            {
                m[y2][x] -= m[y][x] * cc;
            }
        }
    }
    for(int y = h-1;y>-1;y--)
    {
        double cc = m[y][y];
        for(int y2 = 0;y2<y;y2++)
        {
            for(int x = w-1; x > y-1; x--)
            {
                m[y2][x] -= m[y][x] * m[y2][y]/cc;
            }
        }
        m[y][y]/=cc;
        for(int x = h;x<w;x++)
        {
            m[y][x]/=cc;
        }
    }
    cout<<"m"<<endl;
    for(int j = 0;j<dim+1;j++)
    {
        for(int k = 0;k<dim*2+1;k++)
        {
            cout<<m[j][k]<<" ";
        }
        cout<<endl;
    }

    vector<double> ans;
    for(int j = 0;j<dim;j++)
    {
        for(int i = 0;i<=dim;i++)
        {
            ans.push_back(m[i][j+dim+1]);
        }
    }
    return ans;

}

class meanShift
{
private:
    CImg<int> source;
    CImg<int> origin;
public:
    meanShift(string filename)
    {
        source = CImg<int>(filename.c_str());
        origin = source;
    }

    void compute(void)
    {
        time_t start = time(NULL);
        int width = source.width(),height = source.height();
        const int h = 600;//����
        const double dist = 100;//��ɫ�ռ�

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
                //�ж���ֹ
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
        //��ֵ
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
        //source.display();
        source.save("meanshift.bmp");
        edge.save("edge.bmp");

        Hough hough(edge,edge);
        vector<Point> intersections(hough.find_point());
        time_t stop = time(NULL);
        cout<<"��Ե���ʱ��:"<<stop-start<<"s"<<endl;

        for(int i = 0;i<4;i++)
        {
            cout<<intersections[i].x<<" "<<intersections[i].y<<endl;
        }
        bool shortFirst = true;
        if(pow(intersections[1].x-intersections[0].x,2) +pow(intersections[1].y-intersections[0].y,2) > pow(intersections[1].x-intersections[2].x,2) +pow(intersections[1].y-intersections[2].y,2))
            shortFirst = false;


        vector<vector<double>> from,to;
        //�̱���ǰ
        if(shortFirst)
        {
            for(int i = 0;i<4;i++)
            {
                to.push_back(vector<double>{intersections[i].x,intersections[i].y});
            }
        }
        else
        {
            for(int i = 1;i<4;i++)
            {
                to.push_back(vector<double>{intersections[i].x,intersections[i].y});
            }
            to.push_back(vector<double>{intersections[0].x,intersections[0].y});
        }
        double ra = 4;
        int a4w = 210 * ra,a4h = 297 * ra;

        from.push_back(vector<double>{0,0});
        from.push_back(vector<double>{a4w,0});
        from.push_back(vector<double>{a4w,a4h});
        from.push_back(vector<double>{0,a4h});
        vector<double> parameter(affine_fit(from,to));
        for(int i = 0;i<parameter.size();i++)
        {
            cout<<parameter[i]<<endl;
        }
        if(parameter.size()==0)
        {
            cout<<"��������"<<endl;
            return;
        }

        CImg<int> a4(a4w,a4h,1,3,0);
        cimg_forXY(a4,x,y)
        {
            int aimX = (double)parameter[0] * x + parameter[1] *y + parameter[2];
            int aimY = (double)parameter[3] * x + parameter[4] *y + parameter[5];
            for(int i = 0;i<3;i++)
                a4(x,y,i) = origin(aimX,aimY,i);
        }
        a4.save("a4.bmp");

    }
};


#endif // MEANSHIFT
