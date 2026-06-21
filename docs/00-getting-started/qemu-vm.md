# QNX SDP 8.0 - Running a QEMU Virtual Machine from Terminal

> Status: 🟡 In progress — core workflow verified working, a few sections below are marked `_TODO_` pending exact details from the original debugging notes.

Author: Rakuram E.

---

## Objective

This document describes how to create, boot, manage, and troubleshoot a QNX
SDP 8.0 virtual machine using **QEMU** from the Linux command line, without
relying on the Momentics IDE.

---

## Environment

| Component | Value |
|---|---|
| Host OS | Ubuntu 25.xx |
| QNX SDP | 8.0 |
| Architecture | x86_64 |
| Hypervisor | QEMU |
| SSH Client | OpenSSH |
| Project Directory | `~/qnx_vm_clean` |

SSH key:

```text
~/.ssh/id_ed25519
~/.ssh/id_ed25519.pub
```

---

## Prerequisites

- QNX SDP 8.0 installed at `~/qnx800`
- An SSH key pair generated (`~/.ssh/id_ed25519`)
- The project directory created: `~/qnx_vm_clean`
- `mkqnximage` available at `~/qnx800/host/common/bin/mkqnximage` (part of the SDP host tools — no separate QEMU install needed)

> _TODO: confirm exact `mkqnximage` invocation/flags used to **create** the VM image for the first time (see "Create VM" below) — not yet captured in these notes._

---

## Daily Workflow

This is the three-terminal routine used for day-to-day development.

### Terminal 1 — Start the Virtual Machine

```bash
cd ~/qnx_vm_clean
source ~/qnx800/qnxsdp-env.sh

/home/rakuram/qnx800/host/common/bin/mkqnximage --run
```

Leave this terminal running — it owns the VM process.

### Terminal 2 — Interact with the VM

```bash
cd ~/qnx_vm_clean
source ~/qnx800/qnxsdp-env.sh

/home/rakuram/qnx800/host/common/bin/mkqnximage --getip
```

Example output:

```text
172.31.1.208
```

SSH into the VM:

```bash
ssh root@172.31.1.208
```

### Terminal 3 — Build Applications

```bash
source ~/qnx800/qnxsdp-env.sh

cd ~/workspace/proc_thread
make
```

Copy the resulting binary to the VM and execute it over SSH (e.g. `scp` the
binary, then run it via the Terminal 2 SSH session).

---

## QNX Environment

Every **newly opened Ubuntu terminal** starts with a plain Linux environment.
The QNX SDP environment is **not persistent** across terminals and **must**
be loaded before using any QNX tool.

```bash
source ~/qnx800/qnxsdp-env.sh
```

This exports the variables QNX tooling depends on:

```text
QNX_HOST=/home/rakuram/qnx800/host/linux/x86_64
QNX_TARGET=/home/rakuram/qnx800/target/qnx
MAKEFLAGS=-I/home/rakuram/qnx800/target/qnx/usr/include
```

Verify it loaded correctly:

```bash
echo $QNX_HOST
echo $QNX_TARGET
```

Before sourcing, both will print empty. After sourcing, you should see the
paths above.

### Why `mkqnximage: command not found` happens

`mkqnximage` (and `qcc`, `mkifs`, `dumpifs`, `ntox86_64-gdb`, etc.) only
resolve as bare commands if `$QNX_HOST/.../bin` is already on your `PATH`.
Until the environment is sourced, none of these tools exist in the shell —
so either:

- source the environment first, or
- invoke the tool by its full path:

  ```bash
  /home/rakuram/qnx800/host/common/bin/mkqnximage --getip
  ```

> **Rule of thumb:** every new terminal session needs the QNX SDP environment
> loaded before using QNX tools. If you see `command not found` or
> `QNX_TARGET not defined`, the first thing to check is whether
> `qnxsdp-env.sh` has been sourced in *that* terminal.

### Shortcut: alias it

Instead of typing the full `source` command in every terminal, add this to
`~/.bashrc`:

```bash
alias qnxenv='source ~/qnx800/qnxsdp-env.sh'
```

Reload your shell:

```bash
source ~/.bashrc
```

Now every new terminal only needs:

```bash
qnxenv
```

before using QNX tools.

---

## Create VM

> _TODO: this section needs the exact command(s) used to generate the VM
> image for the first time (e.g. an initial `mkqnximage` invocation with
> build-specific flags, run from `~/qnx_vm_clean`). Capture it here once
> confirmed — the rest of this guide assumes the image already exists._

---

## Run VM

From the project root:

```bash
cd ~/qnx_vm_clean
source ~/qnx800/qnxsdp-env.sh

/home/rakuram/qnx800/host/common/bin/mkqnximage --run
```

Leave this terminal open for as long as the VM should stay running.

---

## Obtain VM IP

The VM must already be running (see **Run VM** above).

```bash
cd ~/qnx_vm_clean
source ~/qnx800/qnxsdp-env.sh

/home/rakuram/qnx800/host/common/bin/mkqnximage --getip
```

Example output:

```text
172.31.1.208
```

---

## SSH Access

```bash
ssh root@<vm-ip>
```

Example:

```bash
ssh root@172.31.1.208
```

Uses the key pair at `~/.ssh/id_ed25519` (see **Environment** above).

---

## Directory Structure

> _TODO: full layout not yet captured. Known so far, from the project root
> `~/qnx_vm_clean`:_

```text
qnx_vm_clean/
├── disk-qemu        # raw disk image (see Generated Files)
├── qnx_sdp.ifs       # boot image (IFS)
├── startup.sh         # target-side startup script
└── output/             # TODO: confirm contents — do NOT run mkqnximage from here (see Troubleshooting, Issue 6)
```

---

## Understanding `mkqnximage`

`mkqnximage` wraps the multi-step process of assembling and booting a QNX VM
image so you don't have to invoke QEMU manually with the correct boot
arguments, disk image, and IFS every time. It must be run **from the project
root** (`~/qnx_vm_clean`), with the QNX SDP environment already sourced.

Common flags used in this workflow:

| Flag | Purpose |
|---|---|
| `--run` | Boot the VM |
| `--getip` | Print the running VM's IP address |

> _TODO: document the build/create flag(s) once confirmed (see **Create VM**)._

---

## Generated Files

| File | What it is |
|---|---|
| `disk-qemu` | A raw disk image — **not** an executable. Boot it via `mkqnximage --run`, never run it directly. |
| `qnx_sdp.ifs` | The boot image (IFS) loaded by QEMU at startup. |
| `startup.sh` | A **target-side** script packaged into the image — it runs inside the booted QNX VM, not on the Ubuntu host. Do not try to execute it from the host shell. |

---

## Useful Commands

| Command | Purpose |
|---|---|
| `source ~/qnx800/qnxsdp-env.sh` | Load the QNX SDP environment (required per terminal) |
| `qnxenv` | Shortcut alias for the line above (see QNX Environment) |
| `/home/rakuram/qnx800/host/common/bin/mkqnximage --run` | Boot the VM |
| `/home/rakuram/qnx800/host/common/bin/mkqnximage --getip` | Get the running VM's IP |
| `ssh root@<vm-ip>` | Connect to the VM |
| `echo $QNX_HOST` / `echo $QNX_TARGET` | Verify the environment is loaded |

---

## Troubleshooting

### Issue 1 — `mkqnximage: command not found`

**Command:**

```bash
mkqnximage --getip
```

**Result:**

```text
mkqnximage: command not found
```

**Cause:** A newly opened Ubuntu terminal does not automatically load the
QNX SDP environment, so `mkqnximage`, `qcc`, `mkifs`, `dumpifs`, and
`ntox86_64-gdb` are not on `PATH`.

**Verify:**

```bash
echo $QNX_HOST
echo $QNX_TARGET
```

Both print empty before sourcing.

**Resolution:**

```bash
source ~/qnx800/qnxsdp-env.sh
```

Verify again — you should now see the host/target paths. Either keep using
the full path to `mkqnximage`, or rely on the environment script having put
it on `PATH`.

---

### Issue 2 — `environment variable QNX_TARGET not defined`

**Command:**

```bash
/home/rakuram/qnx800/host/common/bin/mkqnximage --getip
```

**Result:**

```text
environment variable QNX_TARGET not defined
```

**Cause:** Even when invoked by absolute path, `mkqnximage` still depends on
`QNX_HOST`, `QNX_TARGET`, and `MAKEFLAGS` being exported — invoking the
binary directly doesn't set them.

**Resolution:**

```bash
source ~/qnx800/qnxsdp-env.sh
echo $QNX_TARGET   # should print /home/rakuram/qnx800/target/qnx
/home/rakuram/qnx800/host/common/bin/mkqnximage --getip
```

---

### Issue 3 — `startup.sh` cannot be executed

**Cause:** `startup.sh` is a **target-side** script packaged into the QNX
image — it's meant to run inside the booted VM, not on the Ubuntu host.

---

### Issue 4 — Trying to execute `disk-qemu`

**Cause:** `disk-qemu` is a raw disk image, not an executable.

**Resolution:** Boot it via:

```bash
/home/rakuram/qnx800/host/common/bin/mkqnximage --run
```

---

### Issue 5 — Manual QEMU boot hangs at `qnx_sdp.ifs`

**Cause:** Launching QEMU manually omits configuration that `mkqnximage`
normally provides automatically.

**Resolution:** Always launch the VM through the wrapper, not raw QEMU:

```bash
/home/rakuram/qnx800/host/common/bin/mkqnximage --run
```

---

### Issue 6 — Running `mkqnximage --run` from `output/`

**Cause:** `mkqnximage` expects to be executed from the project root, not
from its `output/` subdirectory.

**Resolution:**

```bash
cd ~/qnx_vm_clean
/home/rakuram/qnx800/host/common/bin/mkqnximage --run
```

---

## Lessons Learned

- The QNX SDP environment is **per-terminal** — it must be sourced again in
  every new shell, with no exceptions.
- `command not found` and `QNX_TARGET not defined` are almost always the
  same root cause (environment not sourced) showing up at two different
  points — check this first before digging further.
- Use either the full path to `mkqnximage` or the `qnxenv` alias
  consistently, to avoid ambiguity about whether the environment is loaded.
- `mkqnximage` must be run from the project root (`~/qnx_vm_clean`), never
  from `output/`.
- `disk-qemu` and `startup.sh` are generated artifacts, not commands to run
  directly — `disk-qemu` is data for QEMU, `startup.sh` runs target-side.
- Never invoke QEMU manually for this workflow — `mkqnximage --run` supplies
  configuration a hand-built QEMU command line will miss.

---

## Current Status

- ✅ VM boots reliably via `mkqnximage --run`
- ✅ IP retrieval (`--getip`) and SSH access working
- ✅ Three-terminal build-and-deploy workflow functional for `proc_thread` and similar exercises
- 🔲 "Create VM" (first-time image generation) command not yet documented here
- 🔲 Full `~/qnx_vm_clean` directory layout not yet fully captured

---
[⬅ Back to section index](README.md) · [⬅ Back to docs index](../README.md)
