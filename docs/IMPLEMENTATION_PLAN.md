# BoldMerge Adaptive implementation plan

Status: canonical token pipeline, initial Web foundation, and initial Native foundation implemented locally  
Approved product/architecture source: [`../BOLDMERGE_ADAPTIVE_SPEC.md`](../BOLDMERGE_ADAPTIVE_SPEC.md)  
Plan date: 2026-11-10

## 1. Repository classification and inventory

This workspace began as an uninitialized design-system bootstrap and remains an
untracked directory under the parent Git workspace. It now contains the
canonical token source and generated artifacts plus independent `web/` and
`native/` framework prototypes. Repository identity, ownership, and the split
or monorepo distribution decision remain unresolved; nothing is published.

The local layout is now a development integration workspace rather than a claim
of a third shared runtime. Canonical JSON remains build-time-only, and Web and
Native retain independent dependencies, APIs, builds, tests, and release
cycles. The directory/repository should be named and initialized deliberately
before any remote or publication is created.

What now exists:

- the complete approved architecture and design baseline in
  `BOLDMERGE_ADAPTIVE_SPEC.md`;
- canonical three-layer JSON tokens with deterministic browser and GTK exports;
- an independent Web foundation and initial component slices;
- an independent Native foundation skeleton under `native/`.

What still does not exist:

- a repository-local Git history or established specification release version;
- a decided cross-repository artifact distribution mechanism;
- CI and an approved platform support matrix;
- the remaining Web or Native v1.0 component libraries.

## 2. Gaps, tensions, and risks

The baseline explicitly says unspecified numeric values are proposals requiring
validation. The implementation must preserve that distinction in token metadata
and decision records.

### Blocking before a production foundation release

These do not block the next small milestone, but they block declaring the
foundation complete or handing a stable contract to both frameworks:

1. **Repository identity and ownership.** The current path and Git state do not
   establish this as the canonical `boldmerge-design-system` repository.
2. **Specification version.** Implementations must declare the specification
   version they implement, but the approved baseline has no SemVer identifier.
3. **Theme palettes.** Accent seeds are supplied, but complete primitive and
   semantic Light/Dark colors (text, surfaces, borders, focus, disabled, and
   semantic status colors) are not specified.
4. **Accent generation/contrast policy.** The OKLCH scale algorithm, gamut
   mapping, tone stops, fallback behavior, and the semantic usages against which
   custom accents are accepted or adjusted are unspecified.
5. **Accent-surface calibration.** “Approximately 50% greater transparency”
   refers to unavailable earlier visuals. Final opacity cannot be approved
   without those references or a replacement measurable target.
6. **Cross-repository artifact contract.** Version compatibility, distribution,
   and consumption of generated CSS/GTK CSS are not defined. Copying generated
   files by hand would make drift likely.
7. **Supported platform matrix.** Minimum browser versions, Node version, GTK 4,
   libadwaita, compiler, and Ubuntu versions are missing; these are required for
   reproducible CI and public support claims.

### Safe provisional defaults, subject to validation

These details can be chosen locally without changing the approved product
intent, provided they are documented and tested:

- npm with a committed `package-lock.json` for this JavaScript-only build tool;
- TypeScript in strict mode for build scripts, JSON Schema Draft 2020-12 for
  source validation, and generated files under `dist/`;
- slash-delimited logical token identifiers mapped mechanically to
  `--bm-...` CSS names and `@define-color bm_...`/GTK-compatible output names;
- UTF-8, LF, stable lexical ordering, fixed numeric serialization, and generated
  file headers for deterministic builds;
- `4px 8px` interpreted as vertical then horizontal tag padding;
- `fluid` as the page-layout default and Overlay as the drawer default, exactly
  as stated;
- CSS logical properties and start/end naming for direction-aware tokens;
- `prefers-reduced-motion` as the Web system preference hook, while keeping an
  explicit override possible.

Defaults must not invent production colors, font metrics, component public APIs,
or accessibility claims.

### Apparent contradictions and required interpretations

- Compact controls may be 32–40px high while touch targets must be at least
  44px in touch-oriented contexts. Treat compact dimensions as visual sizes,
  and require an expanded hit area or Balanced density when the interaction
  context is touch-oriented. Density must never be inferred solely from
  viewport width.
- Tooltip animation duration (120ms) and hover delay (400ms) describe different
  phases and are compatible: delay before opening, then transition duration.
- “Two independent frameworks” and their placement in one frameworks monorepo
  are compatible only if packages, dependencies, builds, tests, releases, and
  runtime code remain independent. Shared CI orchestration is acceptable;
  shared UI/runtime modules are not.
- Vite for Web development and Rollup for library output are compatible if Vite
  is not treated as the library publisher. The release build should invoke a
  reviewed Rollup configuration directly, with Vite limited to docs/examples.
- Canonical token JSON and the ban on a JSON-driven renderer are compatible:
  JSON is build-time design input only. Neither framework may load it at runtime
  to construct UI.

### Additional implementation risks

- GTK CSS cannot reproduce CSS custom-property cascading or arbitrary runtime
  OKLCH generation. Native will need explicit theme/accent propagation and
  likely precomputed/validated values, without becoming coupled to Web.
- Inter and JetBrains Mono delivery, font fallback, and Lucide asset licensing
  and version pinning need decisions before deterministic visual baselines.
- Open Shadow DOM still requires a deliberate public `::part()` and slot API;
  exposing internal parts accidentally creates permanent v1 API surface.
- Form-associated custom element behavior, browser autofill, accessible floating
  labels, and integrated validation are high-risk Web areas.
- GTK accessibility behavior and deterministic screenshots depend heavily on
  the supported GTK/libadwaita and renderer matrix.
- Arbitrary consumer accents can fail text/non-text contrast. Restricting usage,
  selecting contrast tones, or rejecting unsafe combinations must be decided.
- RTL affects physical descriptions such as the sidebar indicator and gradient
  anchor. Public values should prefer logical directions while preserving the
  specified visual intent.
- Cross-platform “visual parity” can drift without shared semantic conformance
  cases, even though cross-platform pixel diffs are correctly forbidden.

## 3. Incremental repository layout

Create only the directories needed by each milestone. Do not mass-scaffold the
component catalog.

### Milestone 1: token contract and non-color foundations

```text
boldmerge-design-system/
  BOLDMERGE_ADAPTIVE_SPEC.md
  package.json
  package-lock.json
  tsconfig.json
  tokens/
    schema/
      token-file.schema.json
    primitives/
      geometry.json
      motion.json
      typography.json
    semantic/
      density.json
  scripts/
    build-tokens.ts
    check-generated.ts
    lib/
      load-token-files.ts
      resolve-references.ts
      emit-css.ts
      emit-gtk-css.ts
  tests/
    fixtures/
    token-schema.test.ts
    token-resolution.test.ts
    generated-output.test.ts
  dist/                         # generated, never hand-edited
    css/foundations.css
    gtk/foundations.css
  docs/
    IMPLEMENTATION_PLAN.md
    DECISIONS.md
```

Milestone 1 deliberately excludes production color tokens, accent generation,
fonts/icons as binary assets, component tokens, and all runtime components.
Typography source may record approved family names and sizes only where the
baseline supplies them; it must not invent a full type scale.

### Milestone 2: validated theme and accent foundations

After the palette and accent blockers are decided, add primitive colors,
Light/Dark semantic mappings, five preset scales, custom-accent generation,
contrast validation, theme/density examples, and approved platform-specific
goldens. Generated outputs remain static build artifacts.

### Milestone 3: foundation specifications and versioned contract

Add focused documents under `specifications/foundations/`, reference assets,
license notices, compatibility policy, changelog, and a tagged pre-release of
the design contract. Add `components/` token files only when the first component
specification requires them.

### Subsequent work in `boldmerge-frameworks` (separate repository)

Bootstrap `web/` only after Milestone 3 is consumable. Implement foundations and
a narrow vertical slice (Button plus Card) before expanding Core. Bootstrap
`native/` after the shared semantic contract is stable enough to exercise GTK
constraints. Neither implementation imports the other, and there is no shared
runtime package or JSON UI renderer.

## 4. Tooling and dependencies

The implemented foundation uses standard Node.js ECMAScript modules and the
built-in test runner. It has no third-party production or development
dependencies. The local JSON Schema documents the contract; the build performs
the corresponding structural, reference, context, type, unit, and contrast
checks explicitly.

Do not add Lit, Vite, Rollup, Playwright, GTK, or libadwaita to this repository;
they belong to the appropriate framework implementation. Do not add a general
token framework until the schema/export needs prove it is preferable to the
small explicit exporters above. Production dependencies should remain empty.

Expected commands once Milestone 1 is implemented:

```sh
npm ci
npm run lint
npm run tokens:validate
npm run tokens:build
npm run tokens:check-generated
npm test
npm run check
```

`check` should run all non-install checks in a stable order. CI should run
`npm ci` and `npm run check` on pull requests. `tokens:check-generated` rebuilds
into a temporary directory and compares bytes with `dist/`, failing on drift;
it must not rewrite tracked output during the check.

Future frameworks commands, to be established in their own repository, are:

```sh
npm --prefix web ci
npm --prefix web run build
npm --prefix web run test
npm --prefix web run test:e2e

meson setup native/build native
meson compile -C native/build
meson test -C native/build
```

The platform matrix and exact CI containers must be decided before these become
support commitments.

## 5. Exact next milestone

**Milestone M1: deterministic token contract for approved non-color
foundations.**

Deliver the Milestone 1 files listed above and no components. Encode only values
already approved in the baseline: radii where explicit, density dimensions,
motion durations/easing, icon sizes/stroke, touch-target minimum, and explicitly
stated typography facts. Each token must include a stable name, type, value or
reference, description, and provenance/status (`approved` or `proposal`). The
build resolves aliases, rejects cycles and unknown references, and emits static
CSS and GTK CSS.

Acceptance criteria:

1. Every source token file validates against the checked-in schema; unknown
   fields, duplicate logical names, invalid units, unresolved references, and
   reference cycles fail with actionable file/token diagnostics.
2. Approved Balanced/Compact measurements and motion values in generated CSS
   match `BOLDMERGE_ADAPTIVE_SPEC.md` exactly. No unspecified production color
   or complete type-scale values are introduced.
3. CSS output uses a documented `--bm-` namespace. GTK output is syntactically
   scoped/documented for later framework consumption and does not imply browser
   cascade equivalence.
4. Two clean builds are byte-identical, and the generated-output drift check
   passes without modifying tracked files.
5. Unit tests cover schema failure, alias resolution, cycles, unit formatting,
   both densities, and exporter golden output.
6. `npm ci` followed by `npm run check` succeeds in a clean checkout on the
   selected Node version, and CI runs that sequence on pull requests.
7. Generated artifacts contain no executable code, runtime JSON loader, UI
   schema, component registry, Web/GTK runtime dependency, or cross-framework
   import.
8. Documentation states the still-open palette, accent, typography, icon,
   platform, versioning, and distribution decisions without claiming the
   foundations or accessibility work complete.

M1 is complete only when all eight criteria are demonstrated. It is not a Web
or Native release and must not be published as one.

