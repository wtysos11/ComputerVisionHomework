import struct,os
from array import array as pyarray
from numpy import append, array, int8, uint8, zeros
import numpy as np
from sklearn.metrics import accuracy_score
from sklearn.neural_network import MLPClassifier
from sklearn.externals import joblib
from time import time
import pandas as pd
def load_mnist(image_file, label_file, path="."):
    digits=np.arange(10)

    fname_image = os.path.join(path, image_file)
    fname_label = os.path.join(path, label_file)

    flbl = open(fname_label, 'rb')
    magic_nr, size = struct.unpack(">II", flbl.read(8))
    lbl = pyarray("b", flbl.read())
    flbl.close()

    fimg = open(fname_image, 'rb')
    magic_nr, size, rows, cols = struct.unpack(">IIII", fimg.read(16))
    img = pyarray("B", fimg.read())
    fimg.close()

    ind = [ k for k in range(size) if lbl[k] in digits ]
    N = len(ind)

    images = zeros((N, rows*cols), dtype=uint8)
    labels = zeros(N, dtype=int8)
    for i in range(len(ind)):
        images[i] = array(img[ ind[i]*rows*cols : (ind[i]+1)*rows*cols ]).reshape((1, rows*cols))
        labels[i] = lbl[ind[i]]
    images = [im/255.0 for im in images]

    return images, labels

if __name__=="__main__":
    train_image, train_label = load_mnist("train-images.idx3-ubyte", "train-labels.idx1-ubyte")
    test_image, test_label = load_mnist("t10k-images.idx3-ubyte", "t10k-labels.idx1-ubyte")
    
    mlp = MLPClassifier(hidden_layer_sizes=(400, 200), activation='logistic', 
    				solver='sgd', learning_rate_init=0.001, max_iter=400, verbose = True)
    save_dir = './model/mlp.txt'
    if(os.path.isfile(save_dir)):
        #load model
        mlp = joblib.load(save_dir)
    else:
        t1 = time()
        mlp.fit(train_image, train_label)
        t2 = time()
        print("training time",t2-t1)
        joblib.dump(mlp, save_dir)
        mlp = joblib.load(save_dir)
        y_pred = list(mlp.predict(test_image))
        print(accuracy_score(test_label, y_pred)) # 测试集的得分
    
    testFile = pd.read_table("imageOut",header=None)
    data = testFile[0].values
    stuid = ''
    telphone = ''
    entityid = ''
    for dataStr in data:
        arr = dataStr.split(' ')
        del arr[784]
        numList = []
        for number in arr:
            acNum = int(number)
            acNum = acNum/255.0
            numList.append(acNum)
        numList = np.array(numList)
        numList = numList.reshape(1,784)
        res = int(mlp.predict(numList.copy()))
        if len(stuid) < 8:
            stuid += str(res)
        elif len(telphone) < 11:
            telphone += str(res)
        elif len(entityid) < 18:
            entityid += str(res)
        else:
            print(stuid)
            print(telphone)
            print(entityid)
            stuid = ''
            telphone = ''
            entityid = ''
    
    print(stuid)
    print(telphone)
    print(entityid)
