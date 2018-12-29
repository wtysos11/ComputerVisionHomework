#include "NumberExtract.h"
void NumberExtract::compute()
{
    //�����õ���ֵ��ͼ��
    getBinaryImg();
    //��ȡ�ַ���

    vector<int> horiLines(getVerticallines());

    //����ÿһ����ȡ���ַ��н��в���
    /*
    ����ÿһ���ַ��еĶ�������Ϊ�ַ��������ߡ�
        �����������䣬�����Ϊ����ַ��еĶ����е�
        �����������н���ˮƽ�����ϻҶ�ֱ��ͼͳ�ƣ�ͬ���ض���������ɸ���
        Ϊ��Ӧ�Դ�бд������������������ߵĽ��㿪ʼ�����ܽ�������������Ϊ25�����ҵ�����ͨ������Ϊһ������
            ����ÿ�����֣�������ˮƽ�����봹ֱ����ı����������գ�Ӧ����1�����������
            ������Ϊ784ά����
    */
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
    cimg_forX(bipaper,x)
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
    vector<int> histOrigin(computeVerticalGrayHist());
    vector<int> hist(histOrigin);
    vector<int> ans;
    vector<pair<int,int>> cache;

    for(int i = 0;i<histOrigin.size();i++)
    {
        int l = i-2 < 0 ? 0 : i -2;
        int r = i+2 >= histOrigin.size() ? histOrigin.size()-1 : i+2;
        int maximum = 0;
        for(int j = l;j<=r;j++)
        {
            if(maximum < histOrigin[j])
            {
                maximum = histOrigin[j];
            }
        }
        hist[i] = maximum;
    }

    //��Ե����A4ֽ�����㷨�����⣬���ܻ����ºڱ�
    int st = (double)MARGIN*hist.size();
    int ed = hist.size()-st;
    //���б�Ե�ۺϣ�Ȼ���ҵ�����k�����㣬k=9
    /*
    ���Ƚ�hist��ֵ�������vector�У���Ȩ�ؽ�������Ȼ�������ң����ϵؽ��кϲ���ֱ�����ܺϲ����ҵ���k����Ϊֹ

    ���vector<int,int> value,weight
    ���н�������
    ���µ��Ͻ��кϲ�����
    ����vector<int> num���м���
    */
    bool flag = false;
    int startPoint = 0;
    int endPoint = 0;
    for(int i = st;i<ed;i++)
    {
        if(!flag && hist[i]>VERTICAL_GRAY_THRESHOLD)
        {
            flag = true;
            if(i-endPoint>VERTICAL_DIFF)
                startPoint = i;
            else if(i-endPoint<VERTICAL_DIFF && ans.size()>0)
            {
                ans.pop_back();
                cache.pop_back();
                cout<<"Continue"<<endl;
            }
        }
        else if(flag && hist[i]<VERTICAL_GRAY_THRESHOLD)
        {
            flag = false;
            endPoint = i;
            ans.push_back((startPoint+endPoint)/2);
            cache.push_back(make_pair(startPoint,endPoint));
            cout<<startPoint<<"\t"<<endPoint<<endl;
        }
    }

#ifdef DEBUG
    for(int i = 0;i<hist.size();i++)
    {
        cout<<i<<"\t"<<hist[i]<<endl;
    }
    cout<<"vertical center lines"<<endl;
    for(int i = 0;i<cache.size();i++)
    {
        cout<<i<<"\t"<<cache[i].first<<"\t"<<cache[i].second<<endl;
    }
    bipaper.display();
#endif // DEBUG

    return ans;
}
