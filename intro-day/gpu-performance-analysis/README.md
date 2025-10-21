# GPU performance analysis

In this exercise you can try visualizing GPU execution by
tracing a simple application, "poisson" that you compiler in the
exercise [Using modules](using-modules).

## rocprof in LUMI

1. Run the application with rocprof:
```
#SBATCH ...
...

srun rocprof --hip-trace ./poisson
```

2. Copy the output `result.json` to your local workstation:
   - Find out the filename with absolute path
     ```
     realpath results.json
     ```
   - "rsync" the file by copy-pasting the above output into your local terminal
     ```
     rsync <username>@lumi.csc.fi:<filename_with_absolut_path> .
     ```
3. Go with a web browser to https://ui.perfetto.dev and from the "Open trace" open
   the above file

## Omnitrace in LUMI

1. Load the proper modules and run the application with omnitrace
```
#SBATCH ...
...

module use /scratch/project_462001074/modulefiles
module load omnitrace
srun omnitrace-sample -H false -- ./poisson
```

By default, Omnitrace outputs goes to directory `omnitrace-myexe-output/time-stamp/`, where the file `perfetto-trace-xxxxx.proto`
contains the actual trace. This file can be opened with Perfetto on local browser similar to above.

## Nsight Systems in Mahti

Nsight Systems is available within the module set used in the [Using modules](using-modules).
Run the application with
```
#SBATCH ...
...

srun nsys profile ./poisson
```

Output is in the file `report.nsys-rep` which can be opened with the `nsys-ui` GUI.
For smoother operation we recommend to install Nsight Systems on your local workstation and copy the `.nsys-rep` file there analysis. 
