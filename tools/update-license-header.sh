#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)
#
# SPDX-License-Identifier: LGPL-3.0-or-later

set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

if ! command -v reuse >/dev/null 2>&1; then
    printf '%s\n' "error: REUSE CLI is required (install it with: pip install reuse)" >&2
    exit 127
fi

mapfile -d '' tool_scripts < <(
    find tools -type f -name '*.sh' ! -path "$repo_root/tools/update-license-header.sh" -print0
)

reuse annotate \
    --copyright "Michael Wipplinger (micaah42)" \
    --license LGPL-3.0-or-later \
    --year 2026 \
    --recursive \
    src QPerformanceQml tests \
    "${tool_scripts[@]}" \
    CMakeLists.txt libdatapoints.py README.md

reuse lint
