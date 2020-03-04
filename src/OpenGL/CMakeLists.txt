Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/OpenGL"
)
add_definitions(-DUSE_QT_OPENGL_API)
Ubpa_GetTargetName(Basic "${PROJECT_SOURCE_DIR}/src/Basic")
Ubpa_AddTarget(MODE "LIB" SOURCES ${sources} LIBS ${Basic} Qt5::Widgets)
