# TODO

## 重构

- [x] namespace 简化
- [ ] 删除
  - [ ] FunctionTraits
  - [ ] Math
- [ ] 优化 GS
- [ ] HeapObj 机制重检
- [ ] 名称简化
  - [ ] ArgManager => ArgMngr
- [ ] SObj 机制
- [ ] 空文件删除
- [ ] 第三方依赖整理，尽量减少
  - [x] assimp
  - [ ] 
- [ ] 项目文件整理

## old

### Copy Frostbite

[MovingFrostbiteToPBR](https://github.com/Ubpa/Note/tree/master/CG/courses/MovingFrostbiteToPBR) 

- [x] 材质（HC-GGX + Disney Diffuse） 
  - [x] 离线
  - [x] 实时
- [ ] 光照
  - [x] 直接
    - [x] diffuse
      - [x] 点
      - [x] 聚光灯
      - [x] sphere
        - [x] 离线
        - [x] 实时
      - [x] disk
        - [x] 离线
        - [x] 实时
      - [x] rectangle / frustum
        - [x] 离线
        - [x] 实时
      - [x] tube / line
        - [x] 离线
          - [x] 形状
          - [x] 光源
        - [x] 实时
          - [x] 形状
          - [x] 光源
    - [x] specular
      - [x] Sphere
      - [x] disk
      - [x] rectangle / frustum
      - [x] tube / line
  - [ ] 间接
    - [x] distant light probe
    - [ ] local light probe
    - [ ] SSR
    - [ ] planar reflection
- [ ] 相机

### LTC

- [x] GGX LTC LUT (matrix(4) + norm(2) + sphere(1))
- [ ] Texture
- [ ] disk
- [ ] sphere

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
- [x] 聚光灯
  - [x] 离线
  - [x] 实时
    - [x] 光照计算
    - [x] 阴影
  - [x] UI
  - [x] （反）序列化
- [x] 环境光
  - [x] 离线
    - [x] 别名法 AliasMethod
    - [x] 无限远光源 InifiniteAreaLight
  - [x] 实时
    - [x] 从 equirectangular map 到 cubemap
    - [x] 渲染 skybox
    - [x] irradiace map
    - [x] pre-filtering an HDR environment map
    - [x] pre-computing the BRDF
    - [x] IBL
      - [x] diffuse
      - [x] specular
  - [x] UI
  - [x] （反）序列化
- [x] 延迟管线（金属工作流）
  - [x] GBuffer
  - [x] Direct Light
  - [x] Ambient Light
  - [x] Sky Box
  - [x] window
  - [x] 优化
- [x] Gooch
- [ ] Frostbite
- [x] GLSL include
- [x] 光源与其他 Component 统一
- [ ] GLSL 库函数
- [ ] 将 Engine 取出
- [x] 去除 CUDA 和 Optix 的依赖
- [ ] 重构 OpenGL 库（资源管理）
- [ ] 拷贝粘贴，模型、图像等复用
- [ ] CSM
- [ ] 封装 UBO，改进 FBO
- [ ] 解析 Shader（uniform）
- [ ] 实时渲染——面光源
- [ ] 旋转拖动缩放物体
- [ ] 重新架构 Raster
- [ ] 重构序列化
- [ ] Bloom
- [ ] 重构 MicrofacetDistribution
- [ ] opengl 的 c++ 版本
- [x] 半边结构
- [x] loop
- [x] 极小化曲面
- [x] 参数化
- [x] 空间场平滑插值 RBF
- [ ] 释放 OpenGL 资源
- [ ] TriMesh 性能优化

### Unimportant

- [ ] 添加注释
- [ ] 保存 Code 模式的 Trimesh
- [ ] 优化 intersector
- [x] 清除 useless 参数
- [ ] AreaLight 改名为 RectLight

### Bugs

- [x] 边缘黑斑（大概是因为 ambient light 中直接写 directlight 的纹理导致的
- [ ] 切换 Raster 发生内存泄露
- [ ] 胶囊体的 uv
- [ ] 胶囊体光源离线有问题
- [x] disk 序列化
- [ ] 胶囊体阴影不正确

