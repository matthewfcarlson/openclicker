<script setup lang="ts">

import remote_url from "../wasm/remote.js?url";
import { ref, onMounted, onUnmounted } from 'vue'

globalThis.Module = {} as any;
/*
    presenter_relay: function(raw_msg) {
      const msg = UTF8ToString(raw_msg);
      window.electronAPI.sendMessageToPresenter(msg);
      console.log("Presenter relay", msg);
    },
*/

// // Depending on the build flags that one uses, different files need to be downloaded
// // to load the compiled page. The right set of files will be expanded to be downloaded
// // via the directive below.
// function binary(url) { // Downloads a binary file and outputs it in the specified callback
//     return new Promise((ok, err) => {
//       var x = new XMLHttpRequest();
//       x.open('GET', url, true);
//       x.responseType = 'arraybuffer';
//       x.onload = () => { ok(x.response); }
//       x.send(null);
//     });
//   }

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

function fakePresenter() {
  const mac = globalThis.Module.ccall('get_remote_mac', 'string', []);
  // TODO: pass the real thing 
  // const msg = `MSG=${mac} GGRhcmsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=`;
  const fakeMsg = globalThis.Module.ccall('fake_presenter_state_dark', 'string', []);
  // console.log(msg);
  console.log(fakeMsg);
  // globalThis.Module.ccall('send_presenter_msg', 'void', ['string',],[msg, ]);
  
}


onMounted(async () => {
  console.log(window.electronAPI);
  // , binary(wasm_url)
  await script(remote_url);

  window.electronAPI.listenToPresenterMessage((msg)=> {
    console.log("Presenter says", msg);
    globalThis.Module.ccall('send_presenter_msg', 'void', ['string',],[msg, ]);
  })
  // await binary(wasm_url);
  // console.log(globalThis.Module)
  // console.log(globalThis.Module['main']())
  //console.warn("remote_mac", globalThis.Module.ccall('get_remote_mac', 'string', []));
});
</script>


<template>
  <canvas id="remote-canvas" willReadFrequently="true"></canvas>
  <button @click="fakePresenter">Fake Presenter Dark</button>
</template>

<style>
#remote-canvas {
  min-width: 0;
  min-height: 0;
  margin:1em;
}
</style>
