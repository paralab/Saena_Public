cmake_minimum_required(VERSION 3.3)
project(Saena)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------
# Build Type

#IF(NOT CMAKE_BUILD_TYPE)
#        SET(CMAKE_BUILD_TYPE Release)
#ENDIF()

#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

#message(--\ Saena:\ CMAKE_BUILD_TYPE:\ ${CMAKE_BUILD_TYPE})

#IF(CMAKE_BUILD_TYPE STREQUAL Release)
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#ENDIF()

#IF(CMAKE_BUILD_TYPE STREQUAL Debug)
#    add_definitions(-D__DEBUG__)
#ENDIF()

# -----------------------------------------------------------------------
# OpenMP

find_package(OpenMP)
if(OPENMP_FOUND)
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# -----------------------------------------------------------------------
# MPI

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# -----------------------------------------------------------------------
# Dependencies

#set(PARMETIS_ROOT_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/external/parmetis-4.0.3)
set(METIS_LIBRARY_SAENA    ${CMAKE_CURRENT_SOURCE_DIR}/build/build_metis/lib/libmetis.a)
set(PARMETIS_LIBRARY_SAENA ${CMAKE_CURRENT_SOURCE_DIR}/build/build_parmetis/lib/libparmetis.a)
set(METIS_INCLUDE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/build/build_metis/include)
set(PARMETIS_INCLUDE_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/build/build_parmetis/include)

# -----------------------------------------------------------------------
# BLAS

find_package(BLAS REQUIRED)
message(STATUS BLAS found: ${BLAS_LIBRARIES})

# -----------------------------------------------------------------------
# LAPACK

find_package(LAPACK REQUIRED)
message( STATUS LAPACK found: ${LAPACK_LIBRARIES} )
include_directories(${LAPACK_INCLUDE_PATH})
#set(LAPACK_LIB /usr/lib/openblas-base/liblapack.a)

# -----------------------------------------------------------------------
# SuperLU

set(SuperLU_DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SuperLU_DIST_5.4.0)
set(SuperLU_DIST_LIB ${CMAKE_CURRENT_SOURCE_DIR}/build/build_superlu/lib/libsuperlu_dist.a)
include_directories(
        ${SuperLU_DIST_DIR}/build/include
        ${SuperLU_DIST_DIR}/SRC
)

# -----------------------------------------------------------------------
# IETL: eigensolver

include_directories(external/ietl)

set(HEADER_FILES ${HEADER_FILES}
        include/ietl_saena.h
        )

# -----------------------------------------------------------------------
# usort: to sort elements in parallel

include_directories(external/usort/include)
set(SOURCE_FILES ${SOURCE_FILES}
        external/usort/src/binUtils.cpp
        external/usort/src/parUtils.cpp
        external/usort/src/sort_profiler.cpp
        )

# -----------------------------------------------------------------------
# pugixml: to read xml files for options

include_directories(external/pugixml/include)

set(SOURCE_FILES ${SOURCE_FILES}
        external/pugixml/src/pugixml.cpp
        )

# -----------------------------------------------------------------------
# zfp: for compressing arrays

set(ZFP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/zfp-0.5.3)
set(ZFP_LIB ${CMAKE_CURRENT_SOURCE_DIR}/build/build_zfp/lib/libzfp.so)
include_directories(
        ${ZFP_DIR}/array
        ${ZFP_DIR}/array/zfp
        ${ZFP_DIR}/include
)

# -----------------------------------------------------------------------
#PETSc

#set(PETSC_DIR "~/softwares/petsc-3.8.4")
#set(PETSC_LIB ${PETSC_DIR}/arch-linux2-c-debug/lib/libpetsc.so)
#include_directories(
#        ${PETSC_DIR}/include
#        ${PETSC_DIR}/arch-linux2-c-debug/include
#)

# -----------------------------------------------------------------------
# dollar (profiler)

set(DOLLAR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/dollar")
include_directories(
        ${DOLLAR_DIR}
)

# -----------------------------------------------------------------------
# trsl: for sampling

set(TRSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/trsl-0.2.2)
include_directories(
        ${TRSL_DIR}
)

# -----------------------------------------------------------------------
# Main Files

include_directories(include)

set(HEADER_FILES ${HEADER_FILES}
        include/saena.hpp
        include/saena_matrix.h
        include/saena_matrix_dense.h
        include/saena_object.h
        include/strength_matrix.h
        include/prolong_matrix.h
        include/restrict_matrix.h
        include/aux_functions.h
        include/grid.h
        )

set(SOURCE_FILES ${SOURCE_FILES}
        ${HEADER_FILES}
        src/saena.cpp
        src/saena_matrix_dense.cpp
        src/strength_matrix.cpp
        src/prolong_matrix.cpp
        src/restrict_matrix.cpp
        src/aux_functions.cpp
        src/grid.cpp

        src/saena_matrix.cpp
        src/saena_matrix_repart.cpp
        src/saena_matrix_setup.cpp
        src/saena_matrix_matvec.cpp
        src/saena_matrix_shrink.cpp

        src/saena_object.cpp
        src/saena_object_setup1.cpp
        src/saena_object_setup2.cpp
        src/saena_object_solve.cpp
        src/saena_object_lazy.cpp
        src/saena_object_repart_shrink.cpp
        src/saena_object_sparsify.cpp
        src/saena_object_elemental.cpp
        src/saena_object_write2file.cpp
        src/saena_object_setup2_test.cpp
        )

# -----------------------------------------------------------------------

#add_executable(Saena src/main.cpp ${SOURCE_FILES} ${HEADER_FILES})

# -----------------------------------------------------------------------
# NEKTAR++

ADD_NEKTAR_LIBRARY(Saena
        SOURCES ${SOURCE_FILES}
        HEADERS ${HEADER_FILES}
        SUMMARY "Nektar++ Saena fast multigrid solver library"
        DESCRIPTION "This library provides fast multigrid solves.")

# -----------------------------------------------------------------------

target_link_libraries(Saena ${MPI_LIBRARIES} ${SuperLU_DIST_LIB} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${PARMETIS_LIBRARY_SAENA} ${METIS_LIBRARY_SAENA} m ${ZFP_LIB})
