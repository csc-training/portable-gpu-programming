# Setting up Jupyter for courses on LUMI

Create the environment where notebooks are run (see [LUMI documentation](https://docs.lumi-supercomputer.eu/software/installing/container-wrapper/) for more information):

```bash
module load LUMI/24.03
module load lumi-container-wrapper

conda-containerize new plotting-env.yml --prefix /projappl/project_462001074/plotting_env
```

Create the course module (see [LUMI documentation](https://docs.lumi-supercomputer.eu/runjobs/webui/jupyter-for-courses/) for more information):

```bash
mkdir -p /projappl/project_462001074/www_lumi_modules
cp plotting.lua plotting-resources.yml /projappl/project_462001074/www_lumi_modules/
```
