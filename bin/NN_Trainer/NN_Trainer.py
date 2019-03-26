from __future__ import absolute_import, division, print_function

from keras.layers import Lambda, Input, Dense, Conv2D, MaxPooling2D, UpSampling2D, Reshape
from keras.models import Model
from keras.datasets import mnist
from keras.losses import mse, binary_crossentropy
from keras.utils import plot_model
from keras import backend as K
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers

import pathlib

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

import numpy as np
import matplotlib.pyplot as plt
import argparse
import os
import random
import json

import importlib
import util

# ------------------------------------------------------------------
# 0. config

csvNum = 200

prefix = "data"
postfix = ".csv"
dataDim = 21
inputDim = 17
outputDim = 3
trainRate = 0.8
hiddenUnit0 = 20
hiddenUnit1 = 10
expandRate = 0.5
epoch = 99999999
initBatchSize = 128
initPatience = 6
minRRMSE = 0.05
minSampleNum = 10000

# ------------------------------------------------------------------
# 1. Load Data

allData = []

for i in range(csvNum):
    path = "datas/" + prefix + str(i) + postfix
    try:
        data = np.loadtxt(path, delimiter =",", skiprows=1)
    except:
        print("load data{0}.csv fail".format(i))
        continue
    else:
        for row in data:
            allData.append(row)

    print("load data{0}.csv done".format(i))

allData = np.array(allData)
print("sample num {0}".format(allData.shape[0]))

# ------------------------------------------------------------------
# 2. spilt data by ID

id2rows = {}

for i in range(len(allData)):
    id = int(allData[i, 0])
    if id in id2rows:
        id2rows[id].append(i)
    else:
        id2rows[id] = [i]

IDs = id2rows.keys()
print(IDs)

id2rawData = {}
for ID in IDs:
    rawDataOfID = allData[id2rows[ID]][:, range(1,dataDim)]
    id2rawData[ID] = rawDataOfID

for ID in IDs:
    print("ID: {0}, raw data shape: {1}".format(ID,id2rawData[ID].shape))

# ------------------------------------------------------------------
# 3. 将数据映射到 unit hyperbox 中

def ScaleToUnitHyperbox(data):
    """
    将矩阵映射到单位超盒 Unit Hyperbox 中

    @param
        data: 矩阵

    @return
        scaledData: 单位超盒中的矩阵
        minVal: 矩阵各列向量的最小值
        extent: 矩阵各列向量的范围，extent = maxVal - minVal
        nonZeroExtntAxis: 非0范围的轴
    """

    dim = data.shape[1]

    minVal = np.min(data, axis = 0)
    maxVal = np.max(data, axis = 0)

    extent = maxVal - minVal

    nonZeroExtntAxis = []
    for i in range(dim):
        if extent[i] != 0:
            nonZeroExtntAxis.append(i)

    extent = [x if x != 0 else 1 for x in extent]

    scaledData = (data - minVal) / extent

    return scaledData, minVal, extent, nonZeroExtntAxis

id2data = {}

for ID in IDs:
    id2data[ID] = {}
    id2data[ID]['rawData'] = id2rawData[ID]
    scaledData, minVal, extent, nonZeroExtntAxis = ScaleToUnitHyperbox(id2data[ID]['rawData'])
    id2data[ID]['spiltableAxis'] = []
    for i in range(inputDim):
        if i in nonZeroExtntAxis:
            id2data[ID]['spiltableAxis'].append(i)

    id2data[ID]['data'] = scaledData
    id2data[ID]['minVal'] = minVal
    id2data[ID]['extent'] = extent

for ID in IDs:
    print("ID: {0}".format(ID))
    print("min: {0}".format(np.min(id2data[ID]['data'], axis=0)))
    print("max: {0}".format(np.max(id2data[ID]['data'], axis=0)))
    print("spiltable axis: {0}".format(id2data[ID]['spiltableAxis']))

# ------------------------------------------------------------------
# 4. 训练

def DivideData(data):
    sampleNum = data.shape[0]
    dataDim = data.shape[1]

    if dataDim != inputDim + outputDim:
        raise RuntimeError("data's dim != inputDim + outputDim")

    if trainRate < 0 or trainRate > 1:
        raise RuntimeError("trainRate < 0 or trainRate > 1")

    trainNum = int(sampleNum * trainRate)
    randomNums = [x for x in range(sampleNum)]
    random.shuffle(randomNums)
    testNum = sampleNum - trainNum
    trainRow = randomNums[0:trainNum]
    testRow = randomNums[trainNum:sampleNum]

    trainData = data[trainRow]
    testData = data[testRow]

    trainX = trainData[:, range(inputDim)]
    trainY = trainData[:, range(inputDim,inputDim+outputDim)]

    testX = testData[:, range(inputDim)]
    testY = testData[:, range(inputDim,inputDim+outputDim)]

    return trainX, trainY, testX, testY

def TrainModel(trainX, trainY, batchSize, patience):
    print("training model ...")

    model = keras.Sequential([
            layers.Dense(hiddenUnit0, activation=tf.nn.tanh, input_shape=(inputDim,), name='dense0'),
            layers.Dense(hiddenUnit1, activation=tf.nn.tanh, name='dense1'),
            layers.Dense(outputDim, name='dense2')
          ])

    #model.summary()

    model.compile(optimizer=tf.keras.optimizers.Adam(),
                      loss ='mse',
                      metrics=['mae', 'mse'])

    # The patience parameter is the amount of epochs to check for improvement
    early_stop = keras.callbacks.EarlyStopping(monitor = 'val_loss', patience = patience)

    model.fit(trainX, trainY, epochs = epoch, batch_size = batchSize,
                            validation_split = 0.2, callbacks=[early_stop], verbose = 0)
    
    print("training model complete")

    return model

def EvaluateRRMSE(model, testX, testY, minVal_Y, extent_Y):
    rst = model.predict(testX)

    realTestY = extent_Y * testY + minVal_Y
    realRstY = extent_Y * rst + minVal_Y

    error = realTestY - realRstY
    rmse = np.sqrt(np.mean(np.square(error)))
    rrmse = rmse / np.mean(realTestY)

    return rrmse

def TrainKDTreeNode(objID, data, minVal_Y, extent_Y, hyperbox, batchSize, patience):
    expendedHyperbox = hyperbox.GenExpandedHyperbox(expandRate)
    dataInBox = expendedHyperbox.SelectData(data)

    # check sample num
    sampleNum = dataInBox.shape[0]
    if sampleNum < 6030:
        print("sampleNum({0}) < 6030, not enough".format(sampleNum))
        return None, 9999, sampleNum
    else:
        print("sampleNum: {0}".format(sampleNum))

    # train model
    trainX, trainY, testX, testY = DivideData(dataInBox)
    model = TrainModel(trainX, trainY, batchSize, patience)

    # evaluate rrmse
    rrmse = EvaluateRRMSE(model, testX, testY, minVal_Y, extent_Y)
    print("rrmse: {0}".format(rrmse))

    treeNode = util.ModelKDTree(objID, rrmse, model)

    return treeNode, rrmse, sampleNum, testX, testY

def TrainKDTreeForNode(objID, data, minVal_Y, extent_Y, treeNode, sampleNum, hyperbox, batchSize, patience):
    # spilt
    leftNode = None
    rightNode = None
    spiltAxis = -1
    spiltVal = -1

    if sampleNum > 2 * minSampleNum:
        print("spilting ...")
        spiltableAxis = hyperbox.GetSpiltableAxis()
        random.shuffle(spiltableAxis)

        for axis in spiltableAxis:
            leftBox, rightBox, spiltVal = hyperbox.GenSpiltHyperbox(axis)
            print("[ Try ]")
            print("axis: {0}, spilt value: {1}".format(axis, spiltVal))
            print("left box:\n{0}".format(leftBox))
            print("right box:\n{0}".format(rightBox))
            
            exLeftBox = leftBox.GenExpandedHyperbox(expandRate)
            leftNum = exLeftBox.SelectData(data).shape[0]

            if leftNum < minSampleNum:
                print("[ try again ], because data in left box {0} is not enough {1}".format(leftNum, minSampleNum))
                continue
            else:
                print("data in left box is enough")
            
            exRightBox = rightBox.GenExpandedHyperbox(expandRate)
            rightNum = exRightBox.SelectData(data).shape[0]

            if rightNum < minSampleNum:
                print("[ try again ], because data in right box {0} is not enough {1}".format(rightNum, minSampleNum))
                continue
            else:
                print("data in right box is enough")
            
            # get first
            if leftNum < rightNum:
                print("[ left ] first, because leftNum {0} < rightNum {1}".format(leftNum,rightNum))
                isLeftFirst = True
            else:
                print("[ right ] first, because rightNum {0} <= leftNum {1}".format(rightNum, leftNum))
                isLeftFirst = False

            # train left and right node
            if isLeftFirst:
                print("train [ left node ]")
                leftNode, leftRRMSE, leftSampleNum, leftTestX, leftTestY = TrainKDTreeNode(objID,data,minVal_Y,extent_Y,leftBox,batchSize*2,patience+2)
                parentRRMSEforLeft = EvaluateRRMSE(treeNode.model, leftTestX, leftTestY, minVal_Y, extent_Y)
                if leftRRMSE > parentRRMSEforLeft:
                    print("[ try again ], because leftRRMSE({0}) > parentRRMSEforLeft({1})".format(leftRRMSE, parentRRMSEforLeft))
                    leftNode = None
                    rightNode = None
                    spiltAxis = -1
                    spiltVal = -1
                    continue
                else:
                    print("train left node [ success ], because leftRRMSE({0}) <= parentRRMSEforLeft({1})".format(leftRRMSE, parentRRMSEforLeft))
            
            print("train [ right node ]")
            rightNode, rightRRMSE, rightSampleNum, rightTestX, rightTestY  = TrainKDTreeNode(objID,data,minVal_Y,extent_Y,rightBox,batchSize*2,patience+2)
            parentRRMSEforRight = EvaluateRRMSE(treeNode.model, rightTestX, rightTestY, minVal_Y, extent_Y)
            if rightRRMSE > parentRRMSEforRight:
                leftNode = None
                rightNode = None
                spiltAxis = -1
                spiltVal = -1
                print("[ try again ], because rightRRMSE({0}) > parentRRMSEforRight({1}), ".format(rightRRMSE, parentRRMSEforRight))
                continue
            else:
                print("train right node [ success ], because rightRRMSE({0}) <= parentRRMSEforRight({1})".format(rightRRMSE, parentRRMSEforRight))

            if not isLeftFirst:
                print("train [ left node ]")
                leftNode, leftRRMSE, leftSampleNum, leftTestX, leftTestY = TrainKDTreeNode(objID,data,minVal_Y,extent_Y,leftBox,batchSize*2,patience+2)
                parentRRMSEforLeft = EvaluateRRMSE(treeNode.model, leftTestX, leftTestY, minVal_Y, extent_Y)
                if leftRRMSE > parentRRMSEforLeft:
                    print("[ try again ], because leftRRMSE({0}) > parentRRMSEforLeft({1})".format(leftRRMSE, parentRRMSEforLeft))
                    leftNode = None
                    rightNode = None
                    spiltAxis = -1
                    spiltVal = -1
                    continue
                else:
                    print("train left node [ success ], because leftRRMSE({0}) <= parentRRMSEforLeft({1})".format(leftRRMSE, parentRRMSEforLeft))

            print("[ spilt success ]")

            # train child
            print("train [ left tree ]")
            TrainKDTreeForNode(objID, data, minVal_Y, extent_Y, leftNode, leftSampleNum, leftBox, batchSize*2, patience+1)

            print("train [ right tree ]")
            TrainKDTreeForNode(objID, data, minVal_Y, extent_Y, rightNode, rightSampleNum, rightBox, batchSize*2, patience+1)

            print("train complete")
            spiltAxis = axis
            break
    else:
        print("[ not spilt ], because sampleNum({0}) < 2 * minSampleNum({1})".format(sampleNum, minSampleNum))

    # clear model
    if leftNode != None and rightNode != None:
        treeNode.model = None

    # record rst
    treeNode.spiltAxis = spiltAxis
    treeNode.spiltVal = spiltVal

    treeNode.left = leftNode
    if leftNode != None:
        leftNode.parent = treeNode

    treeNode.right = rightNode
    if rightNode != None:
        rightNode.parent = treeNode

    return

def TrainKDTree(objID, data, minVal_Y, extent_Y, hyperbox, batchSize, patience):
    treeNode, rrmse, sampleNum, testX, testY = TrainKDTreeNode(objID, data, minVal_Y, extent_Y, hyperbox, batchSize, patience)
    if treeNode == None:
        print("train fail, treeNode == None")
        return None

    TrainKDTreeForNode(objID, data, minVal_Y, extent_Y, treeNode, sampleNum, hyperbox, batchSize, patience)
    
    return treeNode

# ------------------------------------------------------------------
# 5. 保存

# IDs
#
#  3 : Cube
# 12 : wall back
# 13 : wall ceil
# 14 : wall down
# 15 : wall left
# 16 : wall right


ID = 14
hyperbox = util.Hyperbox(inputDim, id2data[ID]['spiltableAxis'])
data = id2data[ID]['data']
minVal_Y = id2data[ID]['minVal'][inputDim:inputDim+outputDim]
extent_Y = id2data[ID]['extent'][inputDim:inputDim+outputDim]
tree = TrainKDTree(ID, data, minVal_Y, extent_Y, hyperbox, initBatchSize, initPatience)

treeDict = tree.ToDict()
treeDictJson = json.dumps(treeDict, sort_keys=False, indent=4, separators=(',', ': '))
print(treeDictJson)
fh = open(str(ID)+"_KDTree.json", 'w', encoding='utf-8')
fh.write(treeDictJson)
fh.close()

minValAndExtent = np.row_stack((id2data[ID]['minVal'], id2data[ID]['extent']))
np.savetxt(str(ID) + "_minVal_and_extent.csv", minValAndExtent, delimiter=',')

tree.SaveModel()
