import { createHash } from 'node:crypto';
import { access, readFile, readdir, writeFile } from 'node:fs/promises';
import path from 'node:path';

const root = process.cwd();
const baselinePath = path.join(root, 'public-api-baseline.json');
const update = process.argv.includes('--update');

const readText = (file) => readFile(path.join(root, file), 'utf8');
const exists = async (file) => {
  try {
    await access(path.join(root, file));
    return true;
  } catch {
    return false;
  }
};
const hash = (value) => `sha256-${createHash('sha256').update(value).digest('hex')}`;
const fail = (message) => { throw new Error(message); };

const packageJson = JSON.parse(await readText('package.json'));
const rollup = await readText('rollup.config.mjs');
const index = await readText('src/index.ts');
const docs = new Set((await readdir(path.join(root, 'docs'))).filter((file) => file.endsWith('.md')));
const infrastructureEntries = new Set(['.', './theme', './foundation']);

const declarationEntries = Object.entries(packageJson.exports)
  .filter(([, target]) => typeof target === 'object' && target.types && target.import)
  .sort(([left], [right]) => left.localeCompare(right));

for (const [exportName, target] of declarationEntries) {
  const entry = exportName === '.' ? 'index' : exportName.slice(2);
  if (!await exists(target.types.slice(2))) fail(`${exportName}: missing built declaration ${target.types}`);
  if (!await exists(target.import.slice(2))) fail(`${exportName}: missing built module ${target.import}`);
  if (exportName === './foundation') continue;
  if (!packageJson.sideEffects.includes(target.import)) fail(`${exportName}: ${target.import} is absent from sideEffects`);
  if (entry !== 'index' && !rollup.includes(`'${entry}':`) && !rollup.includes(`    ${entry}:`)) fail(`${exportName}: missing Rollup input`);

  if (!infrastructureEntries.has(exportName)) {
    if (!await exists(`src/${entry}.ts`)) fail(`${exportName}: missing source entry`);
    if (!docs.has(`${entry}.md`)) fail(`${exportName}: missing docs/${entry}.md`);
    if (!index.includes(`import './${entry}.js';`)) fail(`${exportName}: full entry does not register it`);
  }
}

const declaredImports = new Set(declarationEntries.map(([, target]) => target.import));
for (const sideEffect of packageJson.sideEffects) {
  if (sideEffect.endsWith('.js') && !declaredImports.has(sideEffect)) fail(`${sideEffect}: side effect has no public export`);
}

const api = {
  schemaVersion: 1,
  package: packageJson.name,
  version: packageJson.version,
  exports: Object.fromEntries(await Promise.all(Object.entries(packageJson.exports).sort(([left], [right]) => left.localeCompare(right)).map(async ([exportName, target]) => {
    if (typeof target === 'string') return [exportName, target];
    const declaration = await readText(target.types.slice(2));
    return [exportName, { ...target, declaration: hash(declaration) }];
  }))),
};

const serialized = `${JSON.stringify(api, null, 2)}\n`;
if (update) {
  await writeFile(baselinePath, serialized);
  console.log(`Updated ${path.relative(root, baselinePath)} with ${declarationEntries.length} declaration entries.`);
} else {
  if (!await exists('public-api-baseline.json')) fail('Missing public-api-baseline.json; run npm run api:update after an intentional API review.');
  const baseline = await readFile(baselinePath, 'utf8');
  if (baseline !== serialized) fail('Public API differs from public-api-baseline.json; review the declaration/export diff and run npm run api:update only for an intentional change.');
  console.log(`Public API baseline matches ${declarationEntries.length} declaration entries; selective sources, docs, Rollup inputs, and side effects are consistent.`);
}
