#! /bin/bash

##
##
##

#
app_version=$(grep -o 'project([^)]*' CMakeLists.txt | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')

# Check if the variable is empty
if [ -z "$app_version" ]; then
  echo "Error: Could not infer app version from Cmake script. Make sure the app version is formated as 'project(appName VERSION XX.XX' in a single line."
  exit 1  # Exit with an error status code
fi

#
echo "Detected app version: $app_version"

##
##
##

# Define the name of the executable you want to check for
github_cli="gh"

# Check if the executable is found in the PATH
if ! command -v "$github_cli" &> /dev/null; then
  echo "Error: '$executable_name' (Github's CLI) not found in PATH. It is required to communicate releases and artifacts automatically."
  exit 1  # Exit with an error status code
fi

# Check if you are logged in to GitHub CLI
if ! gh auth status &> /dev/null; then
  echo "Error: Not logged in to GitHub CLI. Please log in."
  exit 1  # Exit with an error status code
fi

##
##
##

# Get the URL of the origin/master remote repository
remote_url=$(git remote get-url origin)

# Print the URL
echo "URL of the origin/master remote repository: $remote_url"

# Check if there are releases using gh release list
if ! gh release list --exclude-drafts --exclude-pre-releases -L 1 | grep -q ""; then
  # If the message "no releases found" is found, print a message and exit
  echo "No releases found."
else
  # If there are releases, get the latest release
  latest_release=$(gh release list --exclude-drafts --exclude-pre-releases -L 1 | awk '{print $3}')
  echo "Latest release found on repo: $latest_release"
fi

##
##
##

# Get the operating system name
os_name=$(uname -s)
machine_arch=$(uname -m)

# Check if the operating system is Windows (MINGW)
if [[ $os_name = "MINGW64_NT"* ]]; then
  toolchain_os_type="msys2"
  descriptive_build_str="windows_$machine_arch"
# Check if the operating system is macOS
elif [ "$os_name" == "Darwin" ]; then
  toolchain_os_type="osx"
  descriptive_build_str="windows_$machine_arch"
# Exit with an error message for any other operating system
else
  echo "Unsupported operating system: $os_name"
  exit 1
fi

##
##
##

build_directory="build_release"
artifacts_directory="./$build_directory/artifacts"
package_name="$(basename `git rev-parse --show-toplevel`)-v$app_version-$descriptive_build_str"
generated_artifact_path="$artifacts_directory/$package_name.zip"

echo "====> Configuring ... <====="

cmake -GNinja -B$build_directory -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchains/$os_type.cmake

echo "====> Building ... <====="

ninja -C $build_directory

echo "====> Shipping Bundle ... <====="

cpack --config ./$build_directory/CPackConfig.cmake -G ZIP -B $artifacts_directory -D CPACK_PACKAGE_FILE_NAME=$package_name

##
##
##

if ! gh release view v$app_version; then
  echo "Creating release + upload on v$app_version..."
  gh release create v$app_version --generate-notes $generated_artifact_path
else
  echo "Uploading release ONLY on v$app_version (may override existing artifact with same name)..."
  gh release upload --clobber $app_version $generated_artifact_path
fi
