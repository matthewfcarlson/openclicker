/* The Presenter that communicates with the remotes */
import { defineStore } from 'pinia';
import { ref, computed } from "vue";
import Module from "../../wasm/presenter";
import { LittleStateNames, parsePresenterMessage, PresenterMessageId, ProtocolPresenterSetState, MessageCreators, type PresenterMessage } from '../../common/Protocol';
import type { electronAPIType } from '../../electron/presenter_preload';

export const usePresenterStore = defineStore('presenter', () => {
    const meshDevices = ref(new Map());
    const _wasm = ref(null);
    const api = (window as any).electronAPI as electronAPIType;

    async function sendRawMessageToRemote(msg:string) {
        api.presenterSend(msg)
        api.serialPortSend(msg)
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
    function handleMessage(wasm: any, from: string, message: ReturnType<typeof parsePresenterMessage>) {
        if (message.id == PresenterMessageId.RemoteRequestState) {
            // Respond to the message
            const reply_msg = MessageCreators.PresenterSetStateMultipleChoiceState(wasm, from, 4, "What is your favorite color?");
            console.log("Reply: ", reply_msg);
            sendRawMessageToRemote(reply_msg);
        }
        else if (message.id == PresenterMessageId.RemoteRequestEmojis) {
            // Respond to the message
            const reply_msg = MessageCreators.PresenterOfferEmojiOptions(wasm, from, 128641,128520,129528,127914, 2);
            console.log("Reply: ", reply_msg);
            sendRawMessageToRemote(reply_msg);
        }
        else if (message.id == PresenterMessageId.RemotePickEmojis) {
            // Respond to the message
            const reply_msg = MessageCreators.PresenterAssignEmojis(wasm, from, 128641,128520, 0, 0);
            console.log("Reply: ", reply_msg);
            sendRawMessageToRemote(reply_msg);
        }
    }
    function handleRawMessage(wasm: any, msg: string){
        // Step 1: decode the base64 message and match it against a type
        if (msg.startsWith("MSG=") == false) {
            console.warn("Invalid message", msg)
            return;
        }
        const raw_value = wasm.ccall('message_string_to_json', 'string', ['string',],[msg, ]);
        try {
            const data = JSON.parse(raw_value);
            const from = data.from;
            const to = data.to;
            // TODO: verify that it was to us
            const message = parsePresenterMessage(data['msg']);
            console.log(`From ${from}`, message);
            handleMessage(wasm, from, message);
        }
        catch (e) {
            console.info(raw_value)
            console.warn(e)
        }
    }
    Module().then((wasm) => {
        _wasm.value = wasm;
        const little_state = JSON.parse(wasm.ccall('generate_base64_little_state_hash_json', 'string'));
        console.log("little_state", little_state,);
        api.presenterListen((value) => handleRawMessage(wasm, value));
        api.serialPortListen((value) => handleRawMessage(wasm, value));
        
        // ipcRenderer.on('message-to-presenter', (x, msg:string)=> {
        //     // We assume everything is a bridge unless we get a remote heartbeat or state update request
        //     // Step 1: decode the base64 message and match it against a type
        //     const raw_value = wasm.ccall('message_string_to_json', 'string', ['string',],[msg, ]);
        //     const data = JSON.parse(raw_value);
        //     const from = data.from;
        //     const to = data.to;
        //     // TODO: verify that it was to us
        //     const message = parsePresenterMessage(data['msg']);
        //     console.log(`From ${from}`, message);
        //     if (message.id == PresenterMessageId.RemoteRequestState) {
        //         // Respond to the message
        //         const reply_msg = MessageCreators.PresenterSetStateMultipleChoiceState(wasm, from, 4, "What is your favorite color?");
        //         console.log("Reply: ", reply_msg);
        //         sendRawMessageToRemote(reply_msg);
        //     }

        //     // Step 2: if it's a state request, send our current state to all remotes
        //     // Step 2: if it is a heartbeat, update our current list of meshDevices
        //     // Step 3: publish the message to any subscribers for raw events
        // });
        let to = "01:02:03:04:05:06"; 
        console.log("Dark", MessageCreators.PresenterSetStateDarkState(wasm, to));
        console.log("MC", MessageCreators.PresenterSetStateMultipleChoiceState(wasm, to, 4, "What is your favorite color?"));
    });

    return {sendRawMessageToRemote, emitRawMessageToRemotes, getRemotes};
    //
});