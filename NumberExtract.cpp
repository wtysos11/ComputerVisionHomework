#include "NumberExtract.h"
void NumberExtract::compute()
{
    //�����õ���ֵ��ͼ��
    getBinaryImg();
    //��ȡ�ַ���

    vector<int> horiLines(getVerticallines());
    #ifdef DEBUG

    cout<<"vertical lines"<<endl;
    for(int i = 0;i<VERTICAL_NUM;i++)
    {
        cout<<horiLines[i]<<endl;
    }
    bipaper.display();
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
        int st,ed;
        if(i==0)//��һ����
        {
            int delta = (horiLines[i+1] - horiLines[i])/2;
            st = horiLines[i]-delta>0?horiLines[i]-delta:0;
            ed = horiLines[i]+delta;
        }
        else if(i==horiLines.size()-1)
        {
            int delta = (horiLines[i]-horiLines[i-1])/2;
            st = horiLines[i]-delta;
            ed = horiLines[i]+delta>=horiLines.size()?horiLines.size()-1:horiLines[i]+delta;
        }
        else
        {
            st = horiLines[i] - (horiLines[i]-horiLines[i-1])/2;
            ed = horiLines[i] + (horiLines[i+1]-horiLines[i])/2;
        }

        //�����������н��лҶ�ֱ��ͼͳ��
        //vector<int> hist(computeHorizontalGrayHist(st,ed));
        //ͳ�Ƴ���ʼ��ͽ�����
        vector<bool> isVisited(bipaper.width(),false);
        int up = 0,down = 0;
        int ver = horiLines[i];
        vector<vector<int>> numbers;
        for(int x = 0;x<bipaper.width();x++)
        {
            if(isVisited[x])
                continue;
            //��������
            while(ver-up>st || ver+down<ed)
            {
                //try to find point up
                if(ver-up>st)
                {
                    //8�����ҵ�
                    if(bipaper(x,ver-up)==0)
                    {
                        vector<int> cooridinate(findPoint(x,ver-up,isVisited));
                        numbers.push_back(cooridinate);
                    }
                    up++;
                }
                //try to find point down
                if(ver+down<ed)
                {
                    if(bipaper(x,ver+down)==0)
                    {
                        vector<int> cooridinate(findPoint(x,ver+down,isVisited));
                        numbers.push_back(cooridinate);
                    }
                    down++;
                }
            }
        }
        /*��ÿһ���õ��ĵ���д������⣬Ͷ�䵽28*28�����ϣ�Ȼ��ת����784ά�������д洢*/


    }

}

//25�����ҵ㣬�ҵ��ĵ�����Ͻǵ�x��y��������½ǵ�x��y���귵��
vector<int> NumberExtract::findPoint(int x,int y,vector<bool>& isVisited)
{

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
#ifdef DEBUG
cout<<"�����ظ�"<<endl;
cout<<ranking[repeatNum].first/num[repeatNum]<<"��"<<ranking[counting].first<<"�ظ�"<<endl;
#endif // DEBUG
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
#ifdef DEBUG
cout<<ranking[counting].first<<"û���ظ�"<<endl;
#endif // DEBUG
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
