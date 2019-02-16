# RenderLab

> 渲染实验室

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
- [x] 用新的架构来写离线渲染
  - [x] 相机
  - [x] 组件机制优化
  - [x] Material (BSDF)
    - [x] Diffuse
    - [x] Mirror
    - [x] Glass
  - [x] PathTracer (PTX)
    - [x] 基本框架
    - [x] Mirror
    - [x] Glass
    - [x] Transform
    - [x] Plane
    - [x] 灯光
    - [x] 测试 Cornell Box
    - [x] 网格
      - [x] Triangle
      - [x] TriMesh
      - [x] BVH
    - [x] 相交性测试（不需要找到最近，只要相交就退出）
- [x] 用新的架构来写实时渲染
  - [x] Viewer
  - [x] Raster
  - [x] Roamer（把 Viewer 中的相机相关部分抽出）
- [ ] 实装UI功能（查看，编辑）
  - [x] 层次面板
  - [x] 属性面板（展示 component）
    - [x] 基础框架
    - [x] Transform
    - [x] Geometry
    - [x] Material
    - [x] Light
  - [x] 选择器 Picker
  - [ ] 设置面板
- [ ] 优化光线追踪
  - [x] 噪点异常
  - [ ] 降噪
  - [x] 场景BVH（因为在编辑时场景是动态的，所以在光线追踪任务开始时才建立 BVH）
  - [ ] 块化渲染
  - [ ] 线程化（不要用OpenMP）
- [ ] 光线追踪新增功能
  - [ ] Microfacet BRDF
- [ ] json 场景文件
- [ ] 场景编辑
- [ ] 优化实时渲染
