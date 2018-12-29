#include "NumberExtract.h"
void NumberExtract::compute()
{
    //处理，得到二值化图像
    getBinaryImg();
    //提取字符行

    vector<int> horiLines(getVerticallines());

    //对于每一个提取的字符行进行操作
    /*
    对于每一个字符行的顶，设其为字符行中心线。
        设立搜索区间，区间点为相近字符行的顶的中点
        在搜索区间中进行水平方向上灰度直方图统计，同样截顶，获得若干个顶
        为了应对打斜写的情况，从两条中心线的交点开始向四周进行搜索，搜索为25邻域，找到的连通分量视为一个数字
            对于每个数字，根据其水平距离与垂直距离的比例进行留空（应对如1这样的情况）
            保存结果为784维向量
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
    对图像进行垂直方向上灰度直方图统计
    截顶，对于每一个顶，获得平均值并存入到一个数组中
    这样就得到的顶的数组
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

    //边缘由于A4纸矫正算法的问题，可能会留下黑边
    int st = (double)MARGIN*hist.size();
    int ed = hist.size()-st;
    //进行边缘聚合，然后找到最大的k个顶点，k=9
    /*
    首先将hist的值都存放在vector中，对权重进行排序，然后往后找，不断地进行合并。直到不能合并且找到第k个点为止

    设计vector<int,int> value,weight
    进行降序排序
    从下到上进行合并操作
    加上vector<int> num进行计数
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
