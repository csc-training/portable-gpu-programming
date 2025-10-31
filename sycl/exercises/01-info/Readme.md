<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Enumerate devices
When running a multi-gpu program it is very important that each process or  cpu thread picks the right devices. SYCL provides several function to check the visible devices. For example the `get_devices` which is part of the `sycl::devices` namespace can obtain the list of devices which follow a specific criteria. With the argument `sycl::info::device_type::gpu` it will list all device which have the `device_type` property `gpu`. Similarly one can get the available CPU using the keyword `cpu`as argument. 
A complete code is shown in the  [enumerate_device.cpp](enumerate_device.cpp).

