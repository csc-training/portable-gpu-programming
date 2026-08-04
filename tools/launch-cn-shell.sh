#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# Unset all SLURM_* env variables so that sbatching works expectedly
for var in $(env | cut -d= -f1 | grep -E '^SLURMD?_'); do
    if [[ "$var" != "SLURM_ACCOUNT" ]]; then
        unset "$var"
    fi
done

# Create workspace in /tmp
mkdir -p /tmp/$USER
# cd /tmp/$USER

exec /bin/bash
