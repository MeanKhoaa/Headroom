#!/usr/bin/env bash
set -euo pipefail

python3 -m pip install --user uv
export PATH="$HOME/.local/bin:$PATH"

uv tool install pebble-tool --python 3.13 || uv tool upgrade pebble-tool
pebble sdk install latest
