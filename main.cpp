#include "CImg.h"
#include "Canny.h"
#include "Process.h"
#include "Solution.h"
#include "BPnet.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#define TRAINING_TIMES 3
#define TRAIN_NUM 60000 //MNISTѵ������60000��
using namespace std;
using namespace cimg_library;

int main(void)
{
    /*
    //ѵ��BP������
    BPnet* bp = BPnet::GetInstance();
    srand((unsigned)time(NULL));
//��ȡѵ������
    ifstream tr_image("train_image"),tr_label("train_label");
    vector<vector<int>> train_image;
    vector<int> train_label;
    for(int i = 0;i<TRAIN_NUM;i++)
    {
        vector<int> cache;
        for(int j = 0;j<784;j++)
        {
            int x;
            tr_image>>x;
            cache.push_back(x);
        }
        train_image.push_back(cache);
        int y;
        tr_label>>y;
        train_label.push_back(y);
    }
    tr_image.close();
    tr_label.close();
    cout<<"BPnet read file over"<<endl;
    int times = TRAINING_TIMES;
    for(int n = 0;n<times;n++)
    {
//���룬��ʼ��BPnet����������׼���
        for(int m = 0;m<TRAIN_NUM;m++)
        {
            bp->train(train_image[m],train_label[m]);
        }
    }
    cout<<"BPnet training over"<<endl;
*/
/*
    for(int i = 1;i<=10;i++)
    {
        Solution s("test" + to_string(i)+".bmp");
        s.mainProcess();
        s.clear();
    }
*/
    Solution s("test10.bmp");
    s.mainProcess();
    s.clear();
    return 0;
}
