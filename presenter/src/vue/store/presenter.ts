/* The Presenter that communicates with the remotes */
import { defineStore } from 'pinia';
import { ref, computed } from "vue";
import Module from "../../wasm/presenter";
import { LittleStateNames, parsePresenterMessage, PresenterMessageId, ProtocolPresenterSetState, MessageCreators, type PresenterMessage } from '../../common/Protocol';
const ipcRenderer = window.require('electron').ipcRenderer;

export const usePresenterStore = defineStore('presenter', () => {
    const meshDevices = ref(new Map());
    const _wasm = ref(null);

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
        _wasm.value = wasm;
        const little_state = JSON.parse(wasm.ccall('generate_base64_little_state_hash_json', 'string'));
        console.log("little_state", little_state,);
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
            const reply_msg = MessageCreators.PresenterSetStateMultipleChoiceState(wasm, from, 4, "What is your favorite color?");
            console.log("Reply: ", reply_msg);
            sendRawMessageToRemote(reply_msg);

            // Step 2: if it's a state request, send our current state to all remotes
            // Step 2: if it is a heartbeat, update our current list of meshDevices
            // Step 3: publish the message to any subscribers for raw events
        });
        let to = "01:02:03:04:05:06";
        console.log("Dark", MessageCreators.PresenterSetStateDarkState(wasm, to));
        console.log("MC", MessageCreators.PresenterSetStateMultipleChoiceState(wasm, to, 4, "What is your favorite color?"));
        console.log("emoji", MessageCreators.PresenterSetStateEmojiState(wasm, to, 1,2,3,4));
    });

    return {sendRawMessageToRemote, emitRawMessageToRemotes, getRemotes};
    //
});