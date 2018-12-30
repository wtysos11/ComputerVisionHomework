
#include "BPnet.h"
BPnet* BPnet::m_pInstance = NULL;
void BPnet::init()
{
    memset(dest,0,sizeof(dest));
    memset(output,0,sizeof(output));
    memset(hidden,0,sizeof(hidden));
    memset(input,0,sizeof(input));
    memset(etajy,0,sizeof(etajy));
    memset(etako,0,sizeof(etako));

    for(int i = 0;i<IN_NUM;i++)
    {
        for(int j = 0;j<HID_NUM;j++)
        {
            V[i][j] = getRandom();
        }
    }

    for(int j = 0;j<HID_NUM;j++)
    {
        for(int k = 0 ;k<OUT_NUM;k++)
        {
            W[j][k] = getRandom();
        }
    }
}
//���������������㣨�ṩ����û�й�һ�������ݣ�
void BPnet::setData(vector<int>& in,int label)
{
    memset(dest,0.0,sizeof(dest));
    dest[label] = 1.0;
    for(int i = 0;i<IN_NUM;i++)
    {
        input[i] = in[i];
        input[i] = (input[i]-128.0)/128.0;
    }
}
void BPnet::forward()
{
    //����㵽����
    for(int j = 0;j<HID_NUM;j++)
    {
        double sum = 0;
        for(int i = 0;i<IN_NUM;i++)
        {
            sum += input[i]*V[i][j];
        }
        hidden[j] = sigmoid(sum);
    }

    //���㵽�����
    for(int k = 0;k<OUT_NUM;k++)
    {
        double sum = 0;
        for(int j = 0;j<HID_NUM;j++)
        {
            sum += hidden[j] * W[j][k];
        }
        output[k] = sigmoid(sum);
    }

}
void BPnet::backward()
{
    //�������㵽���������
    for(int k = 0;k<OUT_NUM;k++)
    {
        etako[k] = (dest[k] - output[k])*output[k]*(1-output[k]);
    }
    //��������㵽��������
    for(int j = 0;j<HID_NUM;j++)
    {
        double sum = 0;
        for(int k = 0;k<OUT_NUM;k++)
        {
            sum += etako[k]*W[j][k];
        }
        etajy[j] = sum * hidden[j] * (1-hidden[j]);
    }
}
void BPnet::update()
{
    //�������㵽���������
    for(int j = 0;j<HID_NUM;j++)
    {
        for(int k = 0;k<OUT_NUM;k++)
        {
            W[j][k] += ALPHA * etako[k] * hidden[j];
        }
    }
    //���´�����㵽��������
    for(int i = 0;i<IN_NUM;i++)
    {
        for(int j = 0;j<HID_NUM;j++)
        {
            V[i][j] += ALPHA * etajy[j] * input[i];
        }
    }
}

//���������784ά�������Ԥ���ǩ
int BPnet::predictNum(vector<int>& data)
{
    memset(dest,0.0,sizeof(dest));
    for(int i = 0;i<IN_NUM;i++)
    {
        input[i] = data[i];
        input[i] = (input[i]-128.0)/128.0;
    }
    this->forward();
    double maxP = -1.0;
    int label = 0;
    for(int k = 0 ;k<OUT_NUM;k++)
    {
        if(maxP<output[k])
        {
            maxP = output[k];
            label = k;
        }
    }
    return label;
}
