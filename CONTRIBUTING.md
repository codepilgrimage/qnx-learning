# Contributing

This started as a personal learning repo, so the conventions below exist
mainly to keep it consistent for the author over time — but corrections and
suggestions are welcome.

## Chapter Conventions (`/docs/`)

- Every chapter follows the standard template (see any existing chapter, e.g.
  `docs/01-qnx-architecture/microkernel.md`, for the exact section order).
- Theory before code. Explain *why* before *how*.
- Diagrams: prefer Mermaid, inline in the markdown file, so it renders
  natively on GitHub. Use `/diagrams/` + `/assets/images/` only for visuals
  Mermaid can't express well.
- Cross-link related chapters (e.g. `spawn()` → process chapter, `MsgSend()`
  → IPC chapter).
- Use blockquotes for tips/warnings/notes:

  ```md
  > **Note:** ...
  > **Warning:** ...
  ```

## Example / API Conventions (`/examples/`, `/labs/`)

For any non-trivial API (e.g. `spawn()`), follow the standard example
template: Overview, Prototype, Parameters, Return Value, Error Codes,
Internal Flow, Process State Changes, Memory Layout, Scheduler Behaviour,
Example Program, Build Command, Run Output, `pidin` Analysis, Debugging
Session, Common Mistakes, Comparison with Linux, Interview Questions.

- Every example must be buildable with the command given in its README.
- Keep examples in `/examples/` minimal and single-concept.
- Keep labs in `/labs/` narrative and end-to-end (objective → steps → build →
  run → expected output → challenge).

## Commit Style

Use conventional, descriptive commits, e.g.:

```
docs(ipc): add pulses chapter
lab(05-pulses): add working pulse handler example
fix(spawn): correct error code table
```

## Status Markers

Use these consistently in file headers:

- 🔲 Not started
- 🟡 In progress
- ✅ Complete

## Adding a New Chapter

1. Copy the template structure from an existing chapter.
2. Add it to the relevant `docs/<section>/README.md` index.
3. If it has a hands-on counterpart, add a matching lab under `/labs/`.
4. Add at least one interview question to the chapter's own section, and
   promote the strongest ones to `/interview/`.
