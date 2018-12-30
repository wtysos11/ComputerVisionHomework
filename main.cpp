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
#define TRAIN_NUM 60000 //MNIST训练集有60000个
using namespace std;
using namespace cimg_library;

int main(void)
{
    //训练BP神经网络
/*
    BPnet* bp = BPnet::GetInstance();
    srand((unsigned)time(NULL));
//读取训练数据
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
//读入，初始化BPnet的输入层与标准输出
        for(int m = 0;m<TRAIN_NUM;m++)
        {
            bp->train(train_image[m],train_label[m]);
        }
    }
    cout<<"BPnet training over"<<endl;
*/
    Solution s("test.bmp");
    s.mainProcess();
    s.clear();
    return 0;
}
