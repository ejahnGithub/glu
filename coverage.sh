#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROFRAW_UNIT="${SCRIPT_DIR}/unit_tests.profraw"
PROFRAW_FUNC="${SCRIPT_DIR}/functional_tests.profraw"
PROFDATA_FILE="${SCRIPT_DIR}/default.profdata"
COVERAGE_DIR="${SCRIPT_DIR}/coverage_html"

BUILD_DIR="build"

UNIT_TESTS_EXEC="./${BUILD_DIR}/test/unit_tests"

FUNCTIONAL_TESTS_EXEC="ftests.sh"
FUNCTIONAL_TESTS_DIR="test/functional"

function error_exit {
    echo "$1" 1>&2
    exit 1
}

echo "Cleaning old coverage data..."
rm -f ${PROFDATA_FILE}

if [ ! -d "${BUILD_DIR}" ]; then
    echo "Build directory does not exist. Creating fresh build directory..."
else
    echo "Reconfiguring build with assertions, asan, and coverage enabled..."
fi
cmake -Bbuild -DLLVM_ENABLE_ASSERTIONS=1 -DENABLE_ASAN=ON -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug || error_exit "Failed to configure project."

echo "Building the project..."
cmake --build ${BUILD_DIR} -j$(nproc) || error_exit "Failed to build the project."

if [ ! -f "${UNIT_TESTS_EXEC}" ]; then
    error_exit "Unit tests executable '${UNIT_TESTS_EXEC}' does not exist. Please ensure the build was successful."
fi

echo "Running unit tests with coverage instrumentation..."
LLVM_PROFILE_FILE="${PROFRAW_UNIT}" ${UNIT_TESTS_EXEC} || error_exit "Unit tests execution failed."

echo -e "\nRunning functional tests with coverage instrumentation..."
(cd ${FUNCTIONAL_TESTS_DIR} && LLVM_PROFILE_FILE="${PROFRAW_FUNC}.%p" ./${FUNCTIONAL_TESTS_EXEC}) || error_exit "Functional tests execution failed."

echo -e "\nRunning glu-demangle tests..."
(cd tools/glu-demangle && cargo test) || error_exit "glu-demangle tests execution failed."

echo "Merging profile data..."
# Collect all profraw files from tests only (not from build process)
PROFRAW_FILES=""
[ -f "${PROFRAW_UNIT}" ] && PROFRAW_FILES="${PROFRAW_UNIT}"
for file in ${PROFRAW_FUNC}.*; do
    [ -f "$file" ] && PROFRAW_FILES="${PROFRAW_FILES} $file"
done

if [ -z "${PROFRAW_FILES}" ]; then
    error_exit "No profile data files found!"
fi

echo "Merging files..."
llvm-profdata merge -sparse ${PROFRAW_FILES} -o ${PROFDATA_FILE} || error_exit "Failed to merge profile data."

echo "Generating coverage report..."
# Collect all shared libraries for coverage
OBJECT_FILES=""
for lib in ${BUILD_DIR}/lib/*/*.so; do
    [ -f "$lib" ] && OBJECT_FILES="${OBJECT_FILES} -object $lib"
done

llvm-cov show ${UNIT_TESTS_EXEC} -object ${BUILD_DIR}/tools/gluc/gluc ${OBJECT_FILES} -instr-profile=${PROFDATA_FILE} -format=html -output-dir=${COVERAGE_DIR} -ignore-filename-regex="(build/_deps|test)/" 2>&1

echo "Cleaning up coverage raw data..."
find . -name "*.profraw*" -type f -delete

echo "Coverage report generation completed successfully."
