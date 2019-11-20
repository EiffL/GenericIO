cmake_minimum_required(VERSION 3.15)
# Adding customized cmake module
list(APPEND CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(genericio)

  find_package(MPI REQUIRED)

  set(CMAKE_CXX_FLAGS "-O3 -D__STDC_CONSTANT_MACROS -fopenmp -fPIC")
  set(CMAKE_C_FLAGS "-O3 -D__STDC_CONSTANT_MACROS -fopenmp -fPIC")

  # Compiles dependencies
  add_subdirectory(thirdparty/sqlite)
  add_subdirectory(thirdparty/SZ)
  include(BuildBlosc)

  include_directories(${MPI_INCLUDE_PATH}
                      ${BLOSC_INCLUDE_DIRS}
                      ${CMAKE_SOURCE_DIR}/include
                      ${CMAKE_SOURCE_DIR}/thirdparty/sqlite
                      ${CMAKE_SOURCE_DIR}/thirdparty/SZ/sz/include)
  link_directories(${BLOSC_LIBRARY_DIRS})

  # Builds library
  add_library(genericio SHARED src/GenericIO.cxx)
  target_link_libraries(genericio ${BLOSC_LIBRARIES} sz sqlite ${MPI_CXX_LIBRARIES})

  # Builds binaries
  add_subdirectory(bin)

  # Builds pkgconfig file
  SET(PROJECT_DESCRIPTION "GenericIO is a write-optimized library for writing self-describing scientific data files on large-scale parallel file systems.")
  SET(PKG_CONFIG_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
  SET(PKG_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include/")
  SET(PKG_CONFIG_LIBS "-L${PKG_CONFIG_LIBDIR} -lgenericio")
  SET(PKG_CONFIG_CFLAGS "-I${PKG_CONFIG_INCLUDEDIR}")
  CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg-config.cmakein"
                  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" )

  # Installs the CCL library
  install(TARGETS genericio GenericIOPrint GenericIOBenchmarkRead GenericIOBenchmarkWrite GenericIO2Cosmo GenericIORewrite GenericIOVerify
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
  install(DIRECTORY include/ DESTINATION include
          FILES_MATCHING PATTERN "*.h")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/genericio.pc DESTINATION lib/pkgconfig)
