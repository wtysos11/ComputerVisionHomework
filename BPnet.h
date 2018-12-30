#ifndef BPNET
#define BPNET
/*
Ϊ�˷��������ֻ��һ�������BP������
*/
#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
using namespace std;

#define IN_NUM 784 //����784ά����
#define HID_NUM 20 //����20ά����
#define OUT_NUM 10 //���10ά����
#define ALPHA 0.2//ѧϰ��

inline double sigmoid(double x)
{
    return 1/(1+exp(-1*x));
}

//����-1 ~ 1֮������ֵ
inline double getRandom()
{
    return ((2.0*(double)rand()/RAND_MAX) - 1);
}

struct BPnet{
private:
    static BPnet* m_pInstance;
public:
    double dest[OUT_NUM];//Ŀ�����
    double output[OUT_NUM];//���
    double hidden[HID_NUM];
    double input[IN_NUM];

    double V[IN_NUM][HID_NUM];
    double W[HID_NUM][OUT_NUM];

    double etajy[HID_NUM]; //���������������
    double etako[OUT_NUM]; //���������������


    BPnet()
    {
        this->init();
    }

    static BPnet* GetInstance()
    {
        if(m_pInstance == NULL)
            m_pInstance = new BPnet();
        return m_pInstance;
    }

    ~BPnet()
    {
        if(m_pInstance != NULL)
        {
            delete m_pInstance;
        }
    }

    void train(vector<int>& in,int label)
    {
        this->setData(in,label);
//ǰ�򴫲�
        this->forward();
//���򴫲�
        this->backward();
//����Ȩ��
        this->update();
    }

    void init();
    //���������������㣨�ṩ����û�й�һ�������ݣ�
    void setData(vector<int>& in,int label);

    void forward();

    void backward();

    void update();
//�����������Ȩ�ؼ�������㣬�����ʵ���������ϣ��򷵻�true
    int predictNum(vector<int>& data);
};

#endif
