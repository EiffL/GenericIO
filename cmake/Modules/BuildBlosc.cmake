include(ExternalProject)

set(BloscTag v1.11.1)
set(SHALLOW_GIT_CLONE GIT_SHALLOW 1)

# Downloads and compiles Blosc
ExternalProject_Add(BLOSC
        PREFIX BLOSC
        GIT_REPOSITORY https://github.com/Blosc/c-blosc.git
        GIT_TAG ${BloscTag}
        ${SHALLOW_GIT_CLONE}
        DOWNLOAD_NO_PROGRESS 1
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/extern )

set(BLOSC_LIBRARY_DIRS ${CMAKE_BINARY_DIR}/extern/lib/ )
set(BLOSC_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/extern/include/AngPow)
set(BLOSC_LIBRARIES -lblosc -lz -lzstd)
