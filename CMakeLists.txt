cmake_minimum_required(VERSION 3.15)
project(MyAddin)

include(GenerateG3A)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

set(SOURCES
	src/sudoSolv.cpp
	src/shared/bFile.cpp
	src/position.cpp
	src/element.cpp
	src/tinySquare.cpp
	src/sudoku.cpp
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_cg
  assets-cg/example.png
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_cg} WITH_METADATA)

add_executable(myaddin ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(myaddin PRIVATE -Wall -Wextra -Os -D=DEST_CASIO_CALC)
target_link_libraries(myaddin Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
	generate_g3a(TARGET myaddin OUTPUT "sudoSolv.g3a"
		NAME "sudoSolv" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
endif()
