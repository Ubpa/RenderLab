set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#默认使用目录名作为项目名
GET_DIR_NAME(DIRNAME)
set(TARGET_NAME "${TARGET_PREFIX}${DIRNAME}")
#多个源文件用 [空格] 分隔
#如：set(STR_TARGET_SOURCES "main.cpp src_2.cpp")
file(GLOB ALL_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
set(STR_TARGET_SOURCES "")
foreach(SOURCE ${ALL_SOURCES})
	set(STR_TARGET_SOURCES "${STR_TARGET_SOURCES} ${SOURCE}")
endforeach(SOURCE ${ALL_SOURCES})

#set(STR_TARGET_SOURCES "${STR_TARGET_SOURCES} ${CMAKE_SOURCE_DIR}/config/config.in")
#set(STR_TARGET_SOURCES "${DIRNAME}.cpp ${DIRNAME}.fs ${DIRNAME}.vs")
#多个库文件用 [空格] 分隔，如果为空，就输入[一个空格]
#如：set(STR_TARGET_LIBS "lib1.lib lib2.lib")
set(STR_TARGET_LIBS "Qt5::Widgets RasterOpCreator PaintImgOpCreator RawAPI_OGLW OpThread Shape Component Intersector Light Material Primitive RTX Scene Viewer")

SETUP_PROJECT(${MODE} ${TARGET_NAME} ${STR_TARGET_SOURCES} ${STR_TARGET_LIBS})

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
