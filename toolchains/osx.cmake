SET(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)

list(APPEND CMAKE_PREFIX_PATH 
    "/usr/local/opt/openssl@3"
)

SET (CMAKE_CXX_COMPILER         "clang++")

SET (CMAKE_AR                   "llvm-ar")
SET (CMAKE_RANLIB               "llvm-ranlib")
SET (CMAKE_NM                   "llvm-nm")
SET (CMAKE_RC_COMPILER          "llvm-rc")

SET (CMAKE_CXX_FLAGS            "")
SET (CMAKE_CXX_FLAGS_DEBUG      "-O0 -g")