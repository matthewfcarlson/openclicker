import { defineStore } from 'pinia'
import { ref, computed } from "vue";
import type { electronAPIType } from '../../electron/presenter_preload';

export const useSerialPortStore = defineStore('serial', () => {
    const setup = ref(false)
    const api = (window as any).electronAPI as electronAPIType;

    const isReady = computed(()=> setup.value == true)

    async function getPorts() {
        const ports = await api.serialPortsList()
        return ports.filter((x)=>x.manufacturer != undefined);
    }
    async function listenTo(path:string) {
        await api.serialPortClose()
        if (await api.serialPortOpen(path) == false) {
            console.error("Failed to open port")
            return;
        }
        setup.value = true;
    }
    function skipListen() {
        setup.value = true;
    }
    async function reset() {
        if (await api.serialPortClose()) {
            setup.value = false;
        }
    }
    return {isReady, getPorts, listenTo, skipListen, reset}
})