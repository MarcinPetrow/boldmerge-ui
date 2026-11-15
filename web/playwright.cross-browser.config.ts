import { defineConfig } from '@playwright/test';

export default defineConfig({
  testDir: './test/browser',
  fullyParallel: false,
  reporter: 'line',
  grepInvert: /@visual/,
  use: {
    baseURL: 'http://127.0.0.1:4173',
    headless: true,
    viewport: { width: 1280, height: 900 },
    deviceScaleFactor: 1,
    locale: 'en-US',
    timezoneId: 'UTC',
    colorScheme: 'light',
    reducedMotion: 'no-preference',
  },
  projects: [
    { name: 'firefox', use: { browserName: 'firefox' } },
    { name: 'webkit', use: { browserName: 'webkit' } },
  ],
  webServer: [
    {
      command: 'npm run dev',
      url: 'http://127.0.0.1:4173/test/fixtures/foundation.html',
      reuseExistingServer: false,
      timeout: 30_000,
    },
    {
      command: 'npm run dev:test --prefix ../examples/web',
      url: 'http://127.0.0.1:4181/workflow.html',
      reuseExistingServer: false,
      timeout: 30_000,
    },
  ],
});
