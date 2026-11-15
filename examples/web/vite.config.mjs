import path from 'node:path';
import { defineConfig } from 'vite';

export default defineConfig({
  build: {
    rollupOptions: {
      input: {
        catalog: path.resolve('index.html'),
        workflow: path.resolve('workflow.html'),
      },
    },
  },
});
