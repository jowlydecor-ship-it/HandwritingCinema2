cmake_minimum_required(VERSION 3.20)
project(HandwritingCinema90s2000s LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

include(FetchContent)

# ---------------------------------------------------------------------------
# OpenFX headers + Support library (compilée directement dans le plugin).
# Pour utiliser une copie locale : cmake -DOFX_ROOT=/chemin/vers/openfx ..
# Pense à épingler OFX_GIT_TAG sur un tag/commit précis une fois validé.
# ---------------------------------------------------------------------------
set(OFX_GIT_TAG "main" CACHE STRING "Tag ou commit du dépôt openfx")
if(OFX_ROOT)
  set(openfx_SOURCE_DIR ${OFX_ROOT})
else()
  FetchContent_Declare(openfx
    GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openfx.git
    GIT_TAG ${OFX_GIT_TAG}
    SOURCE_SUBDIR _no_cmake_here)   # on ne veut que les sources, pas leur CMake
  FetchContent_MakeAvailable(openfx)
endif()

file(GLOB OFX_SUPPORT_SRC ${openfx_SOURCE_DIR}/Support/Library/*.cpp)
# Retire les fichiers spécifiques à d'autres plateformes / non utilisés
list(FILTER OFX_SUPPORT_SRC EXCLUDE REGEX "ofxsOGLTextRenderer|ofxsImageBlender")

# ---------------------------------------------------------------------------
# FreeType (statique, sans dépendances optionnelles)
# ---------------------------------------------------------------------------
set(FT_DISABLE_ZLIB   ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2  ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG    ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG VER-2-13-3)
FetchContent_MakeAvailable(freetype)

# JSON pour les presets
FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(nlohmann_json)

# ---------------------------------------------------------------------------
# Plugin
# ---------------------------------------------------------------------------
set(BUNDLE_NAME HandwritingCinema90s2000s)
set(BUNDLE_DIR ${CMAKE_BINARY_DIR}/${BUNDLE_NAME}.ofx.bundle)

add_library(${BUNDLE_NAME} MODULE
  src/Plugin.cpp
  src/TextRaster.cpp
  src/Preset.cpp
  ${OFX_SUPPORT_SRC})

target_include_directories(${BUNDLE_NAME} PRIVATE
  src
  ${openfx_SOURCE_DIR}/include
  ${openfx_SOURCE_DIR}/Support/include)

target_link_libraries(${BUNDLE_NAME} PRIVATE freetype nlohmann_json::nlohmann_json)

set_target_properties(${BUNDLE_NAME} PROPERTIES
  PREFIX ""
  SUFFIX ".ofx"
  LIBRARY_OUTPUT_DIRECTORY ${BUNDLE_DIR}/Contents/MacOS)

# Assemblage du bundle : Info.plist + Resources, puis signature ad hoc
# (obligatoire sur Apple Silicon pour que Resolve charge le binaire).
add_custom_command(TARGET ${BUNDLE_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_SOURCE_DIR}/bundle/Info.plist ${BUNDLE_DIR}/Contents/Info.plist
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/Resources ${BUNDLE_DIR}/Contents/Resources
  COMMAND codesign --force --deep -s - ${BUNDLE_DIR}
  COMMENT "Assemblage et signature du bundle OFX")

# Installation pour Resolve : cmake --build build --target install_resolve
add_custom_target(install_resolve
  COMMAND ${CMAKE_COMMAND} -E make_directory /Library/OFX/Plugins
  COMMAND ${CMAKE_COMMAND} -E remove_directory /Library/OFX/Plugins/${BUNDLE_NAME}.ofx.bundle
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${BUNDLE_DIR} /Library/OFX/Plugins/${BUNDLE_NAME}.ofx.bundle
  DEPENDS ${BUNDLE_NAME}
  COMMENT "Installation dans /Library/OFX/Plugins (sudo requis)")
