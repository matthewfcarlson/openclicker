import type { ForgeConfig } from '@electron-forge/shared-types';
import { MakerSquirrel } from '@electron-forge/maker-squirrel';
import { MakerZIP } from '@electron-forge/maker-zip';
// import { MakerDeb } from '@electron-forge/maker-deb';
// import { MakerRpm } from '@electron-forge/maker-rpm';
import { VitePlugin } from '@electron-forge/plugin-vite';
import { FusesPlugin } from '@electron-forge/plugin-fuses';
import { FuseV1Options, FuseVersion } from '@electron/fuses';
import * as fs from 'fs';
import * as path from 'path';

// Function to get all TypeScript files in a directory recursively
function getAllTypeScriptFiles(dir: string, files: string[] = []): string[] {
    const entries = fs.readdirSync(dir, { withFileTypes: true });

    for (const entry of entries) {
        const fullPath = path.resolve(path.join(dir, entry.name));
        if (entry.isDirectory()) {
            getAllTypeScriptFiles(fullPath, files);
        } else if (entry.isFile() && entry.name.endsWith('.ts')) {
            files.push(fullPath);
        }
    }
    return files;
}

// Function to dynamically import all TypeScript files
async function importTypeScriptFiles(files: string[]) {
    const name_to_lesson_map: Map<string,object> = new Map();
    for (const file of files) {
        // Remove the file extension and replace \ with / for Windows compatibility
        const importPath = file.replace(/\\/g, '/').replace(/\.ts$/, '');
        const value = await import(importPath);
        const lesson_name = path.basename(importPath)
        try{
          name_to_lesson_map.set(lesson_name, value.default)
        }
        catch {
          name_to_lesson_map.set(lesson_name, value)
        }
    }
    return name_to_lesson_map;
}


const config: ForgeConfig = {
  packagerConfig: {
    asar: true,
    executableName: "openclicker",
    // appBundleId: utils.fromBuildIdentifier({ beta: 'com.openclicker.app', prod: 'com.app' })
  },
  buildIdentifier:  process.env.IS_BETA ? 'beta' : 'prod',
  rebuildConfig: {},
  makers: [
    new MakerSquirrel({}), 
    new MakerZIP({}, ['darwin','linux', 'win32']), 
    // new MakerRpm({}), 
    // new MakerDeb({}),
  ],
  plugins: [
    new VitePlugin({
      // `build` can specify multiple entry builds, which can be Main process, Preload scripts, Worker process, etc.
      // If you are familiar with Vite configuration, it will look really familiar.
      build: [
        {
          // `entry` is just an alias for `build.lib.entry` in the corresponding file of `config`.
          entry: 'src/electron/main.ts',
          config: 'vite.main.config.ts',
        },
        {
          entry: 'src/electron/remote_preload.ts',
          config: 'vite.preload.config.ts',
        },
        {
          entry: 'src/electron/presenter_preload.ts',
          config: 'vite.preload.config.ts',
        },
      ],
      renderer: [
        {
          name: 'main_window',
          config: 'vite.renderer.config.ts',
        },
      ],
    }),
    // Fuses are used to enable/disable various Electron functionality
    // at package time, before code signing the application
    new FusesPlugin({
      version: FuseVersion.V1,
      [FuseV1Options.RunAsNode]: false,
      [FuseV1Options.EnableCookieEncryption]: true,
      [FuseV1Options.EnableNodeOptionsEnvironmentVariable]: false,
      [FuseV1Options.EnableNodeCliInspectArguments]: false,
      [FuseV1Options.EnableEmbeddedAsarIntegrityValidation]: true,
      [FuseV1Options.OnlyLoadAppFromAsar]: true,
    }),
  ],
  hooks: {
    generateAssets: async () => {
      const lesson_files = getAllTypeScriptFiles("./src/lessons");
      const lessons = await importTypeScriptFiles(lesson_files);
      fs.writeFileSync("./src/lessons/built-in-lessons.json", JSON.stringify(Object.fromEntries(lessons)));
      console.log(`Generated ${lessons.size} lessons`)
    }
  }
};

export default config;
