#!/usr/bin/env bash
# setup_env.sh
# TODO: source the QNX SDP environment (qnxsdp-env.sh) and verify qcc is on PATH.

set -euo pipefail

echo "TODO: source QNX SDP env script, e.g.:"
echo "  source ~/qnx710/qnxsdp-env.sh"
command -v qcc >/dev/null 2>&1 && echo "qcc found: $(command -v qcc)" || echo "qcc NOT found on PATH"
