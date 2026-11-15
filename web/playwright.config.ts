import { defineConfig } from '@playwright/test';

export default defineConfig({
  testDir: './test/browser',
  fullyParallel: false,
  reporter: 'line',
  use: {
    baseURL: 'http://127.0.0.1:4173',
    browserName: 'chromium',
    headless: true,
    viewport: { width: 1280, height: 900 },
    deviceScaleFactor: 1,
    locale: 'en-US',
    timezoneId: 'UTC',
    colorScheme: 'light',
    reducedMotion: 'no-preference',
  },
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
