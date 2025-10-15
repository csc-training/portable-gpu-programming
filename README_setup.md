# Installations

*Here are instructions how the compilation environments used in the course were created.*

## AdaptiveCpp on Mahti

### Compile from source

Setup the environment:
```
module load gcc/10.4.0
module load cuda/12.6.0
```

First one has compile LLVM with nvidia support:
```
git clone https://github.com/llvm/llvm-project -b release/20.x
cd llvm-project
mkdir -p build
cd build
INSTALL_PREFIX=/projappl/project_2015315/apps/LLVM cmake  -DCMAKE_C_COMPILER=`which gcc` -DCMAKE_CXX_COMPILER=`which g++` -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DLLVM_ENABLE_PROJECTS="clang;lld;openmp" -DLLVM_ENABLE_RUNTIMES=compiler-rt -DOPENMP_ENABLE_LIBOMPTARGET=OFF -DLLVM_ENABLE_ASSERTIONS=OFF -DLLVM_TARGETS_TO_BUILD="AMDGPU;NVPTX;X86" -DLLVM_INCLUDE_BENCHMARKS=0 -DLLVM_INCLUDE_EXAMPLES=0 -DLLVM_INCLUDE_TESTS=0 -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON -DCMAKE_INSTALL_RPATH=$INSTALL_PREFIX/lib -DLLVM_ENABLE_OCAMLDOC=OFF -DLLVM_ENABLE_BINDINGS=OFF -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=OFF -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_ENABLE_DUMP=OFF ../llvm

```
The clang compiler provided by the LLVM is used to compile AdaptiveCpp:
```
git clone https://github.com/AdaptiveCpp/AdaptiveCpp.git
cd AdaptiveCpp
mkdir build
cd build
cmake  -DCMAKE_INSTALL_PREFIX=/projappl/project_2015315/apps/ACPP    -DBOOST_ROOT=/appl/spack/v020/install-tree/gcc-10.4.0/boost-1.82.0-ystwi2 -DCLANG_EXECUTABLE_PATH=/projappl/project_2015315/apps/LLVM/bin/clang -DCLANG_INCLUDE_PATH=/projappl/project_2015315/apps/LLVM/include/ -DLLVM_DIR=/projappl/project_2015315/apps/LLVM/lib/cmake/llvm -DACPP_COMPILER_FEATURE_PROFILE=full .. 
make -j
make install
```

## AdaptiveCpp on LUMI

### Compile from source

Load the modules needed:
```
module load LUMI/24.03
module load partition/G
module load rocm/6.2.2
```
Clone repository and Compile with both cpu and rocm support (the LLVM is provided by ROCM):
```
git clone https://github.com/AdaptiveCpp/AdaptiveCpp.git
cd AdaptiveCpp
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/projappl/project_462000752/ACPP/  -DROCM_PATH=$ROCM_PATH -DWITH_CPU_BACKEND=ON -DWITH_CUDA_BACKEND=OFF  -DWITH_ROCM_BACKEND=ON -DACPP_TARGETS="gfx90a"  -DWITH_ACCELERATED_CPU=ON -DWITH_SSCP_COMPILER=OFF  -DWITH_OPENCL_BACKEND=OFF -DWITH_LEVEL_ZERO_BACKEND=OFF -DBOOST_ROOT=/appl/lumi/SW/LUMI-24.03/G/EB/Boost/1.83.0-cpeGNU-24.03/ ..
make -j 64 
make install 
```

## Intel OneAPI on WSL:

Please follow the instructions from this [Intel Page](https://www.intel.com/content/www/us/en/docs/oneapi/installation-guide-linux/2024-0/configure-wsl-2-for-gpu-workflows.html)
