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
- [ ] 实现事件传递机制（键盘，鼠标等）
- [ ] 移植 CPU RTX
- [ ] 优化数据结构
  - [ ] 结点
  - [ ] 相交器
- [ ] 设计一个数据结构同时用于 Raster 和 RayTracer
- [ ] 在层次面板中显示层次

