Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/Engine"
)
Ubpa_GetTargetName(OpenGL "${PROJECT_SOURCE_DIR}/src/OpenGL")
Ubpa_GetTargetName(Qt "${PROJECT_SOURCE_DIR}/src/Qt")
Ubpa_AddTarget_GDR(MODE "LIB" SOURCES ${sources}
	LIBS_GENERAL "${OpenGL};${Qt};Qt5::Widgets"
	LIBS_DEBUG "3rdParty/assimp-vc140-mtd;3rdParty/IrrXMLd;3rdParty/zlibstaticd"
	LIBS_RELEASE "3rdParty/assimp-vc140-mt;3rdParty/IrrXML;3rdParty/zlibstatic")
