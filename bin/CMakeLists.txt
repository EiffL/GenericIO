
add_executable(GenericIOPrint GenericIOPrint.cxx)
target_link_libraries(GenericIOPrint genericio ${MPI_CXX_LIBRARIES})

add_executable(GenericIOBenchmarkRead GenericIOBenchmarkRead.cxx)
target_link_libraries(GenericIOBenchmarkRead  genericio ${MPI_CXX_LIBRARIES})

add_executable(GenericIOBenchmarkWrite GenericIOBenchmarkWrite.cxx)
target_link_libraries(GenericIOBenchmarkWrite genericio ${MPI_CXX_LIBRARIES})

add_executable(GenericIO2Cosmo GenericIO2Cosmo.cxx)
target_link_libraries(GenericIO2Cosmo genericio ${MPI_CXX_LIBRARIES})

add_executable(GenericIORewrite GenericIORewrite.cxx)
target_link_libraries(GenericIORewrite genericio ${MPI_CXX_LIBRARIES})

add_executable(GenericIOVerify GenericIOVerify.cxx)
target_link_libraries(GenericIOVerify genericio ${MPI_CXX_LIBRARIES})
