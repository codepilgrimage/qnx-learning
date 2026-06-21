# QNX SDP 8.0 - Running a QEMU Virtual Machine from Terminal

> Status: ✅ Working — full build → run → stop → SSH workflow verified end-to-end. One follow-up item open (Momentics embedded SSH client).

Author: Rakuram E.

---

## Objective

Learn how to:

- Create a QNX virtual machine from the command line.
- Launch it without using Momentics IDE.
- Enable SSH access.
- Understand common mistakes.
- Document troubleshooting performed.

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
- SSH key pair generated at `~/.ssh/id_ed25519` / `~/.ssh/id_ed25519.pub`
- `mkqnximage` available at `~/qnx800/host/common/bin/mkqnximage` (part of the SDP host tools — no separate QEMU install needed)

---

## Daily Workflow

This is the three-terminal routine used day-to-day, **once the VM image has
already been built** (see **Create VM** below for the one-time setup).

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

```bash
alias qnxenv='source ~/qnx800/qnxsdp-env.sh'
```

Add it to `~/.bashrc`, reload with `source ~/.bashrc`, and every new terminal
only needs:

```bash
qnxenv
```

---

## Create VM

One-time setup: create the project workspace and build the VM image.

### 1. Create a clean workspace

```bash
mkdir -p ~/qnx_vm_clean
cd ~/qnx_vm_clean
```

### 2. Build the VM image

```bash
/home/rakuram/qnx800/host/common/bin/mkqnximage \
    --noprompt \
    --hostname=vmcli \
    --type=qemu \
    --arch=x86_64 \
    --ssh-ident=/home/rakuram/.ssh/id_ed25519.pub
```

This generates `local/` and `output/` under the project root.
**No VM is started by this step** — it only builds the image. Running bare
`mkqnximage` again later will also just rebuild the image, not launch it
(see Troubleshooting, Issue 3).

---

## Run VM

Always run from the project root (`~/qnx_vm_clean`), **never** from `output/`
(see Troubleshooting, Issue 7).

```bash
cd ~/qnx_vm_clean
/home/rakuram/qnx800/host/common/bin/mkqnximage --run
```

Verbose form, useful when debugging a boot issue:

```bash
/home/rakuram/qnx800/host/common/bin/mkqnximage --run --verbose
```

Leave this terminal open for as long as the VM should stay running.

---

## Stop VM

```bash
mkqnximage --stop
```

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

or pin the key explicitly:

```bash
ssh -i ~/.ssh/id_ed25519 root@<vm-ip>
```

Example:

```bash
ssh root@172.31.1.208
```

> Use plain Ubuntu OpenSSH for this — see **SSH Client Notes** below for why
> the Momentics embedded SSH client currently fails against this VM.

---

## Understanding `mkqnximage`

`mkqnximage` has two distinct phases — most of the early confusion in this
guide (Issue 3) came from not realizing they're separate steps.

### Phase 1 — Build

```text
mkqnximage
    ↓
Generate files
Generate disk
Generate IFS
Return
```

Produces `output/` and `local/`. **No VM is started.**

### Phase 2 — Run

```text
mkqnximage --run
    ↓
Starts QEMU
Boots image
Starts sshd
Starts qconn
```

---

## Directory Structure

```text
qnx_vm_clean/
├── local/
└── output/
    ├── disk-qemu
    ├── ifs.bin
    ├── options
    ├── procnto-smp-instr.sym
    └── build/
        └── qemu_options
```

---

## Generated Files

| File | What it is |
|---|---|
| `disk-qemu` | Complete disk image. `file disk-qemu` reports `DOS/MBR boot sector` — expected, not an error. |
| `ifs.bin` | Initial filesystem image — contains `procnto`, `startup`, and drivers. |
| `procnto-smp-instr.sym` | Kernel symbols, used for debugging. |
| `output/build/qemu_options` | QEMU options file. On SDP 8.0 this is **0 bytes** — also expected, not an error. |

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

### Issue 3 — `mkqnximage` only builds, doesn't launch the VM

**Command:**

```bash
mkqnximage
```

**Expected:** VM launches.

**Actual:** Only the image is built (`output/`, `local/` created) — nothing
boots.

**Cause:** Bare `mkqnximage` is the **build** phase only (see Phase 1 above).

**Resolution:**

```bash
mkqnximage --run
```

---

### Issue 4 — `./startup.sh` / `./post_startup.sh` fail on the host

**Cause:** Both scripts are **target-side** — packaged into the image and
executed inside QNX during boot, not on the Ubuntu host.

**Typical error:**

```text
cannot open /proc/boot/build/options
```

because `/proc/boot` only exists inside a running QNX instance — it has no
meaning on Ubuntu.

**Resolution:** Don't execute these from the host shell at all; they run
automatically when the VM boots.

---

### Issue 5 — Trying to execute `disk-qemu` directly

```bash
chmod +x disk-qemu
./disk-qemu
```

**Result:** Random binary output.

**Cause:** `disk-qemu` is a raw disk image, not an executable.

**Resolution:** Boot it via `mkqnximage --run` — never execute it directly.

---

### Issue 6 — Manual QEMU boot hangs at the QNX boot loader

**Command:**

```bash
qemu-system-x86_64 ...
```

Boot stalls at:

```text
QNX Boot Loader
qnx_sdp.ifs...
```

**Cause:** A hand-built QEMU command line is missing configuration that
`mkqnximage` normally supplies automatically.

**Resolution:** Always launch via:

```bash
mkqnximage --run
```

---

### Issue 7 — Running `mkqnximage --run` from `output/`

**Incorrect:**

```bash
cd output
mkqnximage --run
```

**Correct:**

```bash
cd ~/qnx_vm_clean
mkqnximage --run
```

`mkqnximage` must always be run from the project root.

---

## SSH Client Notes — Momentics vs OpenSSH

Initial attempts via the Momentics IDE's embedded terminal failed:

```text
SSH client error: Algorithm negotiation fail
```

Plain Ubuntu OpenSSH, however, connected without issue:

```bash
ssh root@172.31.1.xx
```

This confirms:

- VM networking is working
- `sshd` is running inside the VM
- the SSH keys are valid

The remaining algorithm-negotiation failure appears specific to Momentics'
embedded SSH client implementation, not to the VM itself.

> 🔲 Open follow-up: investigate the Momentics SSH algorithm negotiation failure.

---

## Alternative Hypervisor Considered — VMware

VMware was installed and evaluated as an alternative to QEMU. It failed with:

```text
Could not open /dev/vmmon
Failed to initialize monitor device
vmrun start 255
Unable to start services
Virtual machine monitor failed
```

**Root cause:** VMware's kernel modules are incompatible with the current
Ubuntu kernel version.

**Decision:** Stick with QEMU via `mkqnximage` for this workflow.

---

## Useful Commands

| Command | Purpose |
|---|---|
| `mkqnximage` | Build the VM image (no VM started) |
| `mkqnximage --run` | Launch the VM |
| `mkqnximage --run --verbose` | Launch with verbose boot logging |
| `mkqnximage --stop` | Stop the running VM |
| `mkqnximage --getip` | Print the running VM's IP address |
| `mkqnximage --clean` | Clean build artifacts |
| `mkqnximage --wipe` | Delete the VM entirely |
| `ssh root@<vm-ip>` | Connect to the VM |
| `source ~/qnx800/qnxsdp-env.sh` | Load the QNX SDP environment |

---

## Lessons Learned

- `mkqnximage` builds images by default — `--run` is required to actually launch QEMU.
- `startup.sh` and `post_startup.sh` belong to the target image; they cannot be executed from the host.
- `disk-qemu` is a raw disk image, not an executable.
- An empty `qemu_options` file (0 bytes) is normal for SDP 8.0 — not a sign of a broken build.
- `mkqnximage` must always be run from the project root, not from `output/`.
- The QNX SDP environment must be sourced in every new terminal — `command not found` and `QNX_TARGET not defined` are both symptoms of this.
- Ubuntu's native OpenSSH works correctly against the VM; the Momentics embedded SSH client has a separate, unresolved issue.
- VMware was not viable on this host due to kernel module incompatibility — QEMU is the supported path going forward.

---

## Current Status

- ✅ Environment setup
- ✅ VM generation
- ✅ QEMU boot
- ✅ SSH enabled (via OpenSSH)
- ✅ Command-line workflow understood (build → run → stop → getip → ssh)
- 🔲 Investigate Momentics embedded SSH algorithm negotiation failure

---
[⬅ Back to section index](README.md) · [⬅ Back to docs index](../README.md)
