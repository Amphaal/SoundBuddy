SET(CMAKE_OSX_DEPLOYMENT_TARGET 13.2)

list(APPEND CMAKE_MODULE_PATH
    "/opt/homebrew/opt/llvm/lib/cmake"
)

# mandatory, else OpenSSL wont be found
SET (OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)

list(APPEND CMAKE_PREFIX_PATH 
    "/opt/homebrew/opt/openssl@3"
    "/opt/homebrew/opt/llvm"
)

# Will probably find AppleClang of XCode first
SET (CMAKE_CXX_COMPILER         "clang++")

# specifics of LLVM's "normal" toolkit
# make sure to add "/opt/homebrew/opt/llvm/bin" to .zshrc PATH
SET (CMAKE_AR                   "llvm-ar")
SET (CMAKE_RANLIB               "llvm-ranlib")
SET (CMAKE_NM                   "llvm-nm")
SET (CMAKE_RC_COMPILER          "llvm-rc")

# we have to manually tell to use SDK libs / framework, because cmake's automatic --isysroot of SDK does not the job using "classic" LLVM
SET (OSX_SDK_PATH               "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")
SET (CMAKE_EXE_LINKER_FLAGS     "-fuse-ld=lld")

SET (CMAKE_SHARED_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS})

SET (CMAKE_CXX_FLAGS            "-isysroot ${OSX_SDK_PATH}")
SET (CMAKE_CXX_FLAGS_DEBUG      "-O0 -g")