import type { ConfigEnv, UserConfig } from 'vite';
import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import { pluginExposeRenderer } from './vite.base.config';
import {resolve} from "path";

// https://vitejs.dev/config
export default defineConfig((env) => {
  const forgeEnv = env as ConfigEnv<'renderer'>;
  const { root, mode, forgeConfigSelf } = forgeEnv;
  const name = forgeConfigSelf.name ?? '';

  return {
    root,
    mode,
    base: './',
    build: {
      outDir: `.vite/electron/renderer/${name}`,
      rollupOptions: {
        input: {
          main_window: resolve(__dirname, "index.html"),
          modal_window: resolve(__dirname, 'remote.html'),
        },
      },
    },
    plugins: [vue(), pluginExposeRenderer(name),],
    resolve: {
      preserveSymlinks: true,
    },
    clearScreen: false,
  } as UserConfig;
});
