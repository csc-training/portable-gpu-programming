# Installations

*Here are instructions how the compilation environments used in the course were created.*

## LUMI ROCm container with hipstdpar and AdaptiveCpp

This container is built with recipe [here](https://github.com/trossi/containers/tree/main/examples/rocm_acpp).

## OneAPI on Mahti

Download [Intel oneAPI base toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html?packages=oneapi-toolkit&oneapi-toolkit-os=linux&oneapi-lin=offline):

    wget https://registrationcenter-download.intel.com/akdlm/IRC_NAS/96aa5993-5b22-4a9b-91ab-da679f422594/intel-oneapi-base-toolkit-2025.0.0.885_offline.sh

Install:

    sh ./intel-oneapi-base-toolkit-2025.0.0.885_offline.sh -a --silent --cli --eula accept --download-cache $SCRATCH/$USER/oneapi_tmp --install-dir $PROJAPPL/intel/oneapi

Get [Codeplay oneAPI for NVIDIA GPUs](https://developer.codeplay.com/products/oneapi/nvidia/2025.0.0/guides/get-started-guide-nvidia#installation):

    curl -LOJ "https://developer.codeplay.com/api/v1/products/download?product=oneapi&variant=nvidia&version=2025.0.0&filters[]=12.0&filters[]=linux"

Install:

    sh ./oneapi-for-nvidia-gpus-2025.0.0-cuda-12.0-linux.sh -y --extract-folder $SCRATCH/$USER/oneapi_tmp --install-dir $PROJAPPL/intel/oneapi

## OneAPI on LUMI

Download [Intel oneAPI base toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html?packages=oneapi-toolkit&oneapi-toolkit-os=linux&oneapi-lin=offline):

    wget https://registrationcenter-download.intel.com/akdlm/IRC_NAS/96aa5993-5b22-4a9b-91ab-da679f422594/intel-oneapi-base-toolkit-2025.0.0.885_offline.sh

Install:

    sh ./intel-oneapi-base-toolkit-2025.0.0.885_offline.sh -a --silent --cli --eula accept --download-cache $SCRATCH/$USER/oneapi_tmp --install-dir $PROJAPPL/intel/oneapi

Get [Codeplay oneAPI for AMD GPUs](https://developer.codeplay.com/products/oneapi/amd/2025.0.0/guides/get-started-guide-amd#installation):

    curl -LOJ "https://developer.codeplay.com/api/v1/products/download?product=oneapi&variant=amd&version=2025.0.0&filters[]=6.0.2&filters[]=linux"

Install:

    sh ./oneapi-for-amd-gpus-2025.0.0-rocm-6.0.2-linux.sh -y --extract-folder $SCRATCH/$USER/oneapi_tmp --install-dir $PROJAPPL/intel/oneapi

## AdaptiveCpp on Mahti

### Spack Instructions  
```
module purge
git clone -c feature.manyFiles=true https://github.com/spack/spack.git
cd spack
 . share/spack/setup-env.sh
spack bootstrap now
spack compiler find
spack config add "modules:default:enable:[tcl]"
spack install lmod
$(spack location -i lmod)/lmod/lmod/init/bash
. share/spack/setup-env.sh
module load gcc/10.4.0
spack compiler find
```
Edit the recipe var/spack/repos/builtin/packages/hipsycl/package.py. Add "-DWITH_ACCELERATED_CPU:Bool=TRUE", after line 84. Edit the recipe var/spack/repos/builtin/packages/llvmpackage.py, remove all the versions higher than 18.
```
spack install hipsycl@24.06  %gcc@10.4.0 + cuda
```

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


Load the modules needed:
```
module load LUMI/24.03
module load partition/G
module load rocm/6.2.2
```
Clone repository and Compile with both cpu and rocm support:
```
git clone https://github.com/AdaptiveCpp/AdaptiveCpp.git
cd AdaptiveCpp
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/projappl/project_462000752/ACPP/  -DROCM_PATH=$ROCM_PATH -DWITH_CPU_BACKEND=ON -DWITH_CUDA_BACKEND=OFF  -DWITH_ROCM_BACKEND=ON -DACPP_TARGETS="gfx90a"  -DWITH_ACCELERATED_CPU=ON -DWITH_SSCP_COMPILER=OFF  -DWITH_OPENCL_BACKEND=OFF -DWITH_LEVEL_ZERO_BACKEND=OFF -DBOOST_ROOT=/appl/lumi/SW/LUMI-24.03/G/EB/Boost/1.83.0-cpeGNU-24.03/ ..
make -j 64 
make install 
```
