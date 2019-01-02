#include "NumberExtract.h"
#include <cmath>
#include <queue>
#include <map>
using namespace std;
#define EDGE 255
#define NOEDGE 0

// XY�����������
int dilateXY(const CImg<int>& img, int x, int y)
{
	if (img(x, y) == NOEDGE)
    {
        int maximum = 0;
		for(int i = -1;i<=1;i++)
        {
            for(int j = -1;j<=1;j++)
            {
                if(x+i>=0 && y+j >= 0 && x+i < img.width() && y+j < img.width())
                {
                    if(img(x+i,y+j) != NOEDGE && img(x+i,y+j) > maximum)
                    {
                        maximum = img(x+i,y+j);
                    }
                }
            }
        }
        return maximum;
	}
	else
        return EDGE;

}
int eroseXY(const CImg<int>& img, int x, int y)
{
    int minimum = 255;
	for(int i = -1;i<=1;i++)
    {
        for(int j = -1;j<=1;j++)
        {
            if(x+i>=0 && y+j >= 0 && x+i < img.width() && y+j < img.width())
            {
                if(img(x+i,y+j)<minimum)
                {
                    minimum = NOEDGE;
                }
            }
        }
    }
    return minimum;
}


//����任�������
vector<double> my_affine_fit(vector<vector<double>> from,vector<vector<double>> to)
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

/*************************************************************************
 ��С���˷����ֱ�ߣ�y = a*x + b; n������; r-���ϵ��[-1,1],fabs(r)->1,˵��x,y֮�����Թ�ϵ�ã�fabs(r)->0��x,y֮�������Թ�ϵ�����������
 a = (n*C - B*D) / (n*A - B*B)
 b = (A*D - B*C) / (n*A - B*B)
 r = E / F
 ���У�
 A = sum(Xi * Xi)
 B = sum(Xi)
 C = sum(Xi * Yi)
 D = sum(Yi)
 E = sum((Xi - Xmean)*(Yi - Ymean))
 F = sqrt(sum((Xi - Xmean)*(Xi - Xmean))) * sqrt(sum((Yi - Ymean)*(Yi - Ymean)))
**************************************************************************/

vector<double> LineFitLeastSquares(vector<int> data_x, vector<int> data_y)
{
    int data_n = data_x.size();
	double A = 0.0;
	double B = 0.0;
	double C = 0.0;
	double D = 0.0;
	double E = 0.0;
	double F = 0.0;

	for (int i=0; i<data_n; i++)
	{
		A += data_x[i] * data_x[i];
		B += data_x[i];
		C += data_x[i] * data_y[i];
		D += data_y[i];
	}

	// ����б��a�ͽؾ�b
	double a, b, temp = 0;
	if( temp = (data_n*A - B*B) )// �жϷ�ĸ��Ϊ0
	{
		a = (data_n*C - B*D) / temp;
		b = (A*D - B*C) / temp;
	}
	else
	{
		a = 1;
		b = 0;
	}

	// �������ϵ��r
	double Xmean, Ymean;
	Xmean = B / data_n;
	Ymean = D / data_n;

	double tempSumXX = 0.0, tempSumYY = 0.0;
	for (int i=0; i<data_n; i++)
	{
		tempSumXX += (data_x[i] - Xmean) * (data_x[i] - Xmean);
		tempSumYY += (data_y[i] - Ymean) * (data_y[i] - Ymean);
		E += (data_x[i] - Xmean) * (data_y[i] - Ymean);
	}
	F = sqrt(tempSumXX) * sqrt(tempSumYY);

	double r;
	r = E / F;

	return vector<double>{a,b};
}
/*
    ������������Ϊhough�任ʼ���Ǵ������ģ������������鷳�����Կ��ǵ���㼯����ͼ����ıߣ��������ı߽�����ͨ�Լ�����������
    ������ͨ�Լ��ϵͳ����˽��У�
        ���ں���ıߣ���ͨ�Լ���Ժ���Ϊ����������������ش���50�����϶�Ϊ�����ͨ��
        ����ı�ͬ��
        ������ԭ���Ǿ������������������ڣ�Ҳ�����ų�����
            ����н�С�����©���ˣ�������ȥ������С����㡣
*/
void NumberExtract::marginDelete()
{
    queue<pair<int,int>> marginQueue;
    map<pair<int,int>,bool> inQueue;
    int pixelLimit = 10;//���������شﵽ50�����϶�Ϊ���
    //���ı߽���ɨ��
    int pixelCount = 0;//��������
    bool flag = false;
    int x = 1;
    int y;
    //����ɨ��
    for(y = 1;y<bipaper.height();y++)
    {
        if(bipaper(x,y) == EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x,y));
            }
        }
        else if(bipaper(x+1,y) == EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x+1,y));
            }
        }
        else// no edge, two chance
        {
            if(!flag)
            {
                flag = true;
            }
            else
            {
                flag = false;
                pixelCount = 0;
            }
        }
    }
    pixelCount = 0;//��������
    flag = false;
    x = bipaper.width()-1;
    //����ɨ��
    for(y = 1;y<bipaper.height();y++)
    {
        if(bipaper(x,y) == EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x,y));
            }
        }
        else if(bipaper(x-1,y)==EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x-1,y));
            }
        }
        else// no edge, two chance
        {
            if(!flag)
            {
                flag = true;
            }
            else
            {
                pixelCount = 0;
                flag = false;
            }
        }
    }
    pixelCount = 0;//��������
    flag = false;
    y = 1;
    //����ɨ��
    for(x = 1;x<bipaper.width();x++)
    {
        if(bipaper(x,y) == EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x,y));
            }
        }
        else if(bipaper(x,y+1)==EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x,y+1));
            }
        }
        else// no edge, two chance
        {
            if(!flag)
            {
                flag = true;
            }
            else
            {
                pixelCount = 0;
                flag = false;
            }
        }
    }

    pixelCount = 0;//��������
    flag = false;
    y = bipaper.height()-1;
    //����ɨ��
    for(x = 1;x<bipaper.width();x++)
    {
        if(bipaper(x,y) == EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x,y));
            }
        }
        else if(bipaper(x,y-1)==EDGE)
        {
            flag = false;
            pixelCount++;
            if(pixelCount>pixelLimit)
            {
                marginQueue.push(make_pair(x,y-1));
            }
        }
        else// no edge, two chance
        {
            if(!flag)
            {
                flag = true;
            }
            else
            {
                pixelCount = 0;
                flag = false;
            }
        }
    }

    while(!marginQueue.empty())
    {
        pair<int,int> p = marginQueue.front(); marginQueue.pop();
        if(bipaper(p.first,p.second) == EDGE)
        {
            bipaper(p.first,p.second) = NOEDGE;
            for(int i = -1;i<=1;i++)
            {
                if(p.first+i>=0 && p.first+i<bipaper.width())
                {
                    if(bipaper(p.first+i,p.second) == EDGE && inQueue.find(make_pair(p.first+i,p.second)) == inQueue.end())
                    {
                        marginQueue.push(make_pair(p.first+i,p.second));
                        inQueue[make_pair(p.first+i,p.second)] = true;
                    }
                }
            }
            for(int j = -1;j<=1;j++)
            {
                if(p.second+j>=0 && p.second+j<bipaper.height())
                {
                    if(bipaper(p.first,p.second+j) == EDGE && inQueue.find(make_pair(p.first,p.second+j)) == inQueue.end())
                    {
                        marginQueue.push(make_pair(p.first,p.second+j));
                        inQueue[make_pair(p.first,p.second+j)] = true;
                    }
                }
            }
        }
    }
    xlMargin = 0;
    xrMargin = bipaper.width();
    cout<<"update margin x:"<<xlMargin<<"\t"<<xrMargin<<endl;
    cout<<"update margin y:"<<yuMargin<<"\t"<<ydMargin<<endl;

}

void NumberExtract::compute()
{

    string baseDireAddress = "./img/"+filename;
	if (_access(baseDireAddress.c_str(), 0) == -1)
		_mkdir(baseDireAddress.c_str());
    BPnet* bp = BPnet::GetInstance();
    ofstream fout(filename+"_out");
    //�����õ���ֵ��ͼ��
    getBinaryImg();
    /*
    ������ΪA4ֽ���������Margin����
    */
    /*
    �Ӽ��ȱ�Ե��ʼ��������
    */
    //�ϱ�Ե�ع�
    marginDelete();
    bipaper.display();
    //��ȡ�ַ���
    vector<int> horiLines(getVerticallines());
    cachepaper = CImg<int>(a4paper.width(),a4paper.height(),1,1,0);
    #ifdef DEBUG
    cout<<"vertical lines"<<endl;
    for(int i = 0;i<VERTICAL_NUM;i++)
    {
        cout<<horiLines[i]<<endl;
    }
    #endif // DEBUG
    //����ÿһ����ȡ���ַ��н��в���
    /*
    ����ÿһ���ַ��еĶ�������Ϊ�ַ��������ߡ�
        �����������䣬���������ڵ����������߾������е㣩
        Ϊ��Ӧ�Դ�бд������������������ߵĽ��㿪ʼ�����ܽ�������������Ϊ25�����ҵ�����ͨ������Ϊһ������
            ����ÿ�����֣�������ˮƽ�����봹ֱ����ı����������գ�Ӧ����1�����������
            ������Ϊ784ά����
    */
    for(int i = 0;i<horiLines.size();i++)
    {
        double deltaParam = 5;
        int st,ed;
        if(i==0)//��һ����
        {
            int delta = (horiLines[i+1] - horiLines[i])/deltaParam;
            st = horiLines[i]-delta>0?horiLines[i]-delta:0;
            ed = horiLines[i]+delta;
        }
        else if(i==horiLines.size()-1)
        {
            int delta = (horiLines[i]-horiLines[i-1])/deltaParam;
            st = horiLines[i]-delta;
            ed = horiLines[i]+delta>=horiLines.size()?horiLines.size()-1:horiLines[i]+delta;
        }
        else
        {
            st = horiLines[i] - (horiLines[i]-horiLines[i-1])/deltaParam;
            ed = horiLines[i] + (horiLines[i+1]-horiLines[i])/deltaParam;
        }

        //�����������н��лҶ�ֱ��ͼͳ��
        //vector<int> hist(computeHorizontalGrayHist(st,ed));
        //ͳ�Ƴ���ʼ��ͽ�����
        vector<bool> isVisited(bipaper.width(),false);
        int up = 0,down = 0;
        int ver = horiLines[i];
        int minSquare = 60;
        vector<vector<int>> numbers;
        cout<<"lines"<<i<<" with vertical lines:"<<horiLines[i]<<"\tst:"<<st<<"\ted:"<<ed<<endl;
        vector<int> data_x;//��һ�μ�����������ĵ��x����
        vector<int> data_y;//��һ�μ�����������ĵ��y����
        for(int x = xlMargin;x<xrMargin;x++)
        {
            up = 0,down = 0;
            if(isVisited[x])
                continue;
            //��������
            while(ver-up>st || ver+down<ed)
            {

                //try to find point up
                if(ver-up>st)
                {
                    //25�����ҵ�
                    if(bipaper(x,ver-up)==EDGE && !isVisited[x])
                    {
                        vector<int> coordinate(findPoint(x,ver-up,isVisited));
                        if((coordinate[2]-coordinate[0])*(coordinate[3]-coordinate[1])>minSquare)
                        {
                            numbers.push_back(coordinate);
                            data_x.push_back((coordinate[0]+coordinate[2])/2);
                            data_y.push_back((coordinate[1]+coordinate[3])/2);
                        }

#ifdef DEBUG
                        for(int j = 0;j<4;j++)
                        {
                            cout<<coordinate[j]<<"\t";
                        }
                        cout<<endl;
#endif
                        break;
                    }
                    up++;
                }
                //try to find point down
                if(ver+down<ed)
                {
                    if(bipaper(x,ver+down)==EDGE && !isVisited[x])
                    {
                        vector<int> coordinate(findPoint(x,ver+down,isVisited));
                        if((coordinate[2]-coordinate[0])*(coordinate[3]-coordinate[1])>minSquare)
                        {
                            numbers.push_back(coordinate);
                            data_x.push_back((coordinate[0]+coordinate[2])/2);
                            data_y.push_back((coordinate[1]+coordinate[3])/2);
                        }
#ifdef DEBUG
                        for(int j = 0;j<4;j++)
                        {
                            cout<<coordinate[j]<<"\t";
                        }
                        cout<<endl;
#endif
                        break;
                    }
                    down++;
                }
            }
        }
        /*
            �����������ĵ���������ߣ�Ȼ����ϳ�ֱ�ߣ��ٴν�������
                ���Ѿ��õ��ĵ�����������½���������
                ����ÿ�����ٽ���֮�����б�ʣ�Ȼ�����б�ʵ�ƽ��ֵ
                ѡ�����һ���㣬���ֱ��
                ͨ���Ը�ֱ�����½����ٴ�����
        */
        //�����Ѿ��õ������ĵ�����С���˷���ϳ�һ��ֱ��
        vector<double> para(LineFitLeastSquares(data_x,data_y));
        double line_k = para[0];
        double line_b = para[1];
#ifdef DEBUG
cout<<"New Vertex"<<endl;
#endif // DEBUG
        //������ֱ���ϵĵ�����ٴ���ֵ
        int uplimit = ver - st;
        int downlimit = ed - ver;
        for(int x = xlMargin;x<xrMargin;x++)
        {
            up = 0,down = 0;
            if(isVisited[x])
            {
                continue;
            }
            int currentVerticalCenter = round(line_k*x+line_b);
            if(currentVerticalCenter<yuMargin || currentVerticalCenter >ydMargin)
                continue;

            while(up<uplimit || down<downlimit)
            {

                //try to find point up
                if(up<uplimit)
                {
                    //25�����ҵ�
                    if(bipaper(x,currentVerticalCenter-up)==EDGE && !isVisited[x])
                    {
                        vector<int> coordinate(findPoint(x,currentVerticalCenter-up,isVisited));
                        if((coordinate[2]-coordinate[0])*(coordinate[3]-coordinate[1])>minSquare)
                            numbers.push_back(coordinate);
#ifdef DEBUG
                        for(int j = 0;j<4;j++)
                        {
                            cout<<coordinate[j]<<"\t";
                        }
                        cout<<endl;
#endif
                        break;
                    }
                    up++;
                }
                //try to find point down
                if(down<downlimit)
                {
                    if(bipaper(x,currentVerticalCenter+down)==EDGE && !isVisited[x])
                    {
                        vector<int> coordinate(findPoint(x,currentVerticalCenter+down,isVisited));
                        if((coordinate[2]-coordinate[0])*(coordinate[3]-coordinate[1])>minSquare)
                            numbers.push_back(coordinate);
#ifdef DEBUG
                        for(int j = 0;j<4;j++)
                        {
                            cout<<coordinate[j]<<"\t";
                        }
                        cout<<endl;
#endif
                        break;
                    }
                    down++;
                }
            }

        }//end ��С���˷�����
/*�и�����غϵ�����*/
        bool valuable = true;
        int acNum;
        if(i%3==0 && numbers.size()<8)
        {
            acNum = 8;
            valuable = false;
        }
        else if(i%3==1 && numbers.size()<11)
        {
            acNum = 11;
            valuable = false;
        }
        else if(i%3==2 && numbers.size()<18)
        {
            acNum = 18;
            valuable = false;
        }
        if(!valuable)
        {
            //�����Ŀ�ʼ�и�
            vector<pair<int,int>> widthIndex ;
            for(int j = 0;j<numbers.size();j++)
            {
                widthIndex.push_back(make_pair(j,numbers[j][2]-numbers[j][0]));
            }
            sort(widthIndex.begin(),widthIndex.end(),[](const pair<int,int>& p1,const pair<int,int>& p2)
                 {
                    return p1.second>p2.second;
                 });
            int index = 0;
            while(numbers.size()<acNum && index<widthIndex.size())
            {
                //��indexλ�õ����ֽ����и�
                int deltax = 0.5*widthIndex[index].second;
                int point = widthIndex[index].first;
                int coordinateX = numbers[point][0]+deltax;
                cout<<"�и��"<<coordinateX<<endl;
                numbers.push_back({coordinateX,numbers[point][1],numbers[point][2],numbers[point][3]});
                numbers[point][2] = coordinateX;
                index++;
            }
        }

        //��ÿ�����ֵ���С��x�����������
        sort(numbers.begin(),numbers.end(),[](const vector<int>& v1,const vector<int>& v2)
             {
                 return v1[0]<v2[0];
             });
        /*��ÿһ���õ��ĵ���д������⣬Ͷ�䵽28*28�����ϣ�Ȼ��ת����784ά�������д洢*/
        /*
        ��������뷨��
            1.��������һ��������ĺ��ӣ�����Ϊ���ֳ�������ֵ+5
            2.������Ͷ�䵽�����м�
            3. �������������κ���и�ʴ
        */
#ifdef DEBUG
    cout<<"check array numbers : "<<numbers.size()<<endl;
    for(int num_index = 0;num_index<numbers.size();num_index++)
    {
        for(int j = 0;j<4;j++)
            cout<<numbers[num_index][j]<<"\t";
        cout<<endl;
    }
#endif
        for(int num_index = 0;num_index<numbers.size();num_index++)
        {
            int xmin = numbers[num_index][0],ymin =  numbers[num_index][1];
            int xmax = numbers[num_index][2],ymax =  numbers[num_index][3];
            int width = xmax - xmin;
            int height = ymax - ymin;

            int digitSize = 28;
            int imgSize = width<height ? height + 6 : width + 6;
            CImg<int> digit(imgSize,imgSize,1,1,0);
            CImg<int> origindigit(imgSize,imgSize,1,1,0);

            int p_x = (imgSize - width)/2;
            int p_y = (imgSize - height)/2;

            vector<int> digitData;
            //��A4ֽ��ȥԭ����
            for(int originX = xmin;originX<=xmax;originX++)
            {
                for(int originY = ymin;originY<=ymax;originY++)
                {
                    //in the digit image digit(xx,yy)
                    int xx = originX-xmin+p_x;
                    int yy = originY-ymin+p_y;
                    origindigit(xx,yy) = a4paper(originX,originY);
                    digit(xx,yy) = bipaper(originX,originY);
                }
            }
            /*
            �ٴν��ж�ֵ��������Ϊ0��ǰ��Ϊ255-ԭʼֵ
            */
            cimg_forXY(digit,xx,yy)
            {
                if(digit(xx,yy)==EDGE)
                {
                    digit(xx,yy) = 255 - origindigit(xx,yy);
                    digit(xx,yy) = digit(xx,yy) + 100 > 255 ? 255 : digit(xx,yy) + 100;
                }
                else
                {
                    digit(xx,yy) = 0;
                }
            }
            //Ϊ�˽�������м��������⣬�������ͺ�ʴ
            CImg<int> digitCache(digit);
            cimg_forXY(digit,dx,dy)
            {
                digit(dx,dy) = dilateXY(digitCache,dx,dy);
            }
            /*
            cimg_forXY(digitCache,dx,dy)
            {
                digitCache(dx,dy) = dilateXY(digit,dx,dy);
            }

            cimg_forXY(digit,dx,dy)
            {
                digit(dx,dy) = eroseXY(digitCache,dx,dy);
            }*/
            digitCache.clear();
            //��digitͶ�䵽28*28
            CImg<int> acNum(digitSize,digitSize,1,1,0);
            //����û��д��MNIST���ݼ���˳�����������
            for(int originY = 0;originY<digitSize;originY++)
            {
                for(int originX = 0;originX<digitSize;originX++)
                {
                    int aimX = round((double)originX*imgSize/digitSize);
                    int aimY = round((double)originY*imgSize/digitSize);
                    digitData.push_back(digit(aimX,aimY));
                    acNum(originX,originY) = digit(aimX,aimY);
                    fout<<digit(aimX,aimY)<<" ";
                }
            }
            //acNum.display();
            fout<<endl;
#ifdef OUTPUT
            string filename = baseDireAddress+"/line"+to_string(i)+"_num_"+to_string(num_index)+".bmp";
            cout<<filename<<endl;
            acNum.save(filename.c_str());
#endif
            //int preNumber = bp->predictNum(digitData);
            //cout<<preNumber<<" ";
            digit.clear();
            acNum.clear();
        }
        cout<<endl;

    }//end horizontalLines��ÿһ����ֱ��
#ifdef DEBUG
    bipaper.display();
#endif
    fout.close();
}

//25�����ҵ㣬�ҵ��ĵ�����Ͻǵ�x��y��������½ǵ�x��y���귵��
/*
ÿ�ε�����ʱ�򣬶�Ŀǰ���ڵķ������������չ�������չ�������е㣬����һ�μ�����չ
����isVisited���и���
*/
void updatePoint(int& xmin,int &xmax,int& ymin,int& ymax,int x,int y)
{
    if(x<xmin)
    {
        xmin = x;
    }
    else if(x>xmax)
    {
        xmax = x;
    }

    if(y<ymin)
    {
        ymin = y;
    }
    else if(y>ymax)
    {
        ymax = y;
    }
}

/*
ע������ȡֵ����ȡ��(xmin,ymin)->(xmax,ymax)���أ���Ȼ����ѭ��
*/
vector<int> NumberExtract::findPoint(int xx,int yy,vector<bool>& isVisited)
{
    int xmin = xx,ymin = yy;
    int xmax = xx,ymax = yy;
    queue<pair<int,int>> q;
    map<pair<int,int>,bool> visit;
    int deltaX = 2,deltaY = 8;
    q.push(make_pair(xx,yy));
    visit[make_pair(xx,yy)] = true;
    while(!q.empty())
    {
        pair<int,int> p = q.front();q.pop();
        for(int x = p.first-deltaX;x<=p.first+deltaX;x++)
        {
            for(int y = p.second - deltaY;y<=p.second+deltaY;y++)
            {
                if(bipaper(x,y) == EDGE && visit.find(make_pair(x,y)) == visit.end())
                {
                    visit[make_pair(x,y)] = true;
                    q.push(make_pair(x,y));
                    updatePoint(xmin,xmax,ymin,ymax,x,y);
                }
            }
        }
    }

    for(int i = xmin;i<=xmax;i++)
    {
        isVisited[i] = true;
    }
    vector<int> ans{xmin,ymin,xmax,ymax};
    return ans;
}
/*
�ȶ�ԭͼ������и�
�ٶ�ÿ����Χ����ͳ�ƣ�����Ҷ����ֵ����Сֵ����С�������
    Ȼ��ʹ�������䷽������ǰ���ͱ�����
*/
void NumberExtract::getBinaryImg()
{
    bipaper = CImg<int>(a4paper.width(),a4paper.height(),1,1,0);
    CImg<int> cache(bipaper);
    for(int i = 1;i<=cache.width()-2;i++)
    {
        for(int j = 1;j<=cache.height()-2;j++)
        {
            double gx = a4paper(i+1,j) - a4paper(i-1,j);
            double gy = a4paper(i,j+1) - a4paper(i,j-1);
            double grad = pow(gx,2)+pow(gy,2);
            #ifdef DEBUG
            if(grad>900)
                grad = 900;
            #endif
            cache(i,j) = grad;

        }
    }
#ifdef DEBUG
    cache.display();
    a4paper.display();
#endif // DEBUG
    cimg_forXY(bipaper,x,y)
    {
        if(cache(x,y)>=900)
        {
            bipaper(x,y) = EDGE;
        }
        else
        {
            int xmin = x-1<0?0:x-1,xmax = x+1>=bipaper.width()?bipaper.width()-1:x+1;
            int ymin = y-1<0?0:y-1,ymax = y+1>=bipaper.height()?bipaper.height()-1:y+1;
            for(int xx = xmin;xx<=xmax;xx++)
            {
                for(int yy = ymin;yy<=ymax;yy++)
                {
                    if(cache(xx,yy)>=900 || a4paper(xx,yy)<BITHRESHOLD)
                    {
                        bipaper(xx,yy) = EDGE;
                        continue;
                    }
                }
            }
            bipaper(x,y) = NOEDGE;
        }
    }
    cache.clear();
}
vector<int> NumberExtract::computeVerticalGrayHist()
{
    vector<int> hist(bipaper.height(),0);
    int st = xlMargin;
    int ed = xrMargin;
    for(int y = yuMargin;y<ydMargin;y++)
    {
        int sum = 0;
        for(int x = st;x<ed;x++)
        {
            if(bipaper(x,y)==EDGE)
                sum++;
        }
        hist[y] = sum;
    }

    return hist;
}

vector<int> NumberExtract::computeHorizontalGrayHist(int yl,int yh)
{
    vector<int> hist;
    for(int x = xlMargin;x<xrMargin;x++)
    {
        int sum = 0;
        for(int y = yl;y<=yh;y++)
        {
            if(bipaper(x,y)==EDGE)
            {
                sum++;
            }
        }
        hist.push_back(sum);
    }
    return hist;
}

/*
    ��ͼ����д�ֱ�����ϻҶ�ֱ��ͼͳ��
    �ض�������ÿһ���������ƽ��ֵ�����뵽һ��������
    �����͵õ��Ķ�������
*/
vector<int> NumberExtract::getVerticallines()
{
    vector<int> hist(computeVerticalGrayHist());
    vector<int> ans;
    vector<pair<int,int>> cache;

    //��Ե����A4ֽ�����㷨�����⣬���ܻ����ºڱ�
    int st = yuMargin;
    int ed = ydMargin;

    int vertical_diff = 0;//����������֮��ľ������С��vertical_diff������кϲ�
    int hist_begin,hist_end;
    int hist_threshold = 5;
    for(int i = st;i<ed;i++)
    {
        if(hist[i]>5)
        {
            hist_begin = i;
            break;
        }
    }
    for(int i = ed;i>st;i--)
    {
        if(hist[i]>5)
        {
            hist_end = i;
            break;
        }
    }
    vertical_diff = (hist_end-hist_begin)/ VERTICAL_NUM;
    vertical_diff = 0.6*vertical_diff;
    cout<<"vertical_diff"<<vertical_diff<<endl;


    //���б�Ե�ۺϣ�Ȼ���ҵ�����k�����㣬k=9
    /*
    ���Ƚ�hist��ֵ�������vector�У���Ȩ�ؽ�������Ȼ�������ң����ϵؽ��кϲ���ֱ�����ܺϲ����ҵ���k����Ϊֹ

    ���vector<int,int> value,weight
    ���н�������
    ���µ��Ͻ��кϲ�����
    ����vector<int> num���м���
    */
    vector<pair<int,int>> ranking;
    for(int i = 0;i<hist.size();i++)
    {
        if(i<st || i>ed)
            ranking.push_back(make_pair(i,0));
        else
            ranking.push_back(make_pair(i,hist[i]));
    }

    sort(ranking.begin(),ranking.end(),[](const pair<int,int>& t1,const pair<int,int>& t2)
         {
            return t1.second>t2.second;
         });

    vector<int> num;//rankingλ�õ�����֮��
    int counting = 0;//countingָ�����ranking�еĵ�ǰ���������
    while(counting<VERTICAL_NUM)
    {
        /*
            �����ж��Ƿ�����ظ�
                ��������ظ������޸��ظ���Ŀ��num����������������ranking����ͣ�
                ����������ظ�����ָ���ƶ�
        */
        //�ж��Ƿ����ظ�
        bool repeat = false;
        int repeatNum = 0;
        for(int i = 0;i<counting;i++)
        {
            if(abs(ranking[i].first/num[i]-ranking[counting].first)<vertical_diff)
            {
                repeat = true;
                repeatNum = i;
                break;
            }
        }
        if(repeat)
        {
            num[repeatNum]++;
            ranking[repeatNum].first += ranking[counting].first;
            for(auto iter = ranking.begin();iter!=ranking.end();iter++)
            {
                if(iter->first == ranking[counting].first)
                {
                    ranking.erase(iter);
                    break;
                }
            }
        }
        else
        {
            num.push_back(1);
            counting++;

        }

    }

    //�õ������߽��
    for(int i = 0;i<VERTICAL_NUM;i++)
    {
        ans.push_back(ranking[i].first/num[i]);
    }
    sort(ans.begin(),ans.end());
    return ans;
}
