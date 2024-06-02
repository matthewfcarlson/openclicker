<script setup lang="ts">

import remote_url from "../wasm/remote.js?url";
import wasm_url from "../wasm/remote.wasm?url";

import { ref, onMounted, onUnmounted } from 'vue'

globalThis.Module = {} as any;

// Depending on the build flags that one uses, different files need to be downloaded
// to load the compiled page. The right set of files will be expanded to be downloaded
// via the directive below.
function binary(url) { // Downloads a binary file and outputs it in the specified callback
    return new Promise((ok, err) => {
      var x = new XMLHttpRequest();
      x.open('GET', url, true);
      x.responseType = 'arraybuffer';
      x.onload = () => { ok(x.response); }
      x.send(null);
    });
  }

function script(url) { // Downloads a script file and adds it to DOM
  return new Promise((ok, err) => {
    var s = document.createElement('script');
    s.src = url;
    s.onload = () => {
      ok();
    };
    document.body.appendChild(s);
  });
}


onMounted(async () => {
  Promise.all([binary(remote_url), binary(wasm_url)]).then((e => { 
    globalThis.Module.wasm = e[1]; 
    var r = URL.createObjectURL(new Blob([e[0]], { type: "application/javascript" })); 
    script(r).then((() => { URL.revokeObjectURL(r) })) 
  }))
});
</script>


<template>
  <canvas id="remote-canvas" willReadFrequently="true"></canvas>
</template>

<style>
#remote-canvas {
  min-width: 0;
  min-height: 0;
  margin:1em;
}
</style>
