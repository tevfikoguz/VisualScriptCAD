cmake_minimum_required (VERSION 3.6)

set (VSCAD_VERSION_1 0)
set (VSCAD_VERSION_2 0)
set (VSCAD_VERSION_3 1)
set (VSCAD_APP_NAME VisualScriptCAD)

function (SetCompilerOptions module maxWarningLevel)
	set (DebugOptions)
	set (ReleaseOptions)
	if (MSVC)
		set (DebugOptions -DDEBUG -D_DEBUG /ZI /JMC /sdl /GR)
		set (ReleaseOptions /Oi /Gy)
		if (${maxWarningLevel})
			target_compile_options (${module} PRIVATE /W4 /WX)
		endif ()
	else ()
		set (DebugOptions -DDEBUG -D_DEBUG -g -ggdb)
		if (${maxWarningLevel})
			target_compile_options (${module} PRIVATE -Wall -Wextra -Werror)
		endif ()
		target_compile_options (${module} PRIVATE -std=c++11)
	endif ()
	target_compile_options(${module} PUBLIC "$<$<CONFIG:DEBUG>:${DebugOptions}>")
	target_compile_options(${module} PUBLIC "$<$<CONFIG:RELEASE>:${ReleaseOptions}>")
endfunction ()

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_SUPPRESS_REGENERATION 1)
set (CMAKE_CONFIGURATION_TYPES Debug;Release)
set (WXWIDGETS_DIR $ENV{WXWIDGETS_DIR} CACHE PATH "wxWidgets binary directory.")
set (VSE_DEVKIT_DIR $ENV{VSE_DEVKIT_DIR} CACHE PATH "VisualScriptEngine binary directory.")
set (BOOST_INCLUDEDIR $ENV{BOOST_INCLUDEDIR} CACHE PATH "Boost directory.")
set (CGAL_DIR $ENV{CGAL_DIR} CACHE PATH "CGAL directory.")

add_definitions (-DUNICODE -D_UNICODE)

project (VisualScriptCAD)

enable_testing ()

set (LibSourcesFolder Libs)
set (GLMSourcesFolder ${LibSourcesFolder}/glm-0.9.9.2)

# Geometry

set (GeometrySourcesFolder Sources/Geometry)
file (GLOB GeometryHeaderFiles ${GeometrySourcesFolder}/*.hpp)
file (GLOB GeometrySourceFiles ${GeometrySourcesFolder}/*.cpp)
set (
	GeometryFiles
	${GeometryHeaderFiles}
	${GeometrySourceFiles}
)
source_group ("Sources" FILES ${GeometryFiles})
add_library (Geometry STATIC ${GeometryFiles})
target_include_directories (
	Geometry PUBLIC
	${GLMSourcesFolder}
	${GeometryHeaderFiles}
	${GeometrySourceFiles}
)
SetCompilerOptions (Geometry ON)

# Modeler

set (ModelerSourcesFolder Sources/Modeler)
file (GLOB ModelerHeaderFiles ${ModelerSourcesFolder}/*.hpp)
file (GLOB ModelerSourceFiles ${ModelerSourcesFolder}/*.cpp)
set (
	ModelerFiles
	${ModelerHeaderFiles}
	${ModelerSourceFiles}
)
source_group ("Sources" FILES ${ModelerFiles})
add_library (Modeler STATIC ${ModelerFiles})
target_include_directories (
	Modeler PUBLIC
	${GLMSourcesFolder}
	${ModelerHeaderFiles}
	${ModelerSourceFiles}
)
target_include_directories (Modeler PUBLIC ${GeometrySourcesFolder})
target_link_libraries (Modeler Geometry)
SetCompilerOptions (Modeler ON)

# CGALOperations

set (CGALOperationsSourcesFolder Sources/CGALOperations)
file (GLOB CGALOperationsHeaderFiles ${CGALOperationsSourcesFolder}/*.hpp)
file (GLOB CGALOperationsSourceFiles ${CGALOperationsSourcesFolder}/*.cpp)
set (
	CGALOperationsFiles
	${CGALOperationsHeaderFiles}
	${CGALOperationsSourceFiles}
)
source_group ("Sources" FILES ${CGALOperationsFiles})
add_library (CGALOperations STATIC ${CGALOperationsFiles})
target_include_directories (
	CGALOperations PUBLIC
	${GLMSourcesFolder}
	${CGALOperationsHeaderFiles}
	${CGALOperationsSourceFiles}
	${BOOST_INCLUDEDIR}
	${CGAL_DIR}/include
	${CGAL_DIR}/build/include
	${CGAL_DIR}/auxiliary/gmp/include
)
target_include_directories (CGALOperations PUBLIC ${GeometrySourcesFolder} ${ModelerSourcesFolder})
target_link_libraries (CGALOperations Geometry Modeler libgmp-10.lib libmpfr-4.lib)
SetCompilerOptions (CGALOperations ON)
target_compile_options (CGALOperations PRIVATE /bigobj)
get_filename_component (CGALFolderAbsolute "${CGAL_DIR}" ABSOLUTE)
add_custom_command (TARGET CGALOperations POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CGALFolderAbsolute}/build/bin $<TARGET_FILE_DIR:CGALOperations>
)
add_custom_command (TARGET CGALOperations POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CGALFolderAbsolute}/auxiliary/gmp/lib $<TARGET_FILE_DIR:CGALOperations>
)

# EngineTest

link_directories (${CGAL_DIR}/build/lib)
link_directories (${CGAL_DIR}/auxiliary/gmp/lib)

set (TestFrameworkSourcesFolder Sources/TestFramework)
set (EngineTestSourcesFolder Sources/EngineTest)
file (GLOB TestFrameworkFiles ${TestFrameworkSourcesFolder}/*.hpp ${TestFrameworkSourcesFolder}/*.cpp)
file (GLOB EngineTestHeaderFiles ${EngineTestSourcesFolder}/*.hpp)
file (GLOB EngineTestSourceFiles ${EngineTestSourcesFolder}/*.cpp)
set (
	EngineTestTestFiles
	${EngineTestHeaderFiles}
	${EngineTestSourceFiles}
)
set (
	EngineTestFiles
	${TestFrameworkFiles}
	${EngineTestTestFiles}
)
source_group ("Framework" FILES ${TestFrameworkFiles})
source_group ("Sources" FILES ${EngineTestTestFiles})
add_executable (EngineTest ${EngineTestFiles})
target_include_directories (EngineTest PUBLIC ${GeometrySourcesFolder} ${ModelerSourcesFolder} ${CGALOperationsSourcesFolder} ${TestFrameworkSourcesFolder})
target_link_libraries (EngineTest Geometry Modeler CGALOperations)
SetCompilerOptions (EngineTest ON)
add_test (EngineTest EngineTest)

# VisualScriptCAD

link_directories (${WXWIDGETS_DIR}/lib/vc_x64_lib)
link_directories (${VSE_DEVKIT_DIR}/lib)

set (VisualScriptCADSourcesFolder Sources/VisualScriptCAD)

configure_file (${VisualScriptCADSourcesFolder}/Application/VersionInfo.hpp.in ${PROJECT_BINARY_DIR}/VersionInfo.hpp)
include_directories (${PROJECT_BINARY_DIR})

file (GLOB VisualScriptCADGladSourceFiles ${LibSourcesFolder}/glad-opengl-3.3/include/glad/*.h ${LibSourcesFolder}/glad-opengl-3.3/src/*.c)
file (GLOB VisualScriptCADSourceFiles ${VisualScriptCADSourcesFolder}/*.hpp ${VisualScriptCADSourcesFolder}/*.cpp)
if (MSVC)
	file (GLOB VisualScriptCADSourceFiles ${VisualScriptCADSourceFiles} ${VisualScriptCADSourcesFolder}/appicon.rc ${VisualScriptCADSourcesFolder}/appicon.ico)
endif ()
file (GLOB VisualScriptCADEditorSourceFiles ${VisualScriptCADSourcesFolder}/Editor/*.hpp ${VisualScriptCADSourcesFolder}/Editor/*.cpp)
file (GLOB VisualScriptCADRendererSourceFiles ${VisualScriptCADSourcesFolder}/Renderer/*.hpp ${VisualScriptCADSourcesFolder}/Renderer/*.cpp)
file (GLOB VisualScriptCADApplicationSourceFiles ${VisualScriptCADSourcesFolder}/Application/*.hpp ${VisualScriptCADSourcesFolder}/Application/*.cpp ${PROJECT_BINARY_DIR}/VersionInfo.hpp)
set (
	VisualScriptCADFiles
	${VisualScriptCADGladSourceFiles}
	${VisualScriptCADSourceFiles}
	${VisualScriptCADEditorSourceFiles}
	${VisualScriptCADRendererSourceFiles}
	${VisualScriptCADApplicationSourceFiles}
)

source_group ("Libs" FILES ${VisualScriptCADGladSourceFiles})
source_group ("Sources" FILES ${VisualScriptCADSourceFiles})
source_group ("Editor" FILES ${VisualScriptCADEditorSourceFiles})
source_group ("Renderer" FILES ${VisualScriptCADRendererSourceFiles})
source_group ("Application" FILES ${VisualScriptCADApplicationSourceFiles})

add_executable (VisualScriptCAD WIN32 ${VisualScriptCADFiles})
target_include_directories (
	VisualScriptCAD PUBLIC
	${GeometrySourcesFolder}
	${ModelerSourcesFolder}
	${CGALOperationsSourcesFolder}
	${LibSourcesFolder}/glad-opengl-3.3/include
	${VisualScriptCADSourcesFolder}/Editor
	${VisualScriptCADSourcesFolder}/Renderer
	${VisualScriptCADSourcesFolder}/Application
	${VisualScriptCADSourcesFolder}
	${WXWIDGETS_DIR}/include
	${WXWIDGETS_DIR}/include/msvc
	${VSE_DEVKIT_DIR}/include
)
target_link_libraries (VisualScriptCAD Geometry Modeler CGALOperations)
target_compile_definitions (VisualScriptCAD PUBLIC _CRT_SECURE_NO_WARNINGS)
SetCompilerOptions (VisualScriptCAD ON)