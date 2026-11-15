# Public API stability

BoldMerge Web remains private and pre-release at `0.0.0`. The API baseline is
therefore a review gate, not a SemVer compatibility promise.

`npm run api:check` compares every exported TypeScript declaration and package
export with `public-api-baseline.json`. It also verifies that each selective
component entry has a source module, API document, Rollup input, full-package
registration import, built JavaScript/declaration output, and matching
`sideEffects` metadata.

After an intentional public API change:

1. Build the package with `npm run build`.
2. Review the generated declaration and package-export changes.
3. Run `npm run api:update` to accept the new hashes.
4. Commit the API implementation, documentation, tests, and baseline together.

Never update the baseline merely to make a failing check pass. Renames,
removals, signature changes, package-entry changes, and newly public members
require explicit compatibility review. A future versioning policy must define
deprecation windows and replace this pre-release rule before a stable release.
