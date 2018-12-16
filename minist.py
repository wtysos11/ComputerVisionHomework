#-.-coding:utf-8-.-
#########################################
#Created by Eric.MC
#September 14th
#HandWriten Recognition by SVM & AdaBoost
#########################################
import numpy as np
import pandas as pd
from sklearn import svm
import matplotlib.colors
import matplotlib.pyplot as plt
from sklearn.metrics import accuracy_score
from sklearn.model_selection import GridSearchCV
from time import time
from sklearn.ensemble import AdaBoostClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier

if __name__ == "__main__":
    #load trainning dataset
    data = pd.read_csv('optdigits.tra', header=None)#使用pandas载入训练样本
    x, y = data[list(range(64))].values, data[64].values#划分训练集的输入与输出
    images_tra = x.reshape(-1, 8, 8)#其实每一个8x8的小矩阵表示一个数字，为下面将错误样本保存成图片做准备
    y_tra = y.ravel().astype(np.int)#将数组变为1维数组

    #load test dataset
    datatest = np.loadtxt('out.txt',dtype=np.float, delimiter=',')#其实这个地方也可以采用pandas读入数据
    x_test, y_test = np.split(datatest, (-1,), axis=1)#划分测试样本集的输入与输出
    print(y_test.shape)
    images_tes = x_test.reshape(-1, 8, 8)#同上
    y_test = y_test.ravel().astype(np.int)
    print(images_tes.shape)
    # print tmp
    # print num

    clf = AdaBoostClassifier(DecisionTreeClassifier(max_depth=5, min_samples_split=5, min_samples_leaf=5), n_estimators=200, learning_rate=0.05, algorithm='SAMME.R')
    t1 = time()
    clf.fit(x, y_tra)
    t2 = time()
    t = t2 - t1
    print('AdaBoost-DT训练模型耗时：%d分%.3f秒' %  ((int)(t/60), t-60*(int)(t/60)))
    y_hat1 = clf.predict(x_test)
    print('AdaBoost-DT训练集准确率：', accuracy_score(y_tra, clf.predict(x)))
    print('AdaBoost-DT测试集准确率：', accuracy_score(y_test, y_hat1))
    print('*************************')
