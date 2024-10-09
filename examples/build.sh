#!/bin/sh

set -e

# It is assumed this script lives in the repository root folder called examples
OD="$(pwd)" # original directory
SD="$(dirname "$(realpath "$0")")" # script directory
RD="$SD/.." # root directory
D="$RD/build_examples" # build directory
trap "[ -d $D ] && rm -rf $D; cd $OD" EXIT INT TERM HUP QUIT

export PLATFORMIO_BUILD_CACHE_DIR="$RD/.pio/build_cache"

cd $RD
# The only reason 'pio ci' is called with '--keep-build-dir' is because we need zephyr/prj.conf to be present at build
for example_file in $(find examples -type f -name "*.cpp"); do
    echo "--- TESTING: $example_file"
    mkdir $D
    cp -ar "./zephyr" $D/
    pio ci $example_file --lib src --build-dir $D --keep-build-dir --project-conf platformio.ini "$@"
    rm -rf $D
done
