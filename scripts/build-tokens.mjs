#!/usr/bin/env node
import { mkdir, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { buildOutputs, loadTokenSystem } from './lib/token-system.mjs';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const validateOnly = process.argv.includes('--validate-only');

try {
  const records = await loadTokenSystem(root);
  if (validateOnly) {
    process.stdout.write(`Validated ${records.filter((record) => !record.generated).length} source tokens and ${records.filter((record) => record.generated).length} generated accent tones.\n`);
  } else {
    const outputs = buildOutputs(records);
    for (const [relative, content] of Object.entries(outputs)) {
      const destination = path.join(root, relative);
      await mkdir(path.dirname(destination), { recursive: true });
      await writeFile(destination, content, 'utf8');
    }
    process.stdout.write(`Generated ${Object.keys(outputs).join(', ')}.\n`);
  }
} catch (error) {
  process.stderr.write(`${error.stack ?? error.message}\n`);
  process.exitCode = 1;
}
