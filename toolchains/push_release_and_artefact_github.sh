#! /bin/bash

# stops if any command fails
set -e

##
## add "bash -x" for this script to be verbose
##

#
app_version=$(grep -o 'project([^)]*' CMakeLists.txt | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')

# Check if the variable is empty
if [ -z "$app_version" ]; then
  echo "Error: Could not infer app version from Cmake script. Make sure the app version is formated as 'project(appName VERSION XX.XX' in a single line."
  exit 1  # Exit with an error status code
fi

#
echo "Detected app version: v$app_version"

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
  
  #
  toolchain_os_type="msys2"
  descriptive_build_str="windows_$machine_arch"
  cpack_generator="NSIS"
  package_output_ext="exe"

# Check if the operating system is macOS
elif [ "$os_name" == "Darwin" ]; then
  #
  toolchain_os_type="osx"
  descriptive_build_str="macos_$machine_arch"
  cpack_generator="ZIP"
  package_output_ext="zip"

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
generated_artifact_path="$artifacts_directory/$package_name.$package_output_ext"

echo "====> Configuring ... <====="

cmake -GNinja -B$build_directory -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchains/$toolchain_os_type.cmake

echo "====> Building ... <====="

ninja -C $build_directory

echo "====> Shipping Bundle ... <====="

cpack --config ./$build_directory/CPackConfig.cmake -G $cpack_generator -B $artifacts_directory -D CPACK_PACKAGE_FILE_NAME=$package_name

# on Windows target, sign installer !
if [[ $os_name = "MINGW64_NT"* ]]; then
  if ! command -v signtool &> /dev/null
  then
      echo "signtool must be in PATH, please refer to README.md. Please add [Ordinateur\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows Kits\Installed Roots:KitsRoot10]/App Certification Kit to PATH."
      exit 1
  fi

  #
  echo "====> Signing Bundle ... <====="

  #
  timestamp_server="http://timestamp.acs.microsoft.com"
  expected_pfx_path="./certs/all.pfx"
  signtool sign -f "$expected_pfx_path" -tr "$timestamp_server" -td SHA256 -fd SHA256 "$generated_artifact_path"
fi

##
##
##

if ! gh release view v$app_version; then
  echo "Creating release + upload on v$app_version..."
  gh release create v$app_version --generate-notes $generated_artifact_path
else
  echo "Uploading release ONLY on v$app_version (may override existing artifact with same name)..."
  gh release upload --clobber v$app_version $generated_artifact_path
fi
