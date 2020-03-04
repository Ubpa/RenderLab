Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/UI"
)

Ubpa_GetTargetName(3rdParty "${PROJECT_SOURCE_DIR}/src/3rdParty")
Ubpa_GetTargetName(Basic "${PROJECT_SOURCE_DIR}/src/Basic")
Ubpa_GetTargetName(OpenGL "${PROJECT_SOURCE_DIR}/src/OpenGL")
Ubpa_GetTargetName(Qt "${PROJECT_SOURCE_DIR}/src/Qt")
Ubpa_GetTargetName(Engine "${PROJECT_SOURCE_DIR}/src/Engine")

Ubpa_AddTarget(MODE "LIB" QT ON SOURCES ${sources} LIBS ${3rdParty} ${Basic} ${OpenGL} ${Qt} ${Engine})
