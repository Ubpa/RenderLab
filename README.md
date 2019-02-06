# RenderLab

渲染实验室

## 1. 编译

CMake: 3.1.0 以上

Visual Studio: 2017

Qt: 5.12.0

```bash
mkdir build
cd build
cmake .. -Wno-dev
```

## 2. TODO

- [x] 移植 OpenGL
- [x] 通过移植 LearnOpenGL / [ 05_08_Bloom ] 测试是否成功移植
- [x] 实现事件传递机制（键盘，鼠标，滚轮等）
- [x] 移植 CPU RTX
  - [x] 渲染图片的类
  - [x] 将光线追踪的结果渲染到图片上
- [x] 实现基础的UI功能
  - [x] 开始
  - [x] 停止
  - [x] 进度条
  - [x] 保存
- [x] 优化数据结构
  - [x] 动态访问模式~~(用了模板，函数static变量等“奇技淫巧”。最重要的特性是可继承)~~（改用 typeid 来实现）
  - [x] 场景对象（组件模式）
  - [x] 相交器
- [ ] 用新的架构来写离线渲染
  - [x] 相机
  - [x] 组件机制优化
  - [x] Material (BSDF)
  - [ ] PathTracer (PTX)
    - [x] 基本框架
    - [x] Mirror
    - [x] Glass
    - [x] Transform
    - [ ] Plane
    - [ ] 灯光（Cornell Box）
- [ ] 优化
  - [ ] Sphere 取消掉冗余的 center 和 radius
- [ ] 用新的架构来写实时渲染

## 3. 其他 TODO

- [x] 将模板头文件的定义和声明分离