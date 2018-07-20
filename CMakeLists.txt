# execute this in the Saena source directory:
#mkdir build; cd build; \
#mkdir build_zfp; cd build_zfp; \
#cmake ../../external/zfp-0.5.3; \
#make -j24; \
#cd ..; \
#mkdir build_superlu; cd build_superlu; \
#cmake ../../external/SuperLU_DIST_5.4.0 \
#-DTPL_PARMETIS_INCLUDE_DIRS="${PARMETIS_ROOT}/include;${PARMETIS_ROOT}/metis/include" \
#-DTPL_PARMETIS_LIBRARIES="${PARMETIS_BUILD_DIR}/libparmetis/libparmetis.a;${PARMETIS_BUILD_DIR}/libmetis/libmetis.a" \
#-Denable_blaslib=OFF \
#-DCMAKE_INSTALL_PREFIX=. ;\
#make -j24 install; \
#cd ..; \
#cmake ..; \
#make -j24

# GFORTRAN_LIB="/usr/lib64/libgfortran.so.3" ..

cmake_minimum_required(VERSION 3.3)
project(Saena)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------
# OpenMP

find_package(OpenMP)
if(OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# end of OpenMP
# -----------------------------------------------------------------------
# MPI - part 1/2

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# end of MPI - part 1/2
# -----------------------------------------------------------------------
# BLAS

find_package(BLAS REQUIRED)
message(STATUS BLAS found: ${BLAS_LIBRARIES})

# BLAS_INCLUDE_DIR does not get found by find_package(BLAS), so find it by the following command:
#find_path(BLAS_INCLUDE_DIR cblas.h
#        /usr/include
#        /usr/local/include
#        $ENV{BLAS_HOME}/include)

#message(STATUS BLAS_include found: ${BLAS_INCLUDE_DIR})
#include_directories(${BLAS_INCLUDE_DIR})

# end of BLAS
# -----------------------------------------------------------------------
# LAPACK

#find_package(LAPACK REQUIRED)
#message( STATUS LAPACK found: ${LAPACK_LIBRARIES} )
#include_directories(${LAPACK_INCLUDE_PATH})
#set(LAPACK_LIB /usr/lib/openblas-base/liblapack.a)

# end of LAPACK
# -----------------------------------------------------------------------
# SuperLU

set(SuperLU_DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SuperLU_DIST_5.4.0)
set(SuperLU_DIST_LIB ${CMAKE_BINARY_DIR}/build_superlu/lib/libsuperlu_dist.a)
include_directories(
        ${SuperLU_DIST_DIR}/build/include
        ${SuperLU_DIST_DIR}/SRC
)

# end of SuperLU
# -----------------------------------------------------------------------
# IETL: eigensolver

include_directories(external/ietl)

set(HEADER_FILES ${HEADER_FILES}
        include/ietl_saena.h
        )

# end of IETL
# -----------------------------------------------------------------------
# usort: to sort elements in parallel

include_directories(external/usort/include)

#set(HEADER_FILES ${HEADER_FILES}
#        external/usort/include/binUtils.h
#        external/usort/include/dtypes.h
#        external/usort/include/indexHolder.h
#        external/usort/include/ompUtils.h
#        external/usort/include/ompUtils.tcc
#        external/usort/include/parUtils.h
#        external/usort/include/parUtils.tcc
#        external/usort/include/seqUtils.h
#        external/usort/include/seqUtils.tcc
#        external/usort/include/sort_profiler.h
#        )

set(SOURCE_FILES ${SOURCE_FILES}
        external/usort/src/binUtils.cpp
        external/usort/src/parUtils.cpp
        external/usort/src/sort_profiler.cpp
        )

# end of usort
# -----------------------------------------------------------------------
# pugixml: to read xml files for options

include_directories(external/pugixml/include)

set(SOURCE_FILES ${SOURCE_FILES}
        external/pugixml/src/pugixml.cpp
        )

# end of pugixml
# -----------------------------------------------------------------------
# zfp: for compressing arrays

set(ZFP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/zfp-0.5.3)
set(ZFP_LIB ${CMAKE_CURRENT_BINARY_DIR}/build_zfp/lib/libzfp.so)
include_directories(
        ${ZFP_DIR}/array
        ${ZFP_DIR}/array/zfp
        ${ZFP_DIR}/include
)

#set(HEADER_FILES ${HEADER_FILES}
#        external/zfp/array/zfparray.h
#        external/zfp/array/zfparray1.h
#        external/zfp/array/zfparray2.h
#        external/zfp/array/zfparray3.h
#        external/zfp/array/zfpcodec.h
#        external/zfp/array/zfpcodecd.h
#        external/zfp/array/zfpcodecf.h
#        external/zfp/array/zfp/cache.h
#        external/zfp/array/zfp/memory.h
#        )

# end of zfp
# -----------------------------------------------------------------------
# Metis/Parmetis

if(NOT PARMETIS_INCLUDE_DIR)
    find_path(PARMETIS_INCLUDE_DIR parmetis.h
            HINTS ${PARMETIS_INCLUDE_DIR} ENV PARMETIS_INCLUDE_DIR ${PARMETIS_DIR} ENV PARMETIS_DIR
            PATH_SUFFIXES include
            DOC "Directory where the ParMETIS header files are located"
            )
endif()

if(NOT METIS_INCLUDE_DIR)
    find_path(METIS_INCLUDE_DIR metis.h
            HINTS ${METIS_INCLUDE_DIR} ENV METIS_INCLUDE_DIR ${METIS_DIR} ENV METIS_DIR
            PATH_SUFFIXES include
            DOC "Directory where the METIS header files are located"
            )
endif()

if(PARMETIS_LIBRARIES)
    set(PARMETIS_LIBRARY ${PARMETIS_LIBRARIES})
endif()
if(NOT PARMETIS_LIBRARY)
    find_library(PARMETIS_LIBRARY
            NAMES parmetis parmetis${PARMETIS_LIB_SUFFIX}
            HINTS ${PARMETIS_LIB_DIR} ENV PARMETIS_LIB_DIR ${PARMETIS_DIR} ENV PARMETIS_DIR
            PATH_SUFFIXES lib
            DOC "Directory where the ParMETIS library is located"
            )
endif()

if(METIS_LIBRARIES)
    set(METIS_LIBRARY ${METIS_LIBRARIES})
endif()
if(NOT METIS_LIBRARY)
    find_library(METIS_LIBRARY
            NAMES metis metis${PARMETIS_LIB_SUFFIX}
            HINTS ${PARMETIS_LIB_DIR} ENV PARMETIS_LIB_DIR ${PARMETIS_DIR} ENV PARMETIS_DIR
            PATH_SUFFIXES lib
            DOC "Directory where the METIS library is located"
            )
endif()

# end of Metis/Parmetis
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
        src/main.cpp
        src/saena_matrix.cpp
        src/saena_matrix_dense.cpp
        src/saena_object.cpp
        src/strength_matrix.cpp
        src/prolong_matrix.cpp
        src/restrict_matrix.cpp
        src/aux_functions.cpp
        src/grid.cpp
        src/saena.cpp
        )

# end of Main Files
# -----------------------------------------------------------------------

add_executable(Saena ${SOURCE_FILES} ${HEADER_FILES})

# -----------------------------------------------------------------------
# MPI - part 2/2

if(MPI_COMPILE_FLAGS)
    set_target_properties(Saena PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(Saena PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

# end of MPI - part 2/2
# -----------------------------------------------------------------------

target_link_libraries(Saena ${MPI_LIBRARIES} ${SuperLU_DIST_LIB} ${BLAS_LIBRARIES} ${METIS_LIBRARY} ${PARMETIS_LIBRARY} m ${ZFP_LIB})

# -----------------------------------------------------------------------
# NEKTAR++

ADD_NEKTAR_LIBRARY(Saena
    SOURCES ${SOURCE_FILES}
    HEADERS ${HEADER_FILES}
    SUMMARY "Nektar++ Saena fast multigrid solver library"
    DESCRIPTION "This library provides fast multigrid solves.")
