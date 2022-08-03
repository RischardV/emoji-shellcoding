#!/usr/bin/env bash

#
# (c) 2022-2022 Hadrien Barral
# SPDX-License-Identifier: CC0-1.0
#
# A small script to check python and shell scripts
#

cd "$(dirname "$0")" || exit

CHECK_ONLY=0
[[ $# -gt 0 ]] && CHECK_ONLY=1

STATUS=0

find . -name "*.sh" -exec shellcheck {} \; || STATUS=1

# $@: python files to check
check_python() {
    black_args=(-l 120 --target-version py310 --preview)
    [[ "${CHECK_ONLY}" -eq 0 ]] && black_args+=(-q)
    [[ "${CHECK_ONLY}" -eq 1 ]] && black_args+=(--check --diff)
    black "${black_args[@]}" "$@" || STATUS=1

    isort_args=()
    [[ "${CHECK_ONLY}" -eq 1 ]] && isort_args+=(-c)
    isort "${isort_args[@]}" "$@" || STATUS=1

    pylint -j 0 "$@" --score=n || STATUS=1

    mypy --disallow-untyped-defs --color-output "$@" || STATUS=1

}

readarray -d '' python_files < \
    <(find . -name "*.py" -type f -print0)
check_python "${python_files[@]}"

exit "${STATUS}"
