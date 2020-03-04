# UGM

> **U**bpa **G**raphics **M**ath

## 特性

- 代数

  - [线性](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/ext/Linearity.h) 

    ```c++
    const ImplT operator+(const ImplT & v) const;
    ImplT & operator+=(const ImplT & v);
    const ImplT operator-() const;
    ImplT & NegativeSelf() const;
    const ImplT operator-(const ImplT & v) const;
    ImplT & operator-=(const ImplT & v);
    const ImplT operator*(T k) const;
    ImplT & operator*=(T k);
    const ImplT operator/(T k) const;
    ImplT & operator/=(T k);
    static const ImplT Zero_Linearity();
    static constexpr T Identiry_Linearity();
    friend const ImplT operator*(T k, const ImplT & v);
    ```

  - [内积](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/ext/InnerProduct.h) 

    ```c++
    static T Dot(const ImplT & lhs, const ImplT & rhs);
    T Dot(const ImplT & v) const;
    ```

  - [赋范](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/ext/Normed.h) 

    ```c++
    T Norm2() const;
    T Norm() const;
    static T CosTheta(const ImplT & lhs, const ImplT & rhs);
    const ImplT Normalize() const;
    ImplT & NormalizeSelf();
    ```

  - [度量](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/ext/Metric.h) 

    ```c++
    T static T Distance2(const ImplT & lhs, const ImplT & rhs);
    T Distance2With(const ImplT & v) const;
    static T Distance(const ImplT & lhs, const ImplT & rhs);
    T DistanceWith(const ImplT & v) const;
    ```

  - [Hadamard Product](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/ext/HadamardProduct.h)（对应元素相乘）

    ```c++
    const ImplT operator*(const ImplT & v) const;
    ImplT & operator*=(const ImplT & v);
    const ImplT operator/(const ImplT & v) const;
    ImplT & operator/=(const ImplT & v);
    static const ImplT Indentity_HadamardProduct();
    const ImplT Inverse_HadamardProduct() const;
    ```

  - [叉乘](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/ext/CrossProduct.h)（仅用于三维向量）

    ```c++
    static const ImplT Cross(const ImplT & lhs, const ImplT & rhs);
    const ImplT Cross(const ImplT & v) const;
    ```

- 面向对象

  > 调用函数十分方便

- The Curiously Recurring Template Pattern (CRTP) 

- ~~Debug 友好~~

  > ~~可在 IDE 中直接看到 x、y、z、w~~
  >
  > 引入了内存问题，取消这个功能

- 量：[值](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Val.h)、[点](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Point.h)、[向量](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Vector.h)、[法向](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Normal.h)、[欧拉角](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/EulerYXZ.h)、[四元数](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Quat.h)、[RGB](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/RGB.h)、[RGBA](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/RGBA.h)、[缩放](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Scale.h)、[3x3 矩阵](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Mat3x3.h)、[4x4 矩阵](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Mat4x4.h)、[包围盒](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/BBox.h)、[射线](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Ray.h)、[变换](https://github.com/Ubpa/RenderLab/blob/master/include/CppUtil/Basic/UGM/Transform.h) 

  - 点：欧式度量
  - 向量：内积范数，欧式内积，线性空间
  - 法向：内积范数，欧式内积，线性空间
  - RGB：Hadamard Product，线性空间
  - 缩放：Hadamard Product，线性空间
  - 欧拉角：Basic
  - 四元数：Basic

- 四元数，轴角，变换，欧拉角转换

  ![rotate](https://raw.githubusercontent.com/Ubpa/ImgBed/master/RenderLab/include/CppUtil/Basic/UGM/rotate.png)

