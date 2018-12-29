#ifndef CORRECT_H
#define CORRECT_H
#include "Hough.h"
#include "CImg.h"
using namespace std;
using namespace cimg_library;
//fromΪ
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

CImg<int> transformToA4(CImg<int>& origin,int vwidth,int vheight,vector<Vertex>& vertexs)
{
    double ra_x = (double)origin.width()/vwidth;
    double ra_y = (double)origin.height()/vheight;
#ifdef DEBUG
    cout<<"ratio x:"<<ra_x<<"\t ratio_y"<<ra_y<<endl;
    cout<<"Point in origin graph"<<endl;
#endif
    vector<vector<double>> from,to;
    for(int i = 0 ;i<4;i++)
    {
        to.push_back(vector<double>{(double)vertexs[i].x * ra_x,(double)vertexs[i].y*ra_y});
#ifdef DEBUG
    cout<<(double)vertexs[i].x * ra_x<<"\t"<<(double)vertexs[i].y*ra_y<<endl;
#endif
    }
    double ra = 4;
    int a4w = 210 * ra,a4h = 297 * ra;

    from.push_back(vector<double>{0,0});
    from.push_back(vector<double>{a4w,0});
    from.push_back(vector<double>{a4w,a4h});
    from.push_back(vector<double>{0,a4h});
    vector<double> parameter(affine_fit(from,to));

    CImg<int> a4(a4w,a4h,1,1,0);
    cimg_forXY(a4,x,y)
    {
        int aimX = (double)parameter[0] * x + parameter[1] *y + parameter[2];
        int aimY = (double)parameter[3] * x + parameter[4] *y + parameter[5];
        a4(x,y,0) = origin(aimX,aimY,0);
    }
    return a4;
}
#endif // CORRECT_H
