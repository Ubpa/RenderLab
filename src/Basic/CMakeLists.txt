Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/Basic"
)

Ubpa_GetTargetName(3rdParty "${PROJECT_SOURCE_DIR}/src/3rdParty")
Ubpa_AddTarget(MODE "LIB" SOURCES ${sources} LIBS ${3rdParty})
