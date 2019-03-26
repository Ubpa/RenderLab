import numpy as np
import random

class Hyperbox:
    def __init__(self, inputDim, spiltableAxis = None):
        if inputDim <= 0:
            raise RuntimeError("inputdDim <= 0")

        self.__box = np.zeros((inputDim, 2))

        if spiltableAxis == None:
            spiltableAxis = [x for x in range(inputDim)]

        self.__spiltableAxis = spiltableAxis

        for axis in self.__spiltableAxis:
            self.__box[axis][1] = 1

    def GetSpiltableAxis(self):
        """
        获取可划分的轴

        @return
            spitableAxis: 可划分的轴
        """
        return self.__spiltableAxis

    def GetDim(self):
        """
        获取维度
        
        @return
            dim: 维度
        """
        return self.__box.shape[0]

    def GetUpperBound(self):
        """
        获取 upper bound
        
        @return
            upperBound: box 的上界
        """
        return self.__box[:, 1]

    def GetLowerBound(self):
        """
        获取 lower bound

        @return
            lowerBound: box 的下界
        """
        return self.__box[:, 0]

    def __str__(self):
        return "{0}".format(self.__box)

    def GetExtent(self):
        """
        获取范围

        @return
            extent: 范围
        """
        return self.GetUpperBound() - self.GetLowerBound()

    def GenCopy(self):
        """
        深拷贝

        @return
            newBox: 生成的拷贝对象
        """
        newBox = Hyperbox(self.GetDim(), self.__spiltableAxis)
        newBox.__box = np.copy(self.__box)

        return newBox

    def GenSpiltHyperbox(self, spiltAxis):
        """
        按轴 spiltAxis 分割超盒

        @param
            spiltAxis: 分割轴，要求 spiltAxis in self.GetSpiltableAxis()

        @return
            leftHyperbox: 左超盒
            rightHyperbox: 右超盒
            spiltVal: 分割值
        """

        if spiltAxis not in self.GetSpiltableAxis():
            raise RuntimeError("spiltAxis not in self.GetSpiltableAxis()")

        hyperbox = self.__box

        spiltVal = (hyperbox[spiltAxis][1] - hyperbox[spiltAxis][0]) / 2
        leftHyperbox = self.GenCopy()
        rightHyperbox = self.GenCopy()
        leftHyperbox.__box[spiltAxis][1] = spiltVal
        rightHyperbox.__box[spiltAxis][0] = spiltVal

        return leftHyperbox, rightHyperbox, spiltVal

    def GenRandSpiltHyperbox(self):
        """
        随机按某一轴分割超盒，该轴的 extent 不为 0

        @return
            leftHyperbox: 左超盒
            rightHyperbox: 右超盒
            spiltAxis: 分割的轴
            spiltVal: 分割值
        """

        hyperbox = self.__box
        dim = self.GetDim()

        extent = hyperbox[:, 1] - hyperbox[:, 0]
        while True:
            spiltAxis = random.randint(0, dim-1)
            if extent[spiltAxis] != 0:
                break

        spiltVal = (hyperbox[spiltAxis][1] - hyperbox[spiltAxis][0]) / 2
        leftHyperbox = self.GenCopy()
        rightHyperbox = self.GenCopy()
        leftHyperbox.__box[spiltAxis][1] = spiltVal
        rightHyperbox.__box[spiltAxis][0] = spiltVal

        return leftHyperbox, rightHyperbox, spiltAxis, spiltVal

    def GenExpandedHyperbox(self, ratio):
        """
        对拷贝的 hyperbox 的所有维度进行扩展，extent 扩大 ratio

        @param
            ratio: 扩大百分比, 0% -- 100%

        @return
            expandedHyperbox: 扩展的超盒
        """

        copiedHyperbox = self.GenCopy()

        newExtent = self.GetExtent() * ratio
        copiedHyperbox.__box[:, 0] -= newExtent / 2
        copiedHyperbox.__box[:, 1] += newExtent / 2

        return copiedHyperbox

    def IsContain(self, sample):
        """
        是否包含 sample

        @param
            sample: 样本，一位数组，要求维度大于 box's dim
        """

        sampleDim = sample.shape[0]
        boxDim = self.GetDim()
        if sampleDim <= boxDim:
            raise RuntimeError("sample's dim <= box's dim")

        for i in range(boxDim):
            if sample[i] < self.__box[i][0] or sample[i] > self.__box[i][1]:
                return False

        return True

    def SelectData(self, data):
        """
        根据超立方体的范围，选择出相应的数据

        @param 
            data: 数据，n x (inputDim + outputDim)

        @return
            dataInHyperbox: 在超盒中的数据
        """

        inputDim = self.GetDim()
        dim = data.shape[1]
        if inputDim >= dim:
            raise RuntimeError("hyperbox's dim >= data's dim")
        
        sampleNum = data.shape[0]
        rows = []
        for row in range(sampleNum):
            if self.IsContain(data[row]):
                rows.append(row)

        dataInHyperbox = data[rows]

        return dataInHyperbox

    def GetSortedSpiltableAxis(self, data):
        spiltableAxis = self.GetSpiltableAxis()

        spiltableAxisNum = len(spiltableAxis)
        sampleNum = data.shape[0]
        scores = []
        for i in range(len(spiltableAxis)):
            spiltAxis = spiltableAxis[i]
            leftBox, rightBox, spiltVal = self.GenSpiltHyperbox(spiltAxis)
            dataInLeftBox = leftBox.SelectData(data)
            dataInRightBox = rightBox.SelectData(data)
            leftSampleNum = dataInLeftBox.shape[0]
            rightSampleNum = dataInRightBox.shape[0]
            score = np.sqrt(leftSampleNum * rightSampleNum) / sampleNum
            scores.append(score)

        Ps = scores / np.sum(scores)
        print("Ps: {0}".format(Ps))
        AccP = [Ps[0]]
        for i in range(1, spiltableAxisNum):
            AccP.append(AccP[i-1]+Ps[i])

        sortedSpiltableAxis = []
        while len(sortedSpiltableAxis) != spiltableAxisNum:
            randVal = random.random()
            for i in range(spiltableAxisNum):
                if randVal < AccP[i]:
                    sortedSpiltableAxis.append(spiltableAxis[i])
                    break

        return sortedSpiltableAxis

    def DataEnough(self, data, expandRate, minNum):
        """
        在扩展的超盒中的数据是否充足

        @param
            data: 数据
            expandRate: 扩大率
            minNum: 最小数据量

        @return
            isEnough: 是否充足
        """

        expendedHyperbox = self.GenExpandedHyperbox(expandRate)
        dataInBox = expendedHyperbox.SelectData(data)
        sampleNum = dataInBox.shape[0]

        isEnough = sampleNum >= minNum

        return isEnough

class BinaryTree:
    def __init__(self, parent = None, left = None, right = None):
        self.parent = parent
        self.left = left
        self.right = right

    def IsLeaf(self):
        return self.left == None and self.right == None

    def GetLeftSize(self):
        if self.left == None:
            return 0

        return self.left.GetSize()

    def GetRightSize(self):
        if self.right == None:
            return 0

        return self.right.GetSize()

    def GetSize(self):
        return 1 + self.GetLeftSize() + self.GetRightSize()

class KDTree(BinaryTree):
    def __init__(self, spiltAxis = -1, spiltVal = 0):
        super().__init__()
        self.spiltAxis = spiltAxis
        self.spiltVal = spiltVal

class ModelKDTree(KDTree):
    def __init__(self, objID, rrmse, model, spiltAxis = -1, spiltVal = 0):
        super().__init__(spiltAxis, spiltVal)
        self.rrmse = rrmse
        self.model = model
        self.objID = objID

    def GetNodeID(self):
        if self.parent == None:
            return 0

        parentID = self.parent.GetNodeID()

        if self == self.parent.left:
            return parentID + 1

        return parentID + self.parent.GetLeftSize() + 1

    def SaveModel(self, saveChildModel = True):
        if saveChildModel:
            if self.left != None:
                self.left.SaveModel()
            if self.right != None:
                self.right.SaveModel()

        if self.model == None:
            return

        if self.left != None and self.right != None:
            return

        nodeID = self.GetNodeID()
        prefix = str(self.objID) + "_" + str(nodeID) + "_"

        dense0weights = self.model.get_layer("dense0").get_weights()
        dense0weightsbias = np.row_stack((dense0weights[0], dense0weights[1]))
        dense1weights = self.model.get_layer("dense1").get_weights()
        dense1weightsbias = np.row_stack((dense1weights[0], dense1weights[1]))
        dense2weights = self.model.get_layer("dense2").get_weights()
        dense2weightsbias = np.row_stack((dense2weights[0], dense2weights[1]))

        np.savetxt(prefix + "dense0.csv", dense0weightsbias, delimiter=',')
        np.savetxt(prefix + "dense1.csv", dense1weightsbias, delimiter=',')
        np.savetxt(prefix + "dense2.csv", dense2weightsbias, delimiter=',')

    def ToDict(self):
        rst = {}

        rst["nodeID"] = self.GetNodeID()
        rst["rrmse"] = self.rrmse
        rst["spiltAxis"] = self.spiltAxis
        rst["spiltVal"] = self.spiltVal

        if self.left != None:
            rst["left"] = self.left.ToDict()
        else:
            rst["left"] = None
            
        if self.right != None:
            rst["right"] = self.right.ToDict()
        else:
            rst["right"] = None

        return rst
