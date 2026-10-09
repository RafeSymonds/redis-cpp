#!/bin/sh
# Builds (unless bin/course already did) and runs your program.
# The tester spawns this script for every test case.
# CC_SKIP_BUILD / CC_BUILD_ONLY are set by bin/course.
set -e
if [ -z "$CC_SKIP_BUILD" ]; then
  (
    cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
    cmake -B build -S . ${VCPKG_ROOT:+-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake}
    cmake --build ./build
  )
fi
[ -z "$CC_BUILD_ONLY" ] || exit 0
exec $(dirname "$0")/build/redis "$@"
