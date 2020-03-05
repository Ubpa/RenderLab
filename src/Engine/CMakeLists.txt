Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/Engine"
)
Ubpa_GetTargetName(OpenGL "${PROJECT_SOURCE_DIR}/src/OpenGL")
Ubpa_GetTargetName(Qt "${PROJECT_SOURCE_DIR}/src/Qt")

set(libs ${OpenGL} ${Qt} Qt5::Widgets)

if(TARGET assimp::assimp)
	list(APPEND libs assimp::assimp)
	add_definitions(-DUSE_ASSIMP)
endif()

Ubpa_AddTarget(MODE "LIB" SOURCES ${sources} LIBS ${libs})
