<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Working in supercomputers
event: Portable GPU Programming
lang:   en
---

# Outline

- Introduction to Roihu and LUMI
- Connecting to Roihu and LUMI supercomputers
- File system in Roihu and LUMI supercomputers
- Module system
- Building applications
- Running applications on a supercomputer

# Roihu supercomputer

- Current national flagship system
- Two parts, Roihu-CPU and Roihu-GPU
- Roihu-GPU
    - 132 nodes with 4 NVIDIA GH200 Grace Hopper superchips per node
    - Each superchip comprises one H100 GPU and Grace CPU with 72 ARM cores
    - 120 GiB of CPU memory and 96 GiB of GPU memory per superchip
- Roihu-CPU
    - 486 nodes with AMD Turin CPUs
    - 384 x86 cores and 768 or 1536 GiB of memory per node

# LUMI supercomputer

<div class=column style=width:60%>
- LUMI is pan-European supercomputer hosted by CSC
    - user support is distributed to LUMI consortium countries
- For Finnish users access is via CSC
- Over 10 000 AMD MI250X GPUs
- "small" (262 144 cores) CPU partition
- auxiliary partition for data-analytics with lots of memory (32 TB per node)
</div>
<div class=column style=width:38%>
![](img/lumi2.jpg){.center width=90%}
</div>

# Connecting to Roihu and LUMI supercomputers {.section}

# Anatomy of a supercomputer

![](img/cluster_diagram.svg){.center width=100%}

# Connecting to Roihu and LUMI supercomputers

- SSH is used to connect to the login node
  - <https://github.com/csc-training/portable-gpu-programming/wiki/Setting-up-CSC-account-and-SSH>
- Web interfaces also exist
  - <https://www.roihu.csc.fi>
  - <https://www.lumi.csc.fi>

# Filesystem in LUMI and CSC supercomputers {.section}

# Directory structure

- Roihu and LUMI supercomputers have separate file systems
  - Files need to be explicitly copied between Roihu and LUMI
- Directory structure is common in all systems

|            |Owner   |Environment variable|Path                 |
|------------|--------|--------------------|---------------------|
|**home**    |Personal|`$HOME`             |`/users/<user-name>` |
|**projappl**|Project |Not available       |`/projappl/<project>`|
|**scratch** |Project |Not available       |`/scratch/<project>` |

- See ``csc-workspaces` on Roihu or lumi-workspaces` on LUMI

# Using project-level storage space

- Common practice: create your personal directory under scratch:
  ```bash
  mkdir -p /scratch/<project>/$USER
  cd /scratch/<project>/$USER
  ```
- Use this personal work space to avoid file conflicts with other project members

# Filesystems on CSC supercomputers

- The filesystem used on CSC systems is called **Lustre**
  - Parallel: data is distributed across many storage drives
  - Shared: Files can be accessed from all nodes
  - Lustre is very common in HPC in general, not just at CSC
- Many systems also provide node-local disk area for temporary storage
  - `/tmp`, `$TMPDIR`, `$LOCAL_SCRATCH` *etc.* depending on the system
  - Sometimes the temporary storage may reside directly in memory (`/tmp` on LUMI compute nodes)
  - See system docs for details

# Lustre architecture

<div class="column">

- Files are chunked up and spread across multiple **storage servers** as **objects**
- Dedicated **metadata server(s)** (MDS): file names, owners, permissions, ...
- **Client**: HPC node that access the data

</div>

<div class="column">

![](img/lustre-architecture.svg)
Clients interact with MDS once to gain OST access, then I/O to objects directly

- Allows for **very high, parallel I/O bandwidth!**

</div>

# Lustre metadata servers

- Every file lookup, file creation/deletion, permission change *etc.* is processed by the metadata servers
- Metadata servers are shared by everyone using the supercomputer!
- Commands like `ls` unresponsive? Servers may be under heavy load

# Being nice to Lustre (and other users)

- Avoid accessing a large number of small files
  - Practical example: Python environments are typically containerized to avoid a significant performance hit due to accessing thousands of small files when loading the enviroment
- Avoid `ls -l` and use plain `ls` instead if you don't need the extra metadata
  - Less stress on the metadata servers
- Use Lustre tools (e.g., `lfs find`) instead of regular file system tools (e.g. `find`)
  - Less stress on the metadata servers

# Editing files

- Edit directly on the supercomputer
  - Vim, emacs, nano, ...
- Edit locally with any editor and sync to the supercomputer
- VSCode, Jupyter, etc apps in the web interface
  - <https://www.roihu.csc.fi>
  - <https://www.lumi.csc.fi>


# Module system {.section}

# Module environment

- Supercomputers have a large number of users with different needs for
  development environments and applications
- _Environment modules_ provide a convenient way to dynamically change the
  user's environment
- Different compiler suites and application versions can be used smoothly with different modules
  - Changing the compiler module automatically loads also the correct versions of other dependent libraries
  - Loading a module for an application sets up the correct environment with a single command


# Common module commands

<div class="column">
`module load mod`
  : Load module **mod** in shell environment

`module unload mod`
  : Remove module **mod** from environment

`module list`
  : List loaded modules

`module avail`
  : List all available modules
</div>

<div class="column">
`module spider mod`
  : Search for module **mod**

`module show mod`
  : Get information about module **mod**

`module switch mod1 mod2`
  : Switch loaded **mod1** to **mod2**

`module reset`
  : Reset the environment to the default module set
</div>


# Building applications {.section}

# Compiling and linking

<div class=column>
- A compiler turns a source code file into an object file that contains
  machine code that can be executed by the processor
- A linker combines several compiled object files into a single executable file
- Together, compiling and linking is called building
</div>
<div class=column>
![](img/building.svg){.center}
</div>

# Compiling and linking

Single file source code:

```bash
cc main.c -o main
```

- In practice programs are separated into several files
  <br>&rarr; tree-like dependency structures
- Build tools allow one to automate compilation and linking and
  rebuild only the parts that changed
    - Make, Ninja


# Build generators

- In a large software projects, figuring out all the dependencies between
  software modules can be difficult
- In order to improve portability and make dependency handling easier, build generators
  are often used
    - Select automatically correct compilers and compiler options
    - Discover dependencies between source files
- **GNU Autotools** and **cmake** are the most common build generators in HPC


# Running applications on a supercomputer {.section}

# Batch job system

- On a supercomputer, instead of running a program instantly, you submit your calculation job to
  a queue and the system will then execute it once the resources are available
  - The queue enables effective and fair resource usage
  - CSC uses Slurm as the queue system

# Available resources: Slurm partitions

- Compute nodes are grouped in different *partitions* for different use cases
  - Small CPU jobs, large CPU jobs, small GPU jobs, large GPU jobs, debugging, ...
- List all partitions:
  ```bash
  sinfo
  ```
- Useful practical formatting:
  ```bash
  sinfo -e -o "%16P %4a %8s %.11l %11A %6z %.9m %30G %40N"
  ```
- See also system documentation:
  - <https://docs.lumi-supercomputer.eu/runjobs/scheduled-jobs/partitions/>
  - <https://docs.csc.fi/computing/running/batch-job-partitions/>

# Slurm batch jobs

- A job defines the following:
  - The resource allocation requests
  - The script to run the calculation
    - Note! A special `srun` launcher is usually needed to launch the calculation
- Structure of a batch job script `job.sh`:
  ```bash
  #!/bin/bash
  #SBATCH <resource allocation requests>

  <script to run the calculation>
  ```


# Submitting batch jobs

- Submit the batch job to the queue:
  ```bash
  sbatch job.sh
  ```
- Useful hint: You can override the parameters in the job script from the command line:
  ```bash
  sbatch --nodes=1 --ntasks-per-node=4 --partition=debug job.sh
  ```

# Managing batch jobs

- Follow the status of your jobs:
  ```bash
  squeue --me
  ```
- Cancel jobs using the numeric ID of the job (`<jobid>`):
  ```bash
  scancel <jobid>
  ```
- Show job resource usage (for completed jobs):
  ```bash
  sacct <jobid>
  ```
- Useful for debugging: Launch an interactive shell on a running allocation:
  ```bash
  srun --overlap --jobid=<jobid> --pty bash
  ```


# Useful environment variables

- Following variables are available inside Slurm scripts:
  - `SLURM_JOBID`: job's id
  - `SLURM_JOB_NAME`: job's name (given in `job-name`)
  - `SLURM_JOB_NODELIST`: list of nodes allocated for the job
- Following variables are available inside program launched by `srun`:
  - `SLURM_NTASKS`: the number of tasks
  - `SLURM_PROCID`: the global id of the calling process
  - `SLURM_LOCALID`: the node-local id of the calling process
- See `man sbatch` for a complete list


# Summary {.section}

# Summary

- Login nodes are entry points to a supercomputer
- Modules and build tools help managing environment and software
- Calculations are submitted as jobs to the queueing system

# Web resources

- CSC User guide: <https://docs.csc.fi/>
- LUMI User guide: <https://docs.lumi-supercomputer.eu/>
- General Slurm documentation: <https://slurm.schedmd.com/>
