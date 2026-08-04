#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

set -euo pipefail

if [[ -z "${1:-}" ]]; then
  echo "Error: no input file provided"
  echo "Usage: $0 <html-file>"
  exit 1
fi

INPUT_FILE="$1"

if [[ ! -f "$INPUT_FILE" ]]; then
  echo "Error: file '$INPUT_FILE' not found"
  exit 1
fi

perl -0777 -i -pe '
s|<script>.*?</script>|
<script>
  document.addEventListener("DOMContentLoaded", () => {
    // Check ?open=... param
    const url = new URL(location.href);
    const val = url.searchParams.get("open");

    // Initialize accordions
    const accordions = document.querySelectorAll("c-accordion");
    accordions.forEach((accordion) => {
      accordion.value = val ? [val] : [];
      accordion.multiple = true;
    });

    // Clean ?open=... from url
    url.searchParams.delete("open");
    history.replaceState(null, "", url.toString());
  });
</script>|s
' "$INPUT_FILE"

echo "Patched: $INPUT_FILE"
