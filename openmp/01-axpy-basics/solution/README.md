## Discussion

### Exercises: Offload to GPU

1. See the solution code.

2. We enable runtime debugging and run the program:

       export CRAY_ACC_DEBUG=2
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   Output:

       ACC: Version 6.0 of HIP already initialized, runtime version 60032831
       ACC: Get Device 0
       ACC: Set Thread Context
       ACC: Start transfer 2 items from axpy.c:30
       ACC:       allocate, copy to acc 'y' (819200 bytes)
       ACC:       allocate, copy to acc 'x' (819200 bytes)
       ACC: End transfer (to acc 1638400 bytes, to host 0 bytes)
       ACC: Execute kernel __omp_offloading_73ac72ce_2c01b06c_main_l30_cce$noloop$form blocks:400 threads:256 from axpy.c:30
       ACC: Start transfer 2 items from axpy.c:30
       ACC:       copy to host, free 'x' (819200 bytes)
       ACC:       copy to host, free 'y' (819200 bytes)
       ACC: End transfer (to acc 0 bytes, to host 1638400 bytes)
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Here the debugging prints show memory allocations and transfers as well as the execution of
   the kernel using 400 blocks and 256 threads per block, that is, 102400 threads in total.

3. Here is the output for `CRAY_ACC_DEBUG=1`:

       ACC: Transfer 2 items (to acc 1638400 bytes, to host 0 bytes) from axpy.c:30
       ACC: Execute kernel __omp_offloading_73ac72ce_2c01b06c_main_l30_cce$noloop$form from axpy.c:30
       ACC: Transfer 2 items (to acc 0 bytes, to host 1638400 bytes) from axpy.c:30
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   And for `CRAY_ACC_DEBUG=3`:

       ACC: Version 6.0 of HIP already initialized, runtime version 60032831
       ACC: __tgt_register_requires: flags = NONE
       ACC: __tgt_register_lib
       ACC:   NumDeviceImages=1
       ACC:   Device Images:
       ACC:   Image location: 0x200c08 - 0x2042d0
       ACC:   Processing valid image
       ACC:   NumEntries=1
       ACC:   Image entries:
       ACC:   __omp_offloading_73ac72ce_2c01b06c_main_l30
       ACC:     {
       ACC:         addr=0x200b30
       ACC:         size=0
       ACC:         flags=0
       ACC:     }
       ACC:   NumHostEntries=1
       ACC:   Host entries:
       ACC:   __omp_offloading_73ac72ce_2c01b06c_main_l30
       ACC:     {
       ACC:         addr=0x200b30
       ACC:         size=0
       ACC:         flags=0
       ACC:     }
       ACC: __tgt_target_kernel(device_id=-1, host_ptr=0x200b30, arg_num=3, nowait=0)
       ACC: __internal_tgt_target_teams(device_id=-1, host_ptr=0x200b30, arg_num=3, num_teams=0, thread_limit=0)
       ACC: Get Device 0
       ACC: Compute level 9.0
       ACC: Device Name: AMD Instinct MI250X
       ACC: Number of cus 110
       ACC: Device name AMD Instinct MI250X
       ACC: AMD GCN arch name: gfx90a:sramecc+:xnack-
       ACC: Max shared memory 65536
       ACC: Max thread blocks per cu 8
       ACC: Max concurrent kernels 8
       ACC: Async table size 8
       ACC: Total GPU memory 68702699520
       ACC: Available GPU memory 68625104896
       ACC: Set Thread Context
       ACC: Establish link bewteen libcrayacc and libcraymp
       ACC:   libcrayacc interface v7
       ACC:    libcraymp interface v7
       ACC:    loading module data
       ACC: __internal_tgt_target_teams(device_id=-1, host_ptr=0x200b30, arg_num=3, num_teams=400, thread_limit=256)
       ACC:   [0] 0x7fff6f82b250 base 0x7fff6f82b250 begin 0x7fff6f82b250 : 819200 bytes type=0x223 (TO FROM TARGET_PARAM IMPLICIT) name (y)
       ACC:   [1] 0x4008000000000000 base 0x4008000000000000 begin 0x4008000000000000 : 8 bytes type=0x320 (TARGET_PARAM LITERAL IMPLICIT) name (alpha)
       ACC:   [2] 0x7fff6f8f3250 base 0x7fff6f8f3250 begin 0x7fff6f8f3250 : 819200 bytes type=0x223 (TO FROM TARGET_PARAM IMPLICIT) name (x)
       ACC: Start transfer 2 items from axpy.c:30
       ACC:   flags:
       ACC: 
       ACC:   Trans 1
       ACC:       Simple transfer of 'y' (819200 bytes)
       ACC:            host ptr 7fff6f82b250
       ACC:            acc  ptr 0
       ACC:            flags: ALLOCATE COPY_HOST_TO_ACC ACQ_PRESENT REG_PRESENT
       ACC:            memory not found in present table
       ACC:            allocate (819200 bytes)
       ACC:              get new reusable memory, added entry
       ACC:            new allocated ptr (14d2d2200000)
       ACC:            add to present table index 0: host 7fff6f82b250 to 7fff6f8f3250, acc 14d2d2200000
       ACC:            copy host to acc (7fff6f82b250 to 14d2d2200000)
       ACC:                internal copy host to acc (host 7fff6f82b250 to acc 14d2d2200000) size = 819200
       ACC:            new acc ptr 14d2d2200000
       ACC: 
       ACC:   Trans 2
       ACC:       Simple transfer of 'x' (819200 bytes)
       ACC:            host ptr 7fff6f8f3250
       ACC:            acc  ptr 0
       ACC:            flags: ALLOCATE COPY_HOST_TO_ACC ACQ_PRESENT REG_PRESENT
       ACC:            memory not found in present table
       ACC:            allocate (819200 bytes)
       ACC:              get new reusable memory, added entry
       ACC:            new allocated ptr (14d2d22c8000)
       ACC:            add to present table index 1: host 7fff6f8f3250 to 7fff6f9bb250, acc 14d2d22c8000
       ACC:            copy host to acc (7fff6f8f3250 to 14d2d22c8000)
       ACC:                internal copy host to acc (host 7fff6f8f3250 to acc 14d2d22c8000) size = 819200
       ACC:            new acc ptr 14d2d22c8000
       ACC: 
       ACC: End transfer (to acc 1638400 bytes, to host 0 bytes)
       ACC: 
       ACC: Kernel Arguments
       ACC: 0: 22895701000192 ptr 0x14d2d2200000 (offset=0)
       ACC: 1: 4613937818241073152 ptr 0x4008000000000000 (offset=0)
       ACC: 2: 22895701819392 ptr 0x14d2d22c8000 (offset=0)
       ACC: Start kernel __omp_offloading_73ac72ce_2c01b06c_main_l30_cce$noloop$form from axpy.c:30
       ACC:        flags: CACHE_MOD CACHE_FUNC
       ACC:    mod cache:  0x34bf88
       ACC: kernel cache:  0x34cc38
       ACC:   async info:  (nil)
       ACC:    arguments: GPU argument info
       ACC:            param size:  24
       ACC:         param pointer:  0x94e2e0
       ACC:       blocks:  400
       ACC:      threads:  256
       ACC:     event id:  0
       ACC:    using cached module
       ACC:    getting function __omp_offloading_73ac72ce_2c01b06c_main_l30_cce$noloop$form
       ACC:       stats threads=1024 threadblocks per cu=4 shared=0 total shared=0
       ACC:       prefer equal shared memory and L1 cache 
       ACC:     kernel information
       ACC:               num registers :       50
       ACC:        max theads per block :     1024
       ACC:                 shared size :        0 bytes
       ACC:                  const size :        0 bytes
       ACC:                  local size :      296 bytes
       ACC: 
       ACC:     launching kernel new
       ACC:     synchronize
       ACC:     caching function
       ACC: End kernel
       ACC: 
       ACC:   [0] 0x7fff6f8f3250 base 0x7fff6f8f3250 begin 0x7fff6f8f3250 : 819200 bytes type=0x223 (TO FROM TARGET_PARAM IMPLICIT) name (x)
       ACC:   [1] 0x4008000000000000 base 0x4008000000000000 begin 0x4008000000000000 : 8 bytes type=0x320 (TARGET_PARAM LITERAL IMPLICIT) name (alpha)
       ACC:   [2] 0x7fff6f82b250 base 0x7fff6f82b250 begin 0x7fff6f82b250 : 819200 bytes type=0x223 (TO FROM TARGET_PARAM IMPLICIT) name (y)
       ACC: Start transfer 2 items from axpy.c:30
       ACC:   flags:
       ACC: 
       ACC:   Trans 1
       ACC:       Simple transfer of 'x' (819200 bytes)
       ACC:            host ptr 7fff6f8f3250
       ACC:            acc  ptr 0
       ACC:            flags: COPY_ACC_TO_HOST FREE REL_PRESENT REG_PRESENT INIT_ACC_PTR IGNORE_ABSENT
       ACC:            host region 7fff6f8f3250 to 7fff6f9bb250 found in present table index 1 (ref count 1)
       ACC:            last release acc 14d2d22c8000 from present table index 1 (ref_count 1)
       ACC:            last release of conditional present (acc 14d2d22c8000, base 14d2d22c8000)
       ACC:            copy acc to host (14d2d22c8000 to 7fff6f8f3250)
       ACC:                interal copy acc to host (acc 14d2d22c8000 to host 7fff6f8f3250) size = 819200
       ACC:            remove acc 14d2d22c8000 from present table index 1
       ACC:            new acc ptr 0
       ACC: 
       ACC:   Trans 2
       ACC:       Simple transfer of 'y' (819200 bytes)
       ACC:            host ptr 7fff6f82b250
       ACC:            acc  ptr 0
       ACC:            flags: COPY_ACC_TO_HOST FREE REL_PRESENT REG_PRESENT INIT_ACC_PTR IGNORE_ABSENT
       ACC:            host region 7fff6f82b250 to 7fff6f8f3250 found in present table index 0 (ref count 1)
       ACC:            last release acc 14d2d2200000 from present table index 0 (ref_count 1)
       ACC:            last release of conditional present (acc 14d2d2200000, base 14d2d2200000)
       ACC:            copy acc to host (14d2d2200000 to 7fff6f82b250)
       ACC:                interal copy acc to host (acc 14d2d2200000 to host 7fff6f82b250) size = 819200
       ACC:            remove acc 14d2d2200000 from present table index 0
       ACC:            new acc ptr 0
       ACC: 
       ACC: End transfer (to acc 0 bytes, to host 1638400 bytes)
       ACC: 
       ACC: __tgt_unregister_lib
       ACC: Start executing pending destructors
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   `CRAY_ACC_DEBUG=2` gives a good balance for the amount of details for the purposes of this training.

4. File axpy.lst is attached. The parallelized loop is marked in the output.

5. None of the other variations work expectedly. Either results are wrong (thread blocks are running for the same array indices instead of distributing the loop)
   or only a single thread might be used (very inefficient).


### Exercises: Offload to CPU threads

1. We execute the code:

       export OMP_DISPLAY_AFFINITY=true
       srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 -t 0:10:00 ./axpy.x

   Output:

       CCE OMP: host nid002275 pid 158201 tid 158201 thread 0 affinity:  0-3
       CCE OMP: host nid002275 pid 158201 tid 158201 thread 0 affinity:  0
       CCE OMP: host nid002275 pid 158201 tid 158204 thread 3 affinity:  3
       CCE OMP: host nid002275 pid 158201 tid 158202 thread 1 affinity:  1
       CCE OMP: host nid002275 pid 158201 tid 158203 thread 2 affinity:  2
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   This output shows that the program created 4 threads and that the threads were bound to individual cores.


### Bonus exercises: AMD Clang compiler

1. We execute the code:

       export LIBOMPTARGET_INFO=$((0x10 | 0x20))
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   Output:

       Libomptarget device 0 info: Copying data from host to device, HstPtr=0x00007ffcadbad870, TgtPtr=0x0000151bf3c20000, Size=819200, Name=y
       Libomptarget device 0 info: Copying data from host to device, HstPtr=0x00007ffcadc75870, TgtPtr=0x0000151bf3ce8000, Size=819200, Name=x
       "PluginInterface" device 0 info: Launching kernel __omp_offloading_73ac72ce_2c01b06c_main_l30 with 400 blocks and 256 threads in SPMD mode
       AMDGPU device 0 info: #Args: 3 Teams x Thrds:  400x 256 (MaxFlatWorkGroupSize: 256) LDS Usage: 9784B #SGPRs/VGPRs: 106/54 #SGPR/VGPR Spills: 38/0 Tripcount: 102400
       Libomptarget device 0 info: Copying data from device to host, TgtPtr=0x0000151bf3ce8000, HstPtr=0x00007ffcadc75870, Size=819200, Name=x
       Libomptarget device 0 info: Copying data from device to host, TgtPtr=0x0000151bf3c20000, HstPtr=0x00007ffcadbad870, Size=819200, Name=y
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   We can read the memory transfers and kernel execution with 400 blocks and 256 threads per block from this output.

   Output for `LIBOMPTARGET_INFO=-1`:

       Libomptarget device 0 info: Entering OpenMP kernel at axpy.c:30:1 with 3 arguments:
       Libomptarget device 0 info: tofrom(y)[819200] (implicit)
       Libomptarget device 0 info: firstprivate(alpha)[8] (implicit)
       Libomptarget device 0 info: tofrom(x)[819200] (implicit)
       Libomptarget device 0 info: Creating new map entry with HstPtrBase=0x00007ffc2f025ac0, HstPtrBegin=0x00007ffc2f025ac0, TgtPtrBegin=0x000014ab7aa20000, Size=819200, DynRefCount=1, HoldRefCount=0, Name=y
       Libomptarget device 0 info: Copying data from host to device, HstPtr=0x00007ffc2f025ac0, TgtPtr=0x000014ab7aa20000, Size=819200, Name=y
       Libomptarget device 0 info: Creating new map entry with HstPtrBase=0x00007ffc2f0edac0, HstPtrBegin=0x00007ffc2f0edac0, TgtPtrBegin=0x000014ab7aae8000, Size=819200, DynRefCount=1, HoldRefCount=0, Name=x
       Libomptarget device 0 info: Copying data from host to device, HstPtr=0x00007ffc2f0edac0, TgtPtr=0x000014ab7aae8000, Size=819200, Name=x
       Libomptarget device 0 info: Mapping exists with HstPtrBegin=0x00007ffc2f025ac0, TgtPtrBegin=0x000014ab7aa20000, Size=819200, DynRefCount=1 (update suppressed), HoldRefCount=0
       Libomptarget device 0 info: Mapping exists with HstPtrBegin=0x00007ffc2f0edac0, TgtPtrBegin=0x000014ab7aae8000, Size=819200, DynRefCount=1 (update suppressed), HoldRefCount=0
       "PluginInterface" device 0 info: Launching kernel __omp_offloading_73ac72ce_2c01b06c_main_l30 with 400 blocks and 256 threads in SPMD mode
       DEVID:  0 SGN:2 ConstWGSize:256  args: 3 teamsXthrds:( 400X 256) reqd:(   0X   0) lds_usage:9784B sgpr_count:106 vgpr_count:54 sgpr_spill_count:38 vgpr_spill_count:0 tripcount:102400 rpc:1 n:__omp_offloading_73ac72ce_2c01b06c_main_l30
       AMDGPU device 0 info: #Args: 3 Teams x Thrds:  400x 256 (MaxFlatWorkGroupSize: 256) LDS Usage: 9784B #SGPRs/VGPRs: 106/54 #SGPR/VGPR Spills: 38/0 Tripcount: 102400
       Libomptarget device 0 info: Mapping exists with HstPtrBegin=0x00007ffc2f0edac0, TgtPtrBegin=0x000014ab7aae8000, Size=819200, DynRefCount=0 (decremented, delayed deletion), HoldRefCount=0
       Libomptarget device 0 info: Copying data from device to host, TgtPtr=0x000014ab7aae8000, HstPtr=0x00007ffc2f0edac0, Size=819200, Name=x
       Libomptarget device 0 info: Mapping exists with HstPtrBegin=0x00007ffc2f025ac0, TgtPtrBegin=0x000014ab7aa20000, Size=819200, DynRefCount=0 (decremented, delayed deletion), HoldRefCount=0
       Libomptarget device 0 info: Copying data from device to host, TgtPtr=0x000014ab7aa20000, HstPtr=0x00007ffc2f025ac0, Size=819200, Name=y
       Libomptarget device 0 info: Removing map entry with HstPtrBegin=0x00007ffc2f0edac0, TgtPtrBegin=0x000014ab7aae8000, Size=819200, Name=x
       Libomptarget device 0 info: Removing map entry with HstPtrBegin=0x00007ffc2f025ac0, TgtPtrBegin=0x000014ab7aa20000, Size=819200, Name=y
       Call         __tgt_rtl_number_of_devices:        0us              4 )
       Call         __tgt_rtl_number_of_devices:        0us              0 )
       Call         __tgt_rtl_number_of_devices:        0us              1 )
       Call            __tgt_rtl_has_apu_device:        0us              0 )
       Call         __tgt_rtl_has_gfx90a_device:        0us              0 )
       Call            __tgt_rtl_has_apu_device:    47240us              0 )
       Call         __tgt_rtl_has_gfx90a_device:    26792us              1 )
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Call             __tgt_rtl_init_requires:        0us              1 (             1)
       Call               __tgt_rtl_init_device:    15899us              0 (             0)
       Call               __tgt_rtl_load_binary:     1916us 0x0000003ed940 (             0, 0x0000003f00b0)
       Call                __tgt_rtl_data_alloc:        3us 0x14ab7aa20000 (             0,         819200, 0x7ffc2f025ac0)
       Call         __tgt_rtl_data_submit_async:      295us              0 (             0, 0x14ab7aa20000, 0x7ffc2f025ac0,         819200, 0x7ffc2f025970)
       Call                __tgt_rtl_data_alloc:        0us 0x14ab7aae8000 (             0,         819200, 0x7ffc2f0edac0)
       Call         __tgt_rtl_data_submit_async:      291us              0 (             0, 0x14ab7aae8000, 0x7ffc2f0edac0,         819200, 0x7ffc2f025970)
       Call             __tgt_rtl_launch_kernel:     3872us              0 (             0, 0x0000003ed9b0, 0x000000437e10, 0x0000004377f0,              3,              0,              0,         102400, 0x7ffc2f025970)
       Call       __tgt_rtl_data_retrieve_async:      280us              0 (             0, 0x7ffc2f0edac0, 0x14ab7aae8000,         819200, 0x7ffc2f025970)
       Call       __tgt_rtl_data_retrieve_async:      311us              0 (             0, 0x7ffc2f025ac0, 0x14ab7aa20000,         819200, 0x7ffc2f025970)
       Call               __tgt_rtl_synchronize:       74us              0 (             0, 0x7ffc2f025970)
       Call               __tgt_rtl_data_delete:        2us              0 (             0, 0x14ab7aae8000)
       Call               __tgt_rtl_data_delete:        0us              0 (             0, 0x14ab7aa20000)
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Output with `LIBOMPTARGET_KERNEL_TRACE=1` (`unset LIBOMPTARGET_INFO`):

       DEVID:  0 SGN:2 ConstWGSize:256  args: 3 teamsXthrds:( 400X 256) reqd:(   0X   0) lds_usage:9784B sgpr_count:106 vgpr_count:54 sgpr_spill_count:38 vgpr_spill_count:0 tripcount:102400 rpc:1 n:__omp_offloading_73ac72ce_2c01b06c_main_l30
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Output with `LIBOMPTARGET_KERNEL_TRACE=2`:

       Call         __tgt_rtl_number_of_devices:        0us              4 )
       Call         __tgt_rtl_number_of_devices:        0us              0 )
       Call         __tgt_rtl_number_of_devices:        0us              1 )
       Call            __tgt_rtl_has_apu_device:        0us              0 )
       Call         __tgt_rtl_has_gfx90a_device:        0us              0 )
       Call            __tgt_rtl_has_apu_device:    27606us              0 )
       Call         __tgt_rtl_has_gfx90a_device:    27074us              1 )
       Call             __tgt_rtl_init_requires:        0us              1 (             1)
       Call               __tgt_rtl_init_device:    17433us              0 (             0)
       Call               __tgt_rtl_load_binary:     2198us 0x0000003ec940 (             0, 0x0000003ef0b0)
       Call                __tgt_rtl_data_alloc:        3us 0x14e55dc20000 (             0,         819200, 0x7ffddba4c170)
       Call         __tgt_rtl_data_submit_async:      309us              0 (             0, 0x14e55dc20000, 0x7ffddba4c170,         819200, 0x7ffddba4c020)
       Call                __tgt_rtl_data_alloc:        1us 0x14e55dce8000 (             0,         819200, 0x7ffddbb14170)
       Call         __tgt_rtl_data_submit_async:      311us              0 (             0, 0x14e55dce8000, 0x7ffddbb14170,         819200, 0x7ffddba4c020)
       Call             __tgt_rtl_launch_kernel:     4157us              0 (             0, 0x0000003ec9b0, 0x000000437dc0, 0x0000004377a0,              3,              0,              0,         102400, 0x7ffddba4c020)
       Call       __tgt_rtl_data_retrieve_async:      297us              0 (             0, 0x7ffddbb14170, 0x14e55dce8000,         819200, 0x7ffddba4c020)
       Call       __tgt_rtl_data_retrieve_async:      341us              0 (             0, 0x7ffddba4c170, 0x14e55dc20000,         819200, 0x7ffddba4c020)
       Call               __tgt_rtl_synchronize:       73us              0 (             0, 0x7ffddba4c020)
       Call               __tgt_rtl_data_delete:        2us              0 (             0, 0x14e55dce8000)
       Call               __tgt_rtl_data_delete:        0us              0 (             0, 0x14e55dc20000)
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   (see [documentation](https://rocm.docs.amd.com/projects/llvm-project/en/latest/conceptual/openmp.html#environment-variables)).

### Bonus exercises: loop construct

1. Cray compiler requires splitting the pragma to two lines. AMD Clang works with one-liner.

