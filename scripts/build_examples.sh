#!/usr/bin/env bash
# build_examples.sh
# Build every example/lab under examples/ and labs/ that has a Makefile.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

for dir in "$ROOT_DIR"/labs/*/ "$ROOT_DIR"/examples/*/; do
    if [ -f "$dir/Makefile" ]; then
        echo "==> Building $dir"
        (cd "$dir" && make)
    fi
done
