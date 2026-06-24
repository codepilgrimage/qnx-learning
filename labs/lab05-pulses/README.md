# Lab 05 - Pulses: Process Death Notification

> Status: ✅ Complete — verified on QNX SDP 8.0, x86_64, QEMU VM.

## Objective

Build and run `death_pulse` — a program that monitors system-wide process
termination using QNX pulses and the Process Manager event API. By the end
of this lab you will have hands-on experience with:

- `ChannelCreate()` / `ConnectAttach()`
- `SIGEV_PULSE_INIT()` / `SIGEV_MAKE_UPDATEABLE()`
- `MsgRegisterEvent()`
- `procmgr_event_notify(PROCMGR_EVENT_PROCESS_DEATH)`
- `MsgReceivePulse()`

## Prerequisites

- Reading: [`../../docs/03-ipc/pulses.md`](../../docs/03-ipc/pulses.md) — complete that chapter first.
- QNX SDP environment loaded (`source ~/qnx800/qnxsdp-env.sh`)
- QEMU VM running and reachable via SSH (see [`../../docs/00-getting-started/qemu-vm.md`](../../docs/00-getting-started/qemu-vm.md))

## Steps

1. Build on the host:

   ```bash
   source ~/qnx800/qnxsdp-env.sh
   cd labs/lab05-pulses
   make
   ```

2. Copy the binary to the VM:

   ```bash
   scp bin/death_pulse root@<vm-ip>:/tmp/
   ```

3. SSH into the VM and run it:

   ```bash
   ssh root@<vm-ip>
   /tmp/death_pulse
   ```

4. Leave `death_pulse` running. Open a second SSH session and run the
   experiments below.

## Build

```bash
make          # ARM64 (default — matches SDP 8.0 QEMU image)
make x86      # x86_64
make clean    # remove bin/
```

## Experiments

### Experiment 1 — Short-lived command

In the second terminal on the VM:

```bash
pidin
```

Expected in the `death_pulse` terminal:

```text
Process with pid: 663570 died
```

### Experiment 2 — Even shorter-lived

```bash
ls
```

Expected:

```text
Process with pid: 663571 died
```

### Experiment 3 — Background process with a delay

```bash
sleep 3 &
```

Wait three seconds. Expected:

```text
Process with pid: 663572 died
```

This confirms `MsgReceivePulse()` is truly blocking — the thread is
in `RECEIVE`-blocked state and only wakes when the pulse arrives.

## Verifying Thread State with `pidin`

While `death_pulse` is running, in another terminal:

```bash
pidin -P death_pulse threads
```

Expected (look for STATE=RECEIVE):

```text
pid  tid  name         STATE    Blocked
...   1   death_pulse  RECEIVE  4
```

The `4` is the channel ID. The thread is BLOCKED — not consuming CPU.

## Expected Output (startup)

```text
Channel created: chid=4
Connection attached: coid=3
Event registered:         ret=0 errno=0
Subscribed to PROCESS_DEATH: ret=0 errno=0

Waiting for process death events...
(run pidin / ls / sleep 3 & in another terminal)
```

Then for each process that exits:

```text
Process with pid: <pid> died
```

## Challenge / Extension

1. **Add `PROCMGR_EVENT_PROCESS_CREATE`** — add a second channel, event, and
   subscription for `PROCMGR_EVENT_PROCESS_CREATE`. Observe whether create
   pulses arrive on SDP 8.0. Document your findings and compare with the open
   research item in `docs/03-ipc/pulses.md`.

2. **Filter by PID range** — modify the pulse handler to only print deaths of
   processes whose PID is above a threshold (e.g. > 100000), filtering out
   system processes.

3. **Log to a file** — redirect output to `/tmp/death_log.txt` with a
   timestamp for each death event using `clock_gettime(CLOCK_MONOTONIC)`.

4. **Combine with a timer pulse** — add a second `sigevent` backed by a QNX
   timer (`timer_create()` / `timer_settime()`) on the same channel, using a
   different pulse code. In the receive loop, dispatch on `pulse.code` to
   print a heartbeat every 5 seconds alongside the death notifications.

---
[⬅ Back to labs index](../README.md) · [⬅ Back to docs chapter](../../docs/03-ipc/pulses.md)
