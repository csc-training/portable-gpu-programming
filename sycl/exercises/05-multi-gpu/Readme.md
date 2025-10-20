# Multi-GPU 
Typically, GPU-based supercomputers comprise between 2 and 8 devices per node. Various strategies can be employed for multi-GPU computing, such as assigning one device per MPI task or enabling a single MPI task to control multiple devices, depending on the application design. In all cases, selecting the right device and ensuring efficient communication between devices are essential for achieving high performance.

## Many GPUs per Process
Each device is associated with its own **queue**, and all operations submitted to a queue are asynchronous. The application can select different devices and submit independent tasks to each queue, enabling overlapping computation and data transfers across multiple GPUs.
For example:
```
//SYCL
std::vector<sycl::device> gpu_devices = sycl::device::get_devices(sycl::info::device_type::gpu);
  size_t num_devices = gpu_devices.size();

  std::cout << "Number of GPUs: " << num_devices << std::endl;

  // Create one queue per GPU
  std::vector<sycl::queue> queues(num_devices);
  for (size_t i = 0; i < num_devices; i++) {
    queues[i] = sycl::queue(gpu_devices[i]);
  }

... 
// Launch kernels (SYCL)
for(unsigned n = 0; n < num_devices; n++) {
  q[n].parallel_for(size[n], [=](id<1> i) { ...});
}
//Synchronize all kernels with host (SYCL)
for(unsigned n = 0; n < num_devices; n++) {
  q[n].wait();
}
```

It is common for data computed on one GPU to be required by another. In this scenario, device-to-device transfers are highly efficient, as they occur within the same node and leverage the high-speed interconnect. When using USM it is alowed to perform memory transfers between different devices using the `memcpy` method. To copy data from a GPU pointer `dA_1` associated with the device 1 to the pointer `dA_0` associated with device 0 one can use:
```
q0.memcpy(dA_0, dA_1, sizeof(int)*N);
```
Becasue SYCL is high-level sits on top of CUDA or HIP the performance will depend on the implementation. If we are lucky the specific very fast features are used and the maximum speed is achieved. 
The [first multi-gpu example]() comapres 
