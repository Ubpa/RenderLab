# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# 设定项目
project(RenderLab VERSION 1.0)
message(STATUS "[Project] ${PROJECT_NAME}")

# 修改默认的安装路径前缀
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "install prefix" FORCE)
endif()

# 在 Debug 模式下自动对 dll 和 lib 文件名后边加 d，但 exe 仍然需要手动设置
set(CMAKE_DEBUG_POSTFIX d)

# 确定 C++ 版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- /Zc:twoPhase-")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()

# 将生成的 exe, dll, lib 等放置在 bin, lib 目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib")

# 开启 VS 的文件夹功能
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)
FetchContent_Declare(
  UGit
  GIT_REPOSITORY https://github.com/Ubpa/UGit
  GIT_TAG        3c3cd3f
)
FetchContent_MakeAvailable(UGit)

Ubpa_InitGit()
Ubpa_UpdateSubModule()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/UCMake/cmake")
include(UbpaTool)

Ubpa_QtInit(COMPONENTS Core Gui Widgets OpenGL)

message(STATUS "----------")
find_package(assimp)
if(TARGET assimp::assimp)
	message(STATUS "assimp_FOUND: ${assimp_FOUND}")
	message(STATUS "assimp_DIR: ${assimp_DIR}")
endif()

message(STATUS "----------")
find_package(tinyxml2)
if(TARGET tinyxml2::tinyxml2)
	message(STATUS "tinyxml2_FOUND: ${tinyxml2_FOUND}")
	message(STATUS "tinyxml2_DIR: ${tinyxml2_DIR}")
endif()

message(STATUS "----------")
# https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html
# https://gitlab.com/libeigen/eigen/-/blob/master/cmake/Eigen3Config.cmake.in
# https://gitlab.com/libeigen/eigen/-/blob/master/cmake/UseEigen3.cmake
find_package(Eigen3 3.3 NO_MODULE)
if(TARGET Eigen3::Eigen)
	message(STATUS "EIGEN3_FOUND: ${EIGEN3_FOUND}")
	message(STATUS "EIGEN3_USE_FILE: ${EIGEN3_USE_FILE}")
	message(STATUS "EIGEN3_DEFINITIONS: ${EIGEN3_DEFINITIONS}")
	message(STATUS "EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
	message(STATUS "EIGEN3_INCLUDE_DIRS: ${EIGEN3_INCLUDE_DIRS}")
	message(STATUS "EIGEN3_ROOT_DIR: ${EIGEN3_ROOT_DIR}")
	message(STATUS "EIGEN3_VERSION_STRING: ${EIGEN3_VERSION_STRING}")
	include(${EIGEN3_USE_FILE})
else()
	message(WARNING "find_package(Eigen3 3.3 NO_MODULE) failed, so use include/_deps/Eigen/")
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/_deps/Eigen")
		message(FATAL_ERROR "not found Eigen/ in ${PROJECT_SOURCE_DIR}/include/_deps/")
	endif()
endif()

configure_file(config/config.in ${PROJECT_SOURCE_DIR}/config/config.out)
install(FILES "${PROJECT_SOURCE_DIR}/config/config.out"
  DESTINATION config)
configure_file(config/ROOT_PATH.h.in config/ROOT_PATH.h)

include_directories (
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include/3rdParty
	${PROJECT_SOURCE_DIR}/external/UHEMesh/include
	${PROJECT_SOURCE_DIR}/external/UGM/include
	${PROJECT_SOURCE_DIR}/external/UTemplate/include
	${PROJECT_SOURCE_DIR}/external/UDP/include
	${CMAKE_BINARY_DIR}/config
	# $ENV{XXX_INCLUDE_PATH}
	# ...
)
link_directories (
	${PROJECT_SOURCE_DIR}/lib
	# $ENV{XXX_LIB_PATH}
	# ...
)

set(USE_DENOISE OFF CACHE BOOL OFF)
if(USE_DENOISE)
	add_definitions(-DUSE_DENOISE)

	set(OptiX_INSTALL_DIR "C:/ProgramData/NVIDIA Corporation/OptiX SDK 6.0.0" CACHE PATH "Path to OptiX installed location." FORCE)

	#set(CUDA_TOOLKIT_ROOT_DIR CACHE PATH "cuda root")
	find_package(CUDA 5.0 REQUIRED)

	set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
	
	include_directories (
		${OptiX_INSTALL_DIR}/include
		${CUDA_TOOLKIT_ROOT_DIR}/include
	)
	link_directories (
		${OptiX_INSTALL_DIR}/lib64
	)
endif()

#FIND_PACKAGE(OpenMP REQUIRED)
#if(OPENMP_FOUND)
#	message("OPENMP FOUND")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

Ubpa_AddSubDirsRec(src)

include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set (CPACK_PACKAGE_CONTACT       "641614112@qq.com")
include (CPack)
