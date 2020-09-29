# 安装步骤

2020/07/30 更新

cmake 过后，将 [Num.h](external/UTemplate/include/UTemplate/Num.h) 的 29-46 行注释

---

- 下载 RenderLab 的源码

- 下载资源文件（百度网盘）

  - 链接：https://pan.baidu.com/s/1D2dIBmN2Ub8dKYjO6LsV3w 
  - 提取码：amiw

- 下载 QT 5.12.2 64 位

  - 选择 Open Source 下载
  - 选择 5.12.2 MSVC 2017 64-bit 和 Sources
  - 将 `YOUR_PATH_TO/Qt5/msvc 2017 64/bin` 加入环境变量 `Path` 中


-  **[如果需要降噪]** 下载 CUDA 9.0（因为 Tensorflow-GPU 需要 9.0 版本，故一直用的是 9.0 版本的 CUDA，没换成新版本），选**自定义**，不安装自带的驱动（自带的版本过低）

-  **[如果需要降噪]** 下载 OptiX 6.0（要用到其中的 **Optix AI Denoiser**），保存在默认位置即可（否则 CMake 里边要修改下 `OptiX_INSTALL_DIR`）

  - **[如果需要降噪]** 将 OptiX 的 bin 目录（`C:\ProgramData\NVIDIA Corporation\OptiX SDK 6.0.0\bin64`）加到环境变量 `Path` 中

-  **[如果需要降噪]** 更新 N 卡驱动到最新（**418.81 及以上**，这是 Optix 要求的，CUDA安装时装了旧版驱动的话就在这一步更新一下）

- 用 CMake-GUI 建工程

  - Options -> Warning Messages -> Suppress Warnings -> Developer Warnings

  - source-code 就是 `YOUR_PATH_TO/RenderLab` 

  - build 目录就是 根目录下的 build 文件夹（ 自行创建 `YOUR_PATH_TO/RenderLab/build`） 

  - configure，选 **Vistual Studio 15 2017 Win64**，**[如果需要降噪]** CMake 会自动找到 CUDA 和 OptiX

    > **[如果需要降噪]** 确保 `Optix_INSTALL_DIR`、`Optix_INCLUDE` 和 `CUDA_TOOLKIT+ROOT_DIR` 是正确的

    -  **[如果需要降噪]** 勾选 `USE_DENOISE` 

  - generate

  - open project

    - 选择 `Release` 

      > `Release` 会开启 **核心数 - 1** 个线程来渲染。`Debug` 模式下为**单线程**渲染

    - 选择 `CMakePredefinedTargets` 里的 `ALLBUILD`，右键**生成**。（`INSTALL` 会将可执行文件安装到 `bin` 目录，将静态库安装到 `lib` 目录）
  
  - 直接运行 `App` 下的 `RenderLab` 和 `RTGIwRRF`。其余需要提供命令行参数。

## 可选项

- assimp
- OptiX AI Denoisor

