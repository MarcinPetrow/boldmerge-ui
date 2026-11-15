# Repository Guidelines

## Project Structure & Module Organization

- `tokens/` is the canonical three-layer token source: primitives, semantic
  roles, and component tokens. JSON is build-time input only.
- `scripts/` contains the dependency-free Node.js token compiler and validators.
- `dist/` and `examples/output/` contain deterministic generated CSS/GTK CSS.
- `web/` is the independent `@boldmerge/web` package. Components live in
  `web/src/components/`, foundations in `web/src/foundations/`, demos in
  `web/demo/`, and Vitest/Playwright tests in `web/test/`.
- `native/` contains the independent C/GObject GTK implementation, with public
  headers under `native/include/boldmerge/`, sources in `native/src/`, and GLib
  tests in `native/tests/`.
- `docs/` records implementation plans, decisions, and release readiness. Treat
  `BOLDMERGE_ADAPTIVE_SPEC.md` as the approved architecture source.

## Current Implementation Snapshot

Web batches through **5B-B** are complete: foundations, core controls,
application shell, overlays/modals/feedback, Combobox, Command Palette, Table,
Pagination, Avatar/Avatar Group, and Empty State. Public entries are explicit
under `@boldmerge/web/*`; Table intentionally enhances consumer-authored native
HTML rather than rendering row data. Decisions are recorded through BM-034.

The latest executed Web check passed token synchronization, typecheck, Rollup,
34/34 Vitest tests, and 94/94 Playwright tests. Thirty Linux Chromium snapshots
are present. The standalone catalog build passed with 47 transformed modules
and is served from `examples/web/` on port 4180. Do not treat these counts as
current after edits without rerunning the commands.

The exact next reviewable milestone is **5B-C — Skeleton**: approved 1.5s
shimmer, static reduced-motion behavior, API docs, selective export, demo, and
Light/Dark × Balanced/Compact tests. Native has foundations plus Button, Input,
and Card; consult `docs/V1_RELEASE_READINESS.md` before claiming broader parity.

## Build, Test, and Development Commands

From the repository root:

```sh
npm run check          # validate tokens, generated artifacts, and token tests
npm run tokens:build   # regenerate all canonical platform artifacts
```

For Web development:

```sh
cd web
npm run dev            # Vite demo server on 127.0.0.1:4173
npm run check          # token sync, typecheck, build, Vitest, Playwright
```

For the standalone component catalog:

```sh
cd examples/web
npm run dev            # catalog on 127.0.0.1:4180
npm run build          # consumer typecheck and production build
```

For Native development, when GTK dependencies are installed:

```sh
meson setup native/build native
meson compile -C native/build
meson test -C native/build --print-errorlogs
```

## Coding Style & Naming Conventions

Use two-space indentation in JavaScript, TypeScript, JSON, Meson, and C. Follow
existing semicolon and brace style. Use kebab-case filenames, `BoldMerge*` for
Web classes and `bm_`/`Bm` for C symbols/types. Custom elements use the
`bm-` prefix. Edit canonical token JSON, never generated files directly; run
`npm run tokens:build` afterward. Do not introduce a shared runtime or runtime
JSON renderer between Web and Native.

## Testing Guidelines

Node token tests use `test/*.test.mjs`; Web unit tests use Vitest and browser,
keyboard, and screenshot tests use Playwright. Native tests use GLib's test
framework. Add behavior tests with every public API change and platform-specific
visual baselines for visual changes. Cover Light/Dark and Balanced/Compact
without inferring density from viewport size. Report blocked or skipped tests;
never describe an unexecuted check as passing.

## Commit & Pull Request Guidelines

This workspace has no repository-local Git history, so no established commit
convention can be inferred. Use short, imperative, scoped subjects such as
`web: add radio group contract`. Pull requests should explain intent, list
changed public APIs and token decisions, include executed commands/results,
link relevant issues or decision records, and attach reviewed Web/GTK
screenshots for visual changes. Never publish, tag, or push release artifacts
without explicit authorization.
