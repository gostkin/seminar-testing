#!/usr/bin/env sh

root=$(cd "$(dirname "$0")"; pwd)

if ! [ -x "$(command -v git)" ]; then
  echo 'Error: git is not installed.' >&2
  exit 1
fi

if ! [ -f "${root}/.gitmodules" ]; then
    echo ".gitmodules file is not found, submodules are potentially missed." >&2
fi

git submodule update --init --recursive

paths=$(cat ${root}/.gitmodules | grep -Po "(?<=\tpath = )[\w /]+(?=$)")

for filename in ${paths}; do
    if ! [ -n "$(ls -A ${root}/${filename})" ]; then
        echo "Error: submodule ${filename} is not fetched" >&2
        exit 1
    fi
done

echo "-- Submodule initialization done"
exit 0
