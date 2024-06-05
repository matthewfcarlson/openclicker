/* The Presenter that communicates with the remotes */
import { defineStore } from 'pinia';
import { ref, computed } from "vue";
import Module from "../../wasm/presenter";
import { LittleStateNames, parsePresenterMessage, PresenterMessageId, ProtocolPresenterSetState } from '../../common/Protocol';
const ipcRenderer = window.require('electron').ipcRenderer;

export const usePresenterStore = defineStore('presenter', () => {
    const meshDevices = ref(new Map());
    const currentState = ref("dark");

    function sendRawMessageToRemote(msg:string) {
        ipcRenderer.invoke('message-from-presenter', msg);
        // TODO: emit to serial
    }
    function sendRawBase64MessageToRemote(mac_address:string, base64_msg:string) {
        const msg = `MSG=af:af:af:af:af:af->${mac_address} ${base64_msg}`
        sendRawMessageToRemote(msg);
    }
    const getRemotes = computed(() => Array.from(meshDevices.value.keys()));
    function emitRawMessageToRemotes(base64_msg:string) {
        const remotes = getRemotes.value;
        remotes.forEach((mac_address)=> {
            sendRawBase64MessageToRemote(mac_address, base64_msg);
        })
    }
    Module().then((wasm) => {
        ipcRenderer.on('message-to-presenter', (x, msg:string)=> {
            // We assume everything is a bridge unless we get a remote heartbeat or state update request
            // Step 1: decode the base64 message and match it against a type
            const raw_value = wasm.ccall('message_string_to_json', 'string', ['string',],[msg, ]);
            console.log("raw",raw_value);
            const data = JSON.parse(raw_value);
            console.log("json", data);
            const message = parsePresenterMessage(data['msg']);
            console.log("parse", data);
            // Respond to the message
            const from = data.from;
            const to = data.to;
            const reply_msg: ProtocolPresenterSetState = {
                id: PresenterMessageId.PresenterSetState,
                state_name: LittleStateNames.DarkState
            }
            console.log(JSON.stringify(reply_msg))
            const result = wasm.ccall('message_json_to_string', 'string', ['string','string'],[from, JSON.stringify(reply_msg) ]);
            console.log("result", result);
            sendRawMessageToRemote(result);

            // Step 2: if it's a state request, send our current state to all remotes
            // Step 2: if it is a heartbeat, update our current list of meshDevices
            // Step 3: publish the message to any subscribers for raw events
        });
    });

    return {meshDevices, sendRawMessageToRemote, emitRawMessageToRemotes, getRemotes, currentState};
    //
});