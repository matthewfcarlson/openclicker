<script setup lang="ts">

import { useSerialPortStore, SerialPortInfos } from "../store/serial";
import { ref, onMounted } from 'vue'
import router from "../router";

const ports = ref([] as SerialPortInfos);
const currentPort = ref("");
const store = useSerialPortStore();
async function selectPort() {
    await store.listenTo(currentPort.value);
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
    <h1>Select Your Serial Port</h1>
    <select v-model="currentPort">
        <option v-for="port in ports"> {{  port.path }}</option>
    </select>
    <button @click="selectPort">Go</button>
    <button @click="skipSerial">Don't Connect Serial</button>
  </div>
</template>
