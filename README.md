# RenderLab

## 0. 简介

渲染实验室，包含了实时渲染，离线渲染和场景编辑的功能

![engine](https://raw.githubusercontent.com/Ubpa/ImgBed/master/RenderLab/000.jpg)

## 1. 特性

- 应用 App
  - 渲染实验室 RenderLab
  - 降噪器 Denoiser
  - 实时全局光照 RTGIwRRF
  - SObj 渲染器 SObjRenderer
  - SObj 采样器 SObjSampler
- 组件 Component
  - 相机 Camera
  - 变换 Transform
  - 材质 Material
    - 玻璃 Glass
    - 镜子 Mirror
    - 漫反射 Diffuse
    - 金属工作流 Metal Workflow
    - 毛玻璃 Frosted Glass
  - 几何体 Geometry
    - 球 Sphere
    - 平面 Plane
    - 三角网格 TriMesh
  - 光源 Light
    - 面光源 AreaLight
    - 点光源 PointLight
    - 方向光 DirectionalLight
- 查看器 Viewer
  - 拣选器 Picker
  - 直接光照光栅器 DirectIllumRaster
  - 漫游器 Roamer
- 相交器 Intersector
  - 最近 RayIntersector
  - 可见性 VisibilityChecker
- 过滤器
  - Box
  - Gaussian
  - Mitchell
  - Sinc
  - Triangle
- 离线渲染
  - 路径追踪 PathTracing
- 其他功能
  - [几何数学库 UGM](https://github.com/Ubpa/RenderLab/tree/master/include/CppUtil/Basic/UGM) 
  - AI 降噪 OptixAIDenoiser

## 2. 安装

CMake: 3.1.0 以上

Visual Studio: 2017

系统：windows 64位

Qt: 5.12.0 64位

CUDA: 9.0

Optix: 6.0

驱动：418.81 or later is required.

安装方法见于 [setup.md](https://github.com/Ubpa/RenderLab/blob/master/setup.md) 

## 3. TODO

### Impotant

- [x] 自己编写数学运算库，放弃 glm
- [x] 重构指针
- [x] 补充安装方法
- [x] 线性化 BVH
- [x] Imaging Pipline
- [x] 简单模型载入
- [x] 方向光
  - [x] 离线
  - [x] 实时
    - [x] 光照计算
    - [x] 阴影
  - [x] UI
  - [x] （反）序列化
- [ ] 聚光灯
  - [x] 离线
  - [ ] 实时
    - [x] 光照计算
    - [ ] 阴影
  - [x] UI
  - [x] （反）序列化
- [ ] 旋转拖动缩放物体
- [ ] 方向标明
- [ ] 统一相机
- [ ] 重新架构 Raster
- [ ] 实时渲染——面光源
- [ ] ...

### Unimportant

- [ ] 添加注释
- [ ] 保存 Code 模式的 Trimesh 
- [ ] ...

