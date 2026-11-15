import path from 'node:path';
import { defineConfig } from 'vite';

export default defineConfig({
  server: {
    fs: {
      allow: [import.meta.dirname, path.resolve(import.meta.dirname, '../../web')],
    },
  },
});
