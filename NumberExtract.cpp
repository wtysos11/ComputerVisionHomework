#include "NumberExtract.h"
#include <cmath>
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


void NumberExtract::compute()
{
    BPnet* bp = BPnet::GetInstance();
    ofstream fout("imageOut");
    //�������õ���ֵ��ͼ��
    getBinaryImg();
    //��ȡ�ַ���
    vector<int> horiLines(getVerticallines());
    anspaper = CImg<int>(a4paper.width(),a4paper.height(),1,1,0);
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
        vector<vector<int>> numbers;
        cout<<"lines"<<i<<" with vertical lines:"<<horiLines[i]<<"\tst:"<<st<<"\ted:"<<ed<<endl;
        vector<int> data_x;//��һ�μ�����������ĵ��x����
        vector<int> data_y;//��һ�μ�����������ĵ��y����
        for(int x = MARGIN*bipaper.width();x<(1-MARGIN)*bipaper.width();x++)
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
                    if(bipaper(x,ver-up)==0)
                    {
                        vector<int> coordinate(findPoint(x,ver-up,isVisited));
                        numbers.push_back(coordinate);
                        data_x.push_back((coordinate[0]+coordinate[2])/2);
                        data_y.push_back((coordinate[1]+coordinate[3])/2);

                        for(int j = 0;j<4;j++)
                        {
                            cout<<coordinate[j]<<"\t";
                        }
                        cout<<endl;
                        break;
                    }
                    up++;
                }
                //try to find point down
                if(ver+down<ed)
                {
                    if(bipaper(x,ver+down)==0)
                    {
                        vector<int> coordinate(findPoint(x,ver+down,isVisited));
                        numbers.push_back(coordinate);
                        data_x.push_back((coordinate[0]+coordinate[2])/2);
                        data_y.push_back((coordinate[1]+coordinate[3])/2);
                        for(int j = 0;j<4;j++)
                        {
                            cout<<coordinate[j]<<"\t";
                        }
                        cout<<endl;
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

        //������ֱ���ϵĵ�����ٴ���ֵ
        int uplimit = ver - st;
        int downlimit = ed - ver;
        for(int x = MARGIN*bipaper.width();x<(1-MARGIN)*bipaper.width();x++)
        {
            up = 0,down = 0;
            if(isVisited[x])
            {
                continue;
            }
            int currentVerticalCenter = round(line_k*x+line_b);
            if(currentVerticalCenter<MARGIN*bipaper.height() || currentVerticalCenter > (1-MARGIN) * bipaper.height())
                continue;

            while(up<uplimit || down<downlimit)
            {

                //try to find point up
                if(up<uplimit)
                {
                    //25�����ҵ�
                    if(bipaper(x,currentVerticalCenter-up)==0)
                    {
                        vector<int> coordinate(findPoint(x,currentVerticalCenter-up,isVisited));
                        numbers.push_back(coordinate);
                        break;
                    }
                    up++;
                }
                //try to find point down
                if(down<downlimit)
                {
                    if(bipaper(x,currentVerticalCenter+down)==0)
                    {
                        vector<int> coordinate(findPoint(x,currentVerticalCenter+down,isVisited));
                        numbers.push_back(coordinate);
                        break;
                    }
                    down++;
                }
            }

        }//end ��С���˷�����
        //��ÿ�����ֵ���С��x�����������
        sort(numbers.begin(),numbers.end(),[](const vector<int>& v1,const vector<int>& v2)
             {
                 return v1[0]<v2[0];
             });
        /*��ÿһ���õ��ĵ���д��������⣬Ͷ�䵽28*28�����ϣ�Ȼ��ת����784ά�������д洢*/
        for(int num_index = 0;num_index<numbers.size();num_index++)
        {
            int xmin = numbers[num_index][0],ymin =  numbers[num_index][1];
            int xmax = numbers[num_index][2],ymax =  numbers[num_index][3];
            int deltaX = xmax - xmin;
            int deltaY = ymax - ymin;
            /*ˮƽ��������*/
            ymin -= deltaY/7;
            ymax += deltaY/7;
            //���ǵ�����һ���Ǵ�ֱ���������ģ���˶��ڹ��������֣�����1������ˮƽ����հ����
            if((double)deltaX/deltaY < 0.75)
            {
                int acX = round(deltaY * 0.75);
                int centerX = (xmin+xmax)/2;
                xmin = centerX - acX/2;
                xmax = centerX + (acX-acX/2);
            }
            xmin -= deltaX/7;
            xmax += deltaX/7;
            //ͶӰ��28*28�ռ���
            int digitSize = 28;

            vector<vector<double>> from,to;
            to.push_back(vector<double>{xmin,ymin});
            to.push_back(vector<double>{xmax,ymin});
            to.push_back(vector<double>{xmax,ymax});
            to.push_back(vector<double>{xmin,ymax});

            from.push_back(vector<double>{0,0});
            from.push_back(vector<double>{digitSize,0});
            from.push_back(vector<double>{digitSize,digitSize});
            from.push_back(vector<double>{0,digitSize});
            vector<double> parameter(my_affine_fit(from,to));
            /*
            ��ԭʼͼ�����Ԥ����
            ���ȴ�A4ͼ���õ�ԭʼ��������
                    ʹ�ö�ֵ��ͼ��Ľ������ǰ���ͱ���
                    ǰ��ʹ��255��ȥǰ��ֵ������ȫ��Ϊ0��ֵ������bipaper��
                ��̬ѧ���ͣ�������ȡ���������Сֵ

                ���⣺ִ��˳���ǰ����������ģ������������֮����������ص�����ʹǰ��ɫ�ͱ���ɫ���á�
            */
            for(int dgx = xmin;dgx<=xmax;dgx++)
            {
                for(int dgy = ymin;dgy<=ymax;dgy++)
                {
                    //�Ǳ�Ե��
                    if(bipaper(dgx,dgy)==0)
                    {
                        cachepaper(dgx,dgy) = 255 - a4paper(dgx,dgy);
                        //cachepaper(dgx,dgy) = 255;
                    }
                    else
                    {
                        cachepaper(dgx,dgy) = 0;
                    }
                }
            }

            for(int dgx = xmin;dgx<=xmax;dgx++)
            {
                for(int dgy = ymin;dgy<=ymax;dgy++)
                {
                    int xl = dgx-1<xmin?xmin:dgx-1;
                    int xr = dgx+1>xmax?xmax:dgx+1;
                    int yl = dgy-1<ymin?ymin:dgy-1;
                    int yr = dgy+1>ymax?ymax:dgy+1;

                    int maxNum = cachepaper(dgx,dgy);
                    /*
                    for(int px = xl;px<=xr;px++)
                    {
                        for(int py = yl;py<=yr;py++)
                        {
                            if(cachepaper(px,py)>maxNum)
                            {
                                maxNum = cachepaper(px,py);
                            }
                        }
                    }
                    */
                    anspaper(dgx,dgy) = maxNum;
                }
            }

            CImg<int> digit(digitSize,digitSize,1,1,0);
            vector<double> digitData;
            cimg_forXY(digit,dgx,dgy)
            {
                int aimX = (double)parameter[0] * dgx + parameter[1] *dgy + parameter[2];
                int aimY = (double)parameter[3] * dgx + parameter[4] *dgy + parameter[5];

                if(anspaper(aimX,aimY)>0)
                {
                    digit(dgx,dgy) = anspaper(aimX,aimY) + 100 > 255 ? 255 : anspaper(aimX,aimY) + 100;
                }
                else
                {
                    int maximum = 0;
                    for(int xx = aimX - 1;xx<=aimX+1;xx++)
                    {
                        for(int yy = aimY - 1;yy<=aimY+1;yy++)
                        {
                            if(maximum < anspaper(xx,yy))
                            {
                                maximum = anspaper(xx,yy);
                            }
                        }
                    }
                    digit(dgx,dgy) = maximum;
                }
                digitData.push_back(digit(dgx,dgy));
                fout<<digit(dgx,dgy)<<" ";
            }
            fout<<endl;
#ifdef OUTPUT
            string filename = "line"+to_string(i)+"_num_"+to_string(num_index)+".bmp";
            digit.save(filename.c_str());
#endif
            //int preNumber = bp->predictNum(digitData);
            //cout<<preNumber<<" ";
            digit.clear();
        }
        cout<<endl;

    }//end horizontalLines��ÿһ����ֱ��
#ifdef DEBUG
    anspaper.display();
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
    bool foundPoint = true;
    bool findUp = true,findLeft = true,findRight = true,findDown = true;
    while(foundPoint)
    {
        foundPoint = false;
        int xl = xmin - 3 < 0 ? 0 : xmin-3;
        int xr = xmax + 3 >= bipaper.width() ? bipaper.width()-1 : xmax + 3;
        int yl = ymin - 5 < 0 ? 0 : ymin-5;
        int yr = ymax + 5 >= bipaper.height() ? bipaper.height()-1 : ymax + 5;
        //��һ�����ѯ
        if(findUp)
        {
            //findUp = false;
            for(int x = xl;x<=xr;x++)
            {
                for(int y = yl;y<ymin;y++)
                {
                    if(bipaper(x,y)==0)
                    {
                        updatePoint(xmin,xmax,ymin,ymax,x,y);
                        foundPoint = true;
                        findUp = true;
                    }

                }
            }
        }

        if(findLeft)
        {
            //findLeft = false;
            for(int x = xl;x<xmin;x++)
            {
                for(int y = ymin;y<=ymax;y++)
                {
                    if(bipaper(x,y)==0)
                    {
                        updatePoint(xmin,xmax,ymin,ymax,x,y);
                        foundPoint = true;
                        findLeft = true;
                    }
                }
            }
        }

        if(findRight)
        {
            //findRight = false;
            for(int x = xmax +1;x<=xr;x++)
            {
                for(int y = ymin;y<=ymax;y++)
                {
                    if(bipaper(x,y)==0)
                    {
                        updatePoint(xmin,xmax,ymin,ymax,x,y);
                        foundPoint = true;
                        findRight = true;
                    }
                }
            }
        }

        if(findDown)
        {
            //findDown = false;
            for(int x = xl;x<=xr;x++)
            {
                for(int y = ymax +1;y<=yr;y++)
                {
                    if(bipaper(x,y)==0)
                    {
                        updatePoint(xmin,xmax,ymin,ymax,x,y);
                        foundPoint = true;
                        findDown = true;
                    }
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

void NumberExtract::getBinaryImg()
{
    bipaper = CImg<int>(a4paper.width(),a4paper.height(),1,1,0);
    cimg_forXY(bipaper,x,y)
    {
        if(a4paper(x,y)<BITHRESHOLD)
        {
            bipaper(x,y) = 0;
        }
        else
        {
            bipaper(x,y) = 255;
        }
    }

}
vector<int> NumberExtract::computeVerticalGrayHist()
{
    vector<int> hist;
    int st = (double)MARGIN*bipaper.width();
    int ed = bipaper.width()-st;
    cimg_forY(bipaper,y)
    {
        int sum = 0;
        for(int x = st;x<ed;x++)
        {
            if(bipaper(x,y)==0)
                sum++;
        }
        hist.push_back(sum);
    }

    return hist;
}

vector<int> NumberExtract::computeHorizontalGrayHist(int yl,int yh)
{
    vector<int> hist;
    for(int x = MARGIN*bipaper.width();x<(1-MARGIN)*bipaper.width();x++)
    {
        int sum = 0;
        for(int y = yl;y<=yh;y++)
        {
            if(bipaper(x,y)==0)
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
    int st = (double)MARGIN*hist.size();
    int ed = hist.size()-st;

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