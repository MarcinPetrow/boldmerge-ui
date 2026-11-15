#!/usr/bin/env node
import { readFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { buildOutputs, loadTokenSystem } from './lib/token-system.mjs';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');

try {
  const expected = buildOutputs(await loadTokenSystem(root));
  const drift = [];
  for (const [relative, content] of Object.entries(expected)) {
    let actual;
    try {
      actual = await readFile(path.join(root, relative), 'utf8');
    } catch {
      drift.push(`${relative} is missing`);
      continue;
    }
    if (actual !== content) drift.push(`${relative} is stale`);
  }
  if (drift.length > 0) throw new Error(`${drift.join('\n')}\nRun npm run tokens:build.`);
  process.stdout.write(`Verified ${Object.keys(expected).length} deterministic generated artifacts.\n`);
} catch (error) {
  process.stderr.write(`${error.stack ?? error.message}\n`);
  process.exitCode = 1;
}
