cmake_minimum_required(VERSION 3.3)
project(Saena)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------
# Build Type

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# -----------------------------------------------------------------------
# OpenMP
option(SAENA_USE_OPENMP OFF)

if(SAENA_USE_OPENMP)
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        add_definitions(-D_USE_OPENMP_)
    endif()
    add_definitions(-DSAENA_USE_OPENMP)
endif()

# -----------------------------------------------------------------------
# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${MPI_LIBRARIES})

# -----------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------

include(ExternalProject)

# SuperLU
ExternalProject_Add(superlu
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SuperLU_DIST_5.4.0
  BINARY_DIR ${CMAKE_BINARY_DIR}/superlu-build
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONIGURE_COMMAND ${CMAKE_EXECUTABLE} ${SuperLU_DIST_DIR} -Denable_blaslib=OFF -DCMAKE_INSTALL_PREFIX=.
  )

set(SuperLU_DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SuperLU_DIST_5.4.0)
set(SuperLU_DIST_LIB ${CMAKE_BINARY_DIR}/superlu-build/lib/libsuperlu_dist.a)
include_directories(
        ${SuperLU_DIST_DIR}/build/include
        ${SuperLU_DIST_DIR}/SRC
)

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${SuperLU_DIST_LIB})

# -----------------------------------------------------------------------
# METIS and PARMETIS

ExternalProject_Add(metis
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3/metis
  BINARY_DIR ${CMAKE_BINARY_DIR}/metis-build
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND make config -C ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3/metis prefix=${CMAKE_BINARY_DIR}/metis-build
  BUILD_COMMAND cd ${CMAKE_BINARY_DIR}/metis-build/build_metis && make
  INSTALL_COMMAND cd ${CMAKE_BINARY_DIR}/metis-build/build_metis && make install
  )
ExternalProject_Add(parmetis
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3
  BINARY_DIR ${CMAKE_BINARY_DIR}/parmetis-build
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND make config -C ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3 prefix=${CMAKE_BINARY_DIR}/parmetis-build
  BUILD_COMMAND cd ${CMAKE_BINARY_DIR}/parmetis-build/build_parmetis && make
  INSTALL_COMMAND cd ${CMAKE_BINARY_DIR}/parmetis-build/build_parmetis && make install
  )

#set(PARMETIS_ROOT_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3)
set(METIS_LIBRARY_SAENA    ${CMAKE_BINARY_DIR}/metis-build/build_metis/lib/libmetis.so)
set(PARMETIS_LIBRARY_SAENA ${CMAKE_BINARY_DIR}/parmetis-build/build_parmetis/lib/libparmetis.a)
#set(METIS_INCLUDE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/build/build_metis/include)
#set(PARMETIS_INCLUDE_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/build/build_parmetis/include)

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${PARMETIS_LIBRARY_SAENA} ${METIS_LIBRARY_SAENA})

# -----------------------------------------------------------------------
# BLAS

find_package(BLAS REQUIRED)
message(STATUS BLAS found: ${BLAS_LIBRARIES})

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${BLAS_LIBRARIES})

# -----------------------------------------------------------------------
# LAPACK

find_package(LAPACK REQUIRED)
message( STATUS LAPACK found: ${LAPACK_LIBRARIES} )
include_directories(${LAPACK_INCLUDE_PATH})

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${LAPACK_LIBRARIES})

# -----------------------------------------------------------------------
# MKL

#if(NOT DEFINED ENV{MKLROOT})
#    message( FATAL_ERROR "MKLROOT is not set! CMake will exit." )
#endif(NOT DEFINED ENV{MKLROOT})

#find_package(MKL REQUIRED)
#include_directories(${MKL_INCLUDE_DIRS})
#SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${MKL_CORE_LIBRARY})

# -----------------------------------------------------------------------
# IETL: eigensolver

#include_directories(external/ietl)
#set(SAENA_HEADER_FILES ${SAENA_HEADER_FILES}
#        include/ietl_saena.h
#        )

# -----------------------------------------------------------------------
# Lambda Lanczos: eigensolver
# https://github.com/mrcdr/lambda-lanczos/

set(LAMLAN_DIR external/lambda_lanczos)
include_directories(${LAMLAN_DIR}/include/lambda_lanczos)
set(SAENA_HEADER_FILES ${SAENA_HEADER_FILES}
        ${SAENA_INCLUDE_DIR}/lamlan_saena.h
        )

# -----------------------------------------------------------------------
# usort: to sort elements in parallel

include_directories(external/usort/include)
set(SAENA_SOURCE_FILES ${SAENA_SOURCE_FILES}
        external/usort/src/binUtils.cpp
        external/usort/src/parUtils.cpp
        external/usort/src/sort_profiler.cpp
        )

# -----------------------------------------------------------------------
# pugixml: to read xml files for options

include_directories(external/pugixml/include)

set(SAENA_SOURCE_FILES ${SAENA_SOURCE_FILES}
        external/pugixml/src/pugixml.cpp
        )

# -----------------------------------------------------------------------
# zfp: for compressing arrays

include(GNUInstallDirs)

option(ENABLE_ZFP OFF)

if(ENABLE_ZFP)
    set(ZFP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/zfp)
    set(ZFP_LIB ${CMAKE_CURRENT_BINARY_DIR}/build_zfp/${CMAKE_INSTALL_LIBDIR}/libzfp.so)
    include_directories(
            ${ZFP_DIR}/array
            ${ZFP_DIR}/array/zfp
            ${ZFP_DIR}/include
    )
    SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${ZFP_LIB})
    add_definitions(-DSAENA_USE_ZFP)
endif(ENABLE_ZFP)

# -----------------------------------------------------------------------
#PETSc

option(ENABLE_PETSC OFF)

set(PETSC_DIR $ENV{PETSC_DIR})
set(PETSC_ARCH $ENV{PETSC_ARCH})

if(ENABLE_PETSC)
    if(NOT DEFINED ENV{PETSC_DIR})
        message( FATAL_ERROR "PETSC_DIR is not set! CMake will exit." )
    endif(NOT DEFINED ENV{PETSC_DIR})

    if(NOT DEFINED ENV{PETSC_ARCH})
        message( FATAL_ERROR "PETSC_ARCH is not set! CMake will exit." )
    endif(NOT DEFINED ENV{PETSC_ARCH})

#    set(PETSC_DIR $ENV{PETSC_DIR})
#    set(PETSC_ARCH $ENV{PETSC_ARCH})
    message(--\ PETSC_DIR:\ ${PETSC_DIR})
    message(--\ PETSC_ARCH:\ ${PETSC_ARCH})

    set(PETSC_LIB ${PETSC_DIR}/${PETSC_ARCH}/lib/libpetsc.so)
    include_directories(
            ${PETSC_DIR}/include
            ${PETSC_DIR}/${PETSC_ARCH}/include
    )

    SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${PETSC_LIB})

    set(SAENA_HEADER_FILES ${SAENA_HEADER_FILES} include/petsc_functions.h)
    set(SAENA_SOURCE_FILES ${SAENA_SOURCE_FILES} src/petsc_functions.cpp)

#    add_compile_definitions(_USE_PETSC_) // requires newer cmake versions
    add_definitions(-D_USE_PETSC_)
endif(ENABLE_PETSC)

# -----------------------------------------------------------------------
# dollar (profiler)

set(DOLLAR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/dollar")
include_directories(
        ${DOLLAR_DIR}
)

# -----------------------------------------------------------------------
# trsl: for sampling

#set(TRSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/trsl-0.2.2)
#include_directories(
#        ${TRSL_DIR}
#)

# -----------------------------------------------------------------------
# Main Files

include_directories(include)

set(SAENA_HEADER_FILES ${SAENA_HEADER_FILES}
        include/saena.hpp
        include/aux_functions.h
        include/aux_functions2.h
        include/data_struct.h
        include/saena_matrix.h
        include/saena_matrix.tpp
        include/saena_matrix_dense.h
        include/saena_object.h
        include/saena_object.tpp
        include/strength_matrix.h
        include/prolong_matrix.h
        include/restrict_matrix.h
        include/grid.h
        include/saena_vector.h
        include/GR_encoder.h
        )

set(SAENA_SOURCE_FILES ${SAENA_SOURCE_FILES}
        src/saena.cpp
        src/aux_functions.cpp
        src/aux_functions2.cpp
        src/data_struct.cpp
        src/grid.cpp
        src/strength_matrix.cpp
        src/prolong_matrix.cpp
        src/restrict_matrix.cpp
        src/saena_matrix_dense.cpp
        src/saena_vector.cpp
        src/GR_encoder.cpp
        src/saena_matrix.cpp
        src/saena_matrix_repart.cpp
        src/saena_matrix_setup.cpp
        src/saena_matrix_matvec.cpp
        src/saena_matrix_shrink.cpp
        src/saena_matrix_zfp.cpp
        src/saena_object.cpp
        src/saena_object_setup1.cpp
        src/saena_object_setup2.cpp
        src/saena_object_pcoarsen_modal.cpp
        src/saena_object_setup_matmat.cpp
        src/saena_object_solve.cpp
        src/saena_object_lazy.cpp
        src/saena_object_repart_shrink.cpp
        src/saena_object_sparsify.cpp
        src/saena_object_write2file.cpp
        )

# -----------------------------------------------------------------------

# Install Saena with rpath modified
# Install Saena inside the ThirdParty libraris in Nektar++.
# The following command tells where saena should look for its dependencies
set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/build-metis/build_metis/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/build/build_zfp/${CMAKE_INSTALL_LIBDIR}")

add_library(saena SHARED ${SAENA_SOURCE_FILES})
target_link_libraries(saena ${SAENA_LIB_DEPENDENCIES})

install(TARGETS saena DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/ThirdParty/dist/lib)
