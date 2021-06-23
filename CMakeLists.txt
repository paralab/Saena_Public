cmake_minimum_required(VERSION 3.3)
project(Saena)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------
# Build Type

IF(NOT CMAKE_BUILD_TYPE)
        SET(CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -O3 -axCORE-AVX512,CORE-AVX2,AVX -xSSE4.2 -g -parallel-source-info=2 -shared-intel")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -axCORE-AVX512,CORE-AVX2,AVX -xSSE4.2 -g -parallel-source-info=2 -shared-intel")
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-D__DEBUG1__)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
ENDIF()

#message(--\ Saena:\ CMAKE_BUILD_TYPE:\ ${CMAKE_BUILD_TYPE})

# -----------------------------------------------------------------------
# BOOST

#find_package(BOOST REQUIRED)
#message(STATUS BOOST found: ${BOOST_LIBRARIES})
#include_directories(${Boost_INCLUDE_DIRS})

# first: module load boost
#set(BOOST_INC_DIR $ENV{TACC_BOOST_INC})
#include_directories(${BOOST_INC_DIR})

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
# SuperLU

set(SuperLU_DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SuperLU_DIST_5.4.0)
set(SuperLU_DIST_LIB ${CMAKE_CURRENT_SOURCE_DIR}/build/build_superlu/lib/libsuperlu_dist.a)
include_directories(
        ${SuperLU_DIST_DIR}/build/include
        ${SuperLU_DIST_DIR}/SRC
)

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${SuperLU_DIST_LIB})

# -----------------------------------------------------------------------
# METIS and PARMETIS

#set(PARMETIS_ROOT_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3)
set(METIS_LIBRARY_SAENA    ${CMAKE_CURRENT_SOURCE_DIR}/build/build_metis/lib/libmetis.so)
set(PARMETIS_LIBRARY_SAENA ${CMAKE_CURRENT_SOURCE_DIR}/build/build_parmetis/lib/libparmetis.a)
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
#set(LAPACK_LIB /usr/lib/openblas-base/liblapack.a)

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${LAPACK_LIBRARIES})

# -----------------------------------------------------------------------
# MKL

set(MKLROOT $ENV{MKLROOT})
include_directories(${MKLROOT}/include)

set(MKL_LIB_DIR    "${MKLROOT}/lib/intel64_lin")

set(MKL_LIB_CORE   "${MKL_LIB_DIR}/libmkl_core.so")
set(MKL_LIB_LP64   "${MKL_LIB_DIR}/libmkl_intel_lp64.so")

set(MKL_LIB_THREAD "${MKL_LIB_DIR}/libmkl_gnu_thread.so")
#set(MKL_LIB_THREAD "${MKL_LIB_DIR}/libmkl_intel_thread.so")

#set(MKL_LIB_BLACS "${MKL_LIB_DIR}/libmkl_blacs_intelmpi_lp64.so")
#set(MKL_LIB_BLACS "${MKL_LIB_DIR}/libmkl_blacs_openmpi_lp64.so")

#set(MKL_OMP "/opt/intel/compilers_and_libraries_2020.0.166/linux/compiler/lib/intel64_lin/libiomp5.so")

SET(SAENA_LIB_DEPENDENCIES ${SAENA_LIB_DEPENDENCIES} ${MKL_LIB_CORE} ${MKL_LIB_LP64} ${MKL_LIB_THREAD})

# -----------------------------------------------------------------------
# IETL: eigensolver

include_directories(external/ietl)

set(SAENA_HEADER_FILES ${SAENA_HEADER_FILES}
        include/ietl_saena.h
        )

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
# NEKTAR++

#ADD_NEKTAR_LIBRARY(Saena
#        SOURCES ${SAENA_SOURCE_FILES}
#        HEADERS ${SAENA_HEADER_FILES}
#        SUMMARY "Nektar++ Saena fast multigrid solver library"
#        DESCRIPTION "This library provides fast multigrid solves.")

# -----------------------------------------------------------------------

# Install Saena with rpath modified
# Install Saena inside the ThirdParty libraris in Nektar++.
# The following command tells where saena should look for its dependencies
set(CMAKE_INSTALL_RPATH "/usr/local/lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/build_metis/lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/build/build_zfp/${CMAKE_INSTALL_LIBDIR}")

add_library(saena SHARED ${SAENA_SOURCE_FILES})
target_link_libraries(saena ${SAENA_LIB_DEPENDENCIES})
install(TARGETS saena DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/ThirdParty/dist/lib)
