# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Paths to components that will be built in the test_app.
# If the components have unit tests (in its test subdirectory) it will be run too.
set(EXTRA_COMPONENT_DIRS ../components
    $ENV{IDF_PATH}/tools/unit-test-app/components)

set(TEST_COMPONENTS "BME280" "pcf8574" "PMS7003" "FileManager" CACHE STRING "List of components to test")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


project(AirSENSE_test_app)
