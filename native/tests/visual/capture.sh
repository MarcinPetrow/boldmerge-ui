#!/bin/sh

set -eu

if [ "$#" -ne 2 ]; then
  echo "usage: capture.sh CAPTURE_EXECUTABLE OUTPUT_DIRECTORY" >&2
  exit 2
fi

capture_executable=$1
output_directory=$2
script_directory=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
lock_file="$script_directory/environment.lock"

if [ ! -r /etc/os-release ]; then
  echo "capture profile requires /etc/os-release" >&2
  exit 1
fi

. /etc/os-release
if [ "${ID:-}" != "ubuntu" ] || [ "${VERSION_ID:-}" != "26.04" ]; then
  echo "capture profile requires Ubuntu 26.04; found ${ID:-unknown} ${VERSION_ID:-unknown}" >&2
  exit 1
fi

while IFS='=' read -r package expected; do
  case "$package" in
    ''|'#'*) continue ;;
  esac
  actual=$(dpkg-query -W -f='${Version}' "$package" 2>/dev/null || true)
  if [ "$actual" != "$expected" ]; then
    echo "capture profile mismatch: $package expected $expected, found ${actual:-missing}" >&2
    exit 1
  fi
done < "$lock_file"

mkdir -p "$output_directory"
rm -f "$output_directory"/*.png "$output_directory/SHA256SUMS"

export GDK_BACKEND=x11
export GSK_RENDERER=cairo
export GTK_A11Y=none
export GTK_USE_PORTAL=0
export GIO_USE_VFS=local
export GSETTINGS_BACKEND=memory
export LANG=C.UTF-8
export LC_ALL=C.UTF-8
export LIBGL_ALWAYS_SOFTWARE=1
export NO_AT_BRIDGE=1
export XDG_CONFIG_HOME="$output_directory/config"
export XDG_CACHE_HOME="$output_directory/cache"

dbus-run-session -- xvfb-run -a \
  -s '-screen 0 1280x1024x24 -dpi 96 -nolisten tcp' \
  "$capture_executable" "$output_directory"

(
  cd "$output_directory"
  sha256sum ./*.png > SHA256SUMS
)

echo "Native visual captures written to $output_directory"
