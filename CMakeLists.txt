cmake_minimum_required(VERSION 3.15)
project(sudoSolver)

include(GenerateG3A)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

set(SOURCES
	src/sudoSolv.cpp
	#src/main.cpp
	src/shared/bFile.cpp
	src/shared/keyboard.cpp
	src/shared/menuBar.cpp
	#src/shared/scrCapture.cpp
	src/position.cpp
	src/element.cpp
	src/tinySquare.cpp
	src/grids.cpp
	src/sudoku.cpp
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_cg
  assets-cg/about.png
  assets-cg/menu.png
)

fxconv_declare_assets(${ASSETS} ${ASSETS_cg} WITH_METADATA)

add_executable(sudoSolver ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(sudoSolver PRIVATE -Wall -Wextra -Os -D=DEST_CASIO_CALC -D=NO_CAPTURE)
target_link_libraries(sudoSolver Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
	generate_g3a(TARGET sudoSolver OUTPUT "sudoSolv.g3a"
		NAME "sudoSolver" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
endif()
