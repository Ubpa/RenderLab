# RenderLab

<h1 align=center>
<img src="logo/horizontal.png" width=50%>
</h1>

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
    - 寒霜 Frostbite
    - Gooch
  - 几何体 Geometry
    - 球 Sphere
    - 平面 Plane
    - 三角网格 TriMesh
    - 圆盘 Disk
    - 胶囊 Capsule
  - 光源 Light
    - 点光源 PointLight
    - 面光源 AreaLight
    - 方向光 DirectionalLight
    - 聚光灯 SpotLight
    - 无限远光源 InfiniteAreaLight
    - 球光源 SphereLight
    - 胶囊光源 CapsuleLight
    - 圆盘光源 DiskLight
- 离线渲染
  - 路径追踪 PathTracing
- 网格编辑
  - 参数化 Paramaterization
  - 粘合 Glue
  - 变形 DeformRBF
  - 极小曲面 MinSurf
  - 各向同性网格重划分 IsotropicRemeshing
  - Loop 细分 LoopSubdivision
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

驱动：418.81 or later is required

安装方法见于 [setup.md](setup.md) 

