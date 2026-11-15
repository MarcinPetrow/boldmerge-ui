import { cp, mkdir, readFile, writeFile } from 'node:fs/promises';

await mkdir(new URL('../dist/types/', import.meta.url), { recursive: true });
await cp(new URL('../.build/types/', import.meta.url), new URL('../dist/types/', import.meta.url), { recursive: true });
await cp(new URL('../src/styles/tokens.css', import.meta.url), new URL('../dist/tokens.css', import.meta.url));
const tokens = await readFile(new URL('../src/styles/tokens.css', import.meta.url), 'utf8');
const foundations = await readFile(new URL('../src/styles/foundations.css', import.meta.url), 'utf8');
const fontPackages = ['@fontsource-variable/inter', '@fontsource-variable/jetbrains-mono'];
let fontCss = '';
await mkdir(new URL('../dist/fonts/', import.meta.url), { recursive: true });
for (const packageName of fontPackages) {
  const packageRoot = new URL(`../node_modules/${packageName}/`, import.meta.url);
  const css = await readFile(new URL('wght.css', packageRoot), 'utf8');
  const files = [...css.matchAll(/\.\/files\/([^\)]+\.woff2)/g)].map((match) => match[1]);
  for (const file of new Set(files)) {
    await cp(new URL(`files/${file}`, packageRoot), new URL(`../dist/fonts/${file}`, import.meta.url));
  }
  fontCss += css
    .replaceAll("font-family: 'Inter Variable'", "font-family: 'Inter'")
    .replaceAll("font-family: 'JetBrains Mono Variable'", "font-family: 'JetBrains Mono'")
    .replaceAll('url(./files/', 'url(./fonts/');
  if (!fontCss.endsWith('\n')) fontCss += '\n';
}
await writeFile(new URL('../dist/fonts.css', import.meta.url), fontCss, 'utf8');
await mkdir(new URL('../dist/licenses/', import.meta.url), { recursive: true });
await cp(new URL('../node_modules/@fontsource-variable/inter/LICENSE', import.meta.url), new URL('../dist/licenses/Inter-OFL-1.1.txt', import.meta.url));
await cp(new URL('../node_modules/@fontsource-variable/jetbrains-mono/LICENSE', import.meta.url), new URL('../dist/licenses/JetBrains-Mono-OFL-1.1.txt', import.meta.url));
await cp(new URL('../node_modules/lucide/LICENSE', import.meta.url), new URL('../dist/licenses/Lucide-ISC-MIT.txt', import.meta.url));
const foundationsWithoutFontImport = foundations.replace("@import './fonts.css';\n\n", '');
await writeFile(new URL('../dist/styles.css', import.meta.url), `@import './fonts.css';\n\n${tokens}\n${foundationsWithoutFontImport}`, 'utf8');
