Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/3rdParty"
)

Ubpa_AddTarget(MODE "LIB" SOURCES ${sources})
