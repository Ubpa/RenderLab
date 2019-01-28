#项目名，默认为目录名
GET_DIR_NAME(DIRNAME)
set(TARGET_NAME ${DIRNAME})
#多个源文件用 [空格] 分隔
#如：set(STR_TARGET_SOURCES "main.cpp src_2.cpp")
set(STR_TARGET_SOURCES "${DIRNAME}.cpp ${CMAKE_SOURCE_DIR}/include/CppUtil/RTX/${DIRNAME}.h")
set(STR_TARGET_SOURCES "${STR_TARGET_SOURCES} ${CMAKE_SOURCE_DIR}/include/CppUtil/OpenGL/CommonDefine.h")

set (ALL_SOURCES ${ALL_SOURCES}
	"${CMAKE_SOURCE_DIR}/data/shaders/common/basic_P2T2.vs"
	"${CMAKE_SOURCE_DIR}/data/shaders/common/basic_T2.fs"
	"${CMAKE_SOURCE_DIR}/data/shaders/common/ppBlur.fs"
	"${CMAKE_SOURCE_DIR}/data/shaders/common/gamma.fs"
	"${CMAKE_CURRENT_SOURCE_DIR}/TexWindow.cpp"
	"${CMAKE_SOURCE_DIR}/data/shaders/common/interpolation.fs"
	"${CMAKE_SOURCE_DIR}/include/CppUtil/RTX/TexWindow.h"
)
foreach(SOURCE ${ALL_SOURCES})
	set(STR_TARGET_SOURCES "${STR_TARGET_SOURCES} ${SOURCE}")
endforeach(SOURCE ${ALL_SOURCES})

#多个库文件用 [空格] 分隔，如果为空，就输入[一个空格]
#如：set(STR_TARGET_LIBS "lib1.lib lib2.lib")
set(STR_TARGET_LIBS "Operation glfw3.lib glad Glfw Image EventMngr Texture Model Shape Shader FBO Timer Config Camera VAO")

SETUP_PROJECT(${MODE} ${TARGET_NAME} ${STR_TARGET_SOURCES} ${STR_TARGET_LIBS})