# QNX Learning

> A living knowledge base for learning QNX from fundamentals to advanced
> topics — written by an engineer moving from AUTOSAR Diagnostics and
> embedded Linux into QNX, Adaptive AUTOSAR, and system software.

[![Status](https://img.shields.io/badge/status-in--progress-yellow)]()
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

## Why This Repo Exists

Most QNX material online is either official reference documentation (accurate
but dry) or scattered forum posts (useful but unstructured). This repo is an
attempt at something in between: a structured, example-driven path through
QNX that also doubles as a portfolio and interview-prep resource.

## Goals

- 📚 Learn QNX from fundamentals to advanced topics
- 🧪 Runnable code and labs for every concept
- 🏗️ Internal architecture explained with diagrams
- 🎯 Interview preparation (MCQs, coding, debugging, system design)
- 💼 A public showcase of practical QNX expertise
- 🔄 Continuously evolving as the author learns more

## Repository Structure

```text
qnx-learning/
├── docs/            # Theory, organized by phase (00-getting-started ... 17-references)
├── labs/             # Hands-on, buildable labs (one per major concept)
├── examples/         # Smaller, focused, single-concept code samples
├── scripts/          # VM and build automation
├── diagrams/         # Mermaid sources / exported diagrams
├── assets/           # Images, screenshots, gifs used in docs
├── interview/         # MCQs, coding questions, debugging scenarios, system design
└── notes/             # Release notes, common errors, troubleshooting, scratchpad
```

See [`docs/README.md`](docs/README.md) for the full documentation index.

## Learning Roadmap

| Phase | Topic |
|---|---|
| 1 | Environment, QEMU, Momentics |
| 2 | QNX Architecture |
| 3 | Processes & Threads |
| 4 | IPC |
| 5 | Resource Managers |
| 6 | Filesystems |
| 7 | Networking |
| 8 | Device Drivers |
| 9 | Debugging & Performance |
| 10 | Security |
| 11 | Adaptive AUTOSAR on QNX |
| 12 | Interview Preparation |

## How Each Chapter Is Structured

Every chapter under `/docs/` follows the same template — Introduction, Why It
Exists, Architecture, Internal Working, API Reference, Example, Kernel
Perspective, Common Mistakes, Debugging, Interview Questions, Summary,
References — so the repo stays predictable to navigate as it grows.

## Getting Started

1. Start with [`docs/00-getting-started/`](docs/00-getting-started/README.md)
   to set up your QNX SDP environment (QEMU or VMware).
2. Work through the phases in order — each one builds on the last.
3. Pair every chapter with its matching lab under [`labs/`](labs/README.md).

## Status

This repo is actively being filled in. Anything marked `🔲 Not started` is a
placeholder generated from the standard template — contributions, corrections,
and suggestions are welcome via issues/PRs once the repo is public.

## License

[MIT](LICENSE) — see license file for details.
