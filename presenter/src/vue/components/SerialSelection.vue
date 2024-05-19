<script setup lang="ts">

import { useSerialPortStore, SerialPortInfos } from "../store/serial";
import { ref, onMounted } from 'vue'

const ports = ref([] as SerialPortInfos);
const store = useSerialPortStore();
async function selectPort(port_path: string) {
  await store.listenTo(port_path);
}
onMounted(async () => {
  ports.value = await store.getPorts();
});

function skipSerial() {
  store.skipListen()
}

</script>
<template>
  <div style="background-color: beige">
    <div class="container">
      <div class="block">
        <h1 class="title">Select Your Serial Port</h1>
      </div>
      <div class="block" v-for="port in ports">
        <button class="button is-fullwidth is-primary my-3" @click="selectPort(port.path)">
          {{ port.path }}
        </button>
      </div>
      <button @click="skipSerial" class="button is-fullwidth is-warning my-3">Don't Connect Serial</button>
    </div>
  </div>
</template>
