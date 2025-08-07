cmake_minimum_required(VERSION 3.23)
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(PreProjectInit)

calculate_xray_build_id(XRAY_BUILD_ID)
project(OpenXRay
    DESCRIPTION "OpenXRay is an improved version of the X-Ray Engine, \
                the game engine used in the world-famous S.T.A.L.K.E.R. game series by GSC Game World."
    VERSION 1.6.02.${XRAY_BUILD_ID}
    HOMEPAGE_URL "https://github.com/OpenXRay/xray-16"
    LANGUAGES CXX C
)
message(STATUS "CMAKE_PROJECT_VERSION: ${CMAKE_PROJECT_VERSION}")

include(PostProjectInit)

add_subdirectory(Externals)
add_subdirectory(src)
add_subdirectory(res)
add_subdirectory(misc)
