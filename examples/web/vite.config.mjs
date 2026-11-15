import path from 'node:path';
import { defineConfig } from 'vite';

export default defineConfig({
  base: process.env.BM_DEMO_BASE ?? '/',
  build: {
    rollupOptions: {
      input: {
        catalog: path.resolve('index.html'),
        workflow: path.resolve('workflow.html'),
      },
    },
  },
});
