-- Jupyter
local project = "project_462001074"
prepend_path("PATH", pathJoin("/projappl", project, "plotting_env", "bin"))

-- lab / notebook / empty (defaults to jupyter)
setenv("_COURSE_NOTEBOOK_TYPE", "lab")
