# RenderLab

## 0. 简介

渲染实验室，包含了实时渲染，离线渲染和场景编辑的功能

![engine](https://raw.githubusercontent.com/Ubpa/ImgBed/master/RenderLab/000.jpg)

## 1. 特性

- 应用 App
  - RenderLab
  - Denoiser
  - RTGIwRRF
  - SObjRenderer
  - SObjSampler
- 组件 Component
  - Camera
  - Transform
  - Material
    - Glass
    - Mirror
    - Diffuse
    - Metal Workflow
    - Frosted Glass
  - Geometry
    - Sphere
    - Plane
    - TriMesh
  - Light
    - AreaLight
    - PointLight
- 查看器 Viewer
  - Picker
  - Raster
  - Roamer
- 相交器 Intersector
  - RayIntersector
  - VisibilityChecker
- 离线渲染
  - PathTracing
- 其他功能
  - 几何数学库 UGM
  - AI 降噪 OptixAIDenoiser

## 2. 资源

项目所需的资源文件并没有上传到 github 上，放在了网盘里

链接：https://pan.baidu.com/s/1D2dIBmN2Ub8dKYjO6LsV3w 

提取码：amiw 

## 3. 编译

（安装方法以后补充）

CMake: 3.1.0 以上

Visual Studio: 2017

系统：windows 64位

Qt: 5.12.0 64位

CUDA: 9.0

Optix: 6.0

驱动：418.81 or later is required.

```bash
mkdir build
cd build
cmake .. -Wno-dev
```

## 4. TODO

- [x] 自己编写数学运算库，放弃 glm
- [x] 重构指针
- [ ] 旋转拖动缩放物体
- [ ] 方向标明
- [ ] 统一相机
- [ ] 重新架构 Raster
- [ ] 实时渲染——面光源
- [ ] obj 格式的模型载入
- [ ] ...

