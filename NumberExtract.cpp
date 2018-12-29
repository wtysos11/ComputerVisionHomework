#include "NumberExtract.h"
void NumberExtract::compute()
{
    //处理，得到二值化图像
    getBinaryImg();
    //提取字符行

    vector<int> horiLines(getVerticallines());
    #ifdef DEBUG

    cout<<"vertical lines"<<endl;
    for(int i = 0;i<VERTICAL_NUM;i++)
    {
        cout<<horiLines[i]<<endl;
    }
    bipaper.display();
    #endif // DEBUG
    //对于每一个提取的字符行进行操作
    /*
    对于每一个字符行的顶，设其为字符行中心线。
        设立搜索区间，区间由相邻的两条中心线决定（中点）
        为了应对打斜写的情况，从两条中心线的交点开始向四周进行搜索，搜索为25邻域，找到的连通分量视为一个数字
            对于每个数字，根据其水平距离与垂直距离的比例进行留空（应对如1这样的情况）
            保存结果为784维向量
    */
    for(int i = 0;i<horiLines.size();i++)
    {
        int st,ed;
        if(i==0)//第一个点
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

        //在搜索区间中进行灰度直方图统计
        //vector<int> hist(computeHorizontalGrayHist(st,ed));
        //统计出开始点和结束点
        vector<bool> isVisited(bipaper.width(),false);
        int up = 0,down = 0;
        int ver = horiLines[i];
        vector<vector<int>> numbers;
        for(int x = 0;x<bipaper.width();x++)
        {
            if(isVisited[x])
                continue;
            //尝试搜索
            while(ver-up>st || ver+down<ed)
            {
                //try to find point up
                if(ver-up>st)
                {
                    //8邻域找点
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
        /*对每一个得到的点进行处理，按扁，投射到28*28矩阵上，然后转换成784维向量进行存储*/


    }

}

//25邻域找点，找到的点的左上角的x和y坐标和右下角的x和y坐标返回
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
    对图像进行垂直方向上灰度直方图统计
    截顶，对于每一个顶，获得平均值并存入到一个数组中
    这样就得到的顶的数组
*/
vector<int> NumberExtract::getVerticallines()
{
    vector<int> hist(computeVerticalGrayHist());
    vector<int> ans;
    vector<pair<int,int>> cache;

    //边缘由于A4纸矫正算法的问题，可能会留下黑边
    int st = (double)MARGIN*hist.size();
    int ed = hist.size()-st;

    int vertical_diff = 0;//两条中心线之间的距离如果小于vertical_diff，则进行合并
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


    //进行边缘聚合，然后找到最大的k个顶点，k=9
    /*
    首先将hist的值都存放在vector中，对权重进行排序，然后往后找，不断地进行合并。直到不能合并且找到第k个点为止

    设计vector<int,int> value,weight
    进行降序排序
    从下到上进行合并操作
    加上vector<int> num进行计数
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

    vector<int> num;//ranking位置的数字之和
    int counting = 0;//counting指向的是ranking中的当前待考察对象
    while(counting<VERTICAL_NUM)
    {
        /*
            首先判断是否存在重复
                如果存在重复，则修改重复项目的num（次数），并更新ranking（求和）
                如果不存在重复，则指针移动
        */
        //判断是否有重复
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
cout<<"存在重复"<<endl;
cout<<ranking[repeatNum].first/num[repeatNum]<<"与"<<ranking[counting].first<<"重复"<<endl;
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
cout<<ranking[counting].first<<"没有重复"<<endl;
#endif // DEBUG
            num.push_back(1);
            counting++;

        }

    }

    //得到中心线结果
    for(int i = 0;i<VERTICAL_NUM;i++)
    {
        ans.push_back(ranking[i].first/num[i]);
    }
    sort(ans.begin(),ans.end());
    return ans;
}
