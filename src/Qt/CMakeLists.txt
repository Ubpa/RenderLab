Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/Qt"
)
Ubpa_GetTargetName(Basic "${PROJECT_SOURCE_DIR}/src/Basic")
Ubpa_AddTarget(MODE "LIB" QT ON SOURCES ${sources} LIBS "${Basic};Qt5::Widgets")
