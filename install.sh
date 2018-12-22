if [ ! -d "build" ]; then
    mkdir build
fi
cd build
make config prefix=`pwd` -C ../external/parmetis-4.0.3/metis
cd build_metis
make install
cd ..
make config prefix=`pwd` -C ../external/parmetis-4.0.3
cd build_parmetis
make install
cd ..
mkdir build_zfp && cd build_zfp
cmake ../../external/zfp-0.5.3
make -j28
cd ..
mkdir build_superlu && cd build_superlu
cmake ../../external/SuperLU_DIST_5.4.0 \
-DXSDK_INDEX_SIZE=64 \
-Denable_blaslib=OFF \
-DCMAKE_INSTALL_PREFIX=. ;\
make -j28 install
cd ../..
