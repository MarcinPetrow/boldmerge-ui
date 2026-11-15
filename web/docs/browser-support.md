# Browser support and test matrix

BoldMerge Web is private pre-release software. The matrix below records current
engineering evidence; it is not yet a stable browser-version support promise.

| Engine | Current evidence | Visual baselines | Status |
| --- | --- | --- | --- |
| Chromium 1254 | 144/144 Playwright tests | 61 Linux baselines | Passed |
| Firefox 1572 | 95/95 portable behavior tests | Not established | Passed for behavior |
| WebKit 2388 | 94/95 portable behavior tests passed; 1 forced-colors test skipped | Not established | Passed for supported behavior |

The cross-browser configuration excludes tests tagged `@visual`. Pixel
baselines remain renderer-, operating-system-, font-, and DPI-specific and must
not be reused across engines. Portable behavior still covers public component
semantics, forms, keyboard interaction, focus management, RTL/reflow, reduced
motion, forced colors where supported by Playwright, selective registration,
and the standalone consumer workflow.

Commands:

```sh
npm run test:e2e          # Chromium behavior plus reviewed visual baselines
npm run test:firefox      # Firefox portable behavior
npm run test:webkit       # WebKit portable behavior
npm run test:browser-matrix
```

The latest combined matrix passed 189/190 tests: all 95 Firefox tests and 94
WebKit tests passed, with the single WebKit forced-colors assertion skipped.

The current Linux host now provides the Playwright WebKit runtime dependencies.
Playwright WebKit does not emulate the `forced-colors` media feature, so that
single engine-specific assertion is explicitly skipped; Chromium and Firefox
retain the forced-colors evidence. No other WebKit behavior test is skipped.

Before stable release, maintainers must define supported minimum browser
versions, run the matrix in a pinned CI image, decide whether Firefox/WebKit
visual baselines add useful release
coverage, and document the relationship between Playwright WebKit and supported
Safari versions. Passing the Playwright engine alone is not a claim about every
shipping browser or operating system.
