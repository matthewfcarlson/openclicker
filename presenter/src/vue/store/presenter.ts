/* The Presenter that communicates with the remotes */
import { defineStore } from 'pinia';
import { ref, computed } from "vue";
const ipcRenderer = window.require('electron').ipcRenderer;

export const usePresenterStore = defineStore('presenter', () => {
    const meshDevices = ref(new Map());
    const currentState = ref("dark");
    ipcRenderer.on('message-to-presenter', (x, msg:string)=> {
        //
        // We assume everything is a bridge unless we get a remote heartbeat or state update request
        if (msg.startsWith("MSG=") == false) {
            console.error("Unknown message: ", msg);
            return;
        }
        const real_msg = msg.substring(4);
        const mac_parts = real_msg.split("->", 2);
        const from_mac = mac_parts[0].trim();
        if (!meshDevices.value.has(from_mac)) {
            meshDevices.value.set(from_mac, {});
        }
        const to_mac_and_msg = mac_parts[1].split(" ", 2)
        const to_mac = to_mac_and_msg[0].trim();
        if (to_mac != "af:af:af:af:af:af") {
            console.error("Message to presenter is not to presenter", msg);
            return;
        }
        const base64_msg = to_mac_and_msg[1].trim();
        console.log(`Message from ${from_mac} = ${base64_msg}`);
        // Step 1: decode the base64 message and match it against a type
        // Step 2: if it's a state request, send our current state to all remotes
        // Step 2: if it is a heartbeat, update our current list of meshDevices
        // Step 3: publish the message to any subscribers for raw events
    });
    function sendRawMessageToRemote(mac_address:string, base64_msg:string) {
        const msg = `MSG=af:af:af:af:af:af->${mac_address} ${base64_msg}`
        ipcRenderer.invoke('message-from-presenter', msg);
        // TODO: emit to serial
    }
    const getRemotes = computed(() => Array.from(meshDevices.value.keys()));
    function emitRawMessageToRemotes(base64_msg:string) {
        const remotes = getRemotes.value;
        remotes.forEach((mac_address)=> { 
            sendRawMessageToRemote(mac_address, base64_msg);
        })
    }
    return {meshDevices, sendRawMessageToRemote, emitRawMessageToRemotes, getRemotes, currentState};
    // 
});