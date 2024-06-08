/**
 AUTOGENERATED ON 2024-06-08 11:47:35.416553
 DO NOT MODIFY THIS FILE MANUALLY
 IT WILL BE OVERWRITTEN ON THE NEXT BUILD
 Use the generate.py script in the protocol folder to regenerate this file
**/

import {z} from 'zod';

export const MIN_PRESENTER_MESSAGE_ID = 20;
export enum PresenterMessageId {
    RemoteButtonPressed = 20,
    BridgeButtonPressed = 21,
    RemoteHeartBeat = 22,
    RemoteRequestState = 23,
    PresenterSetState = 24,
    PresenterRemotePacket = 25,
}
const PresenterMessageIdZ = z.nativeEnum(PresenterMessageId);

export enum LittleStateNames {
    DarkState = 'dark',
    MultipleChoiceState = 'mc',
    EmojiState = 'emoji',
}
const LittleStateNamesZ = z.nativeEnum(LittleStateNames);

const PresenterMessageBaseZ = z.object({id: PresenterMessageIdZ});
export const ProtocolRemoteButtonPressedZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.RemoteButtonPressed), // RemoteButtonPressed 
    button_id: z.number().int().nonnegative().lt(4), // 8bit unsigned integer between 0 and 4 
});
export type ProtocolRemoteButtonPressed = z.infer<typeof ProtocolRemoteButtonPressedZ>;

export const ProtocolBridgeButtonPressedZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.BridgeButtonPressed), // BridgeButtonPressed 
    button_id: z.number().int().nonnegative().lt(4), // 8bit unsigned integer between 0 and 4 
});
export type ProtocolBridgeButtonPressed = z.infer<typeof ProtocolBridgeButtonPressedZ>;

export const ProtocolRemoteHeartBeatZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.RemoteHeartBeat), // RemoteHeartBeat 
    battery_level: z.number().int().nonnegative().lt(100), // 8bit unsigned integer between 0 and 100 
});
export type ProtocolRemoteHeartBeat = z.infer<typeof ProtocolRemoteHeartBeatZ>;

export const ProtocolRemoteRequestStateZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.RemoteRequestState), // RemoteRequestState 
    state_hash1: z.number().int().nonnegative().lte(0xFFFFFFFF), // 32bit unsigned integer 
    state_hash2: z.number().int().nonnegative().lte(0xFFFFFFFF), // 32bit unsigned integer 
    state_hash3: z.number().int().nonnegative().lte(0xFFFFFFFF), // 32bit unsigned integer 
    state_hash4: z.number().int().nonnegative().lte(0xFFFFFFFF), // 32bit unsigned integer 
});
export type ProtocolRemoteRequestState = z.infer<typeof ProtocolRemoteRequestStateZ>;

export const ProtocolPresenterSetStateDarkStateZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.PresenterSetState), // PresenterSetState 
    state_name: z.literal(LittleStateNames.DarkState), // "dark" 
});
export type ProtocolPresenterSetStateDarkState = z.infer<typeof ProtocolPresenterSetStateDarkStateZ>;

export const ProtocolPresenterSetStateMultipleChoiceStateZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.PresenterSetState), // PresenterSetState 
    state_name: z.literal(LittleStateNames.MultipleChoiceState), // "mc" 
    num_choices: z.number().int().nonnegative().lte(255), // 8bit unsigned integer 
    question_text: z.string().max(63), // 64 character (including null terminator) string 
});
export type ProtocolPresenterSetStateMultipleChoiceState = z.infer<typeof ProtocolPresenterSetStateMultipleChoiceStateZ>;

export const ProtocolPresenterSetStateEmojiStateZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.PresenterSetState), // PresenterSetState 
    state_name: z.literal(LittleStateNames.EmojiState), // "emoji" 
    emoji_choice_1: z.number().int().nonnegative().lte(65535), // 16bit unsigned integer 
    emoji_choice_2: z.number().int().nonnegative().lte(65535), // 16bit unsigned integer 
    emoji_choice_3: z.number().int().nonnegative().lte(65535), // 16bit unsigned integer 
    emoji_choice_4: z.number().int().nonnegative().lte(65535), // 16bit unsigned integer 
});
export type ProtocolPresenterSetStateEmojiState = z.infer<typeof ProtocolPresenterSetStateEmojiStateZ>;

export const ProtocolPresenterSetStateZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.PresenterSetState), // PresenterSetState 
    state_name: LittleStateNamesZ, // 20 character string that is a little state name 
});
export type ProtocolPresenterSetState = z.infer<typeof ProtocolPresenterSetStateZ>;

export const ProtocolPresenterRemotePacketZ = PresenterMessageBaseZ.extend({
    id: z.literal(PresenterMessageId.PresenterRemotePacket), // PresenterRemotePacket 
    from_mac: z.string().length(17), // A 6 byte mac address that is encoded in and out of a string for ease 
});
export type ProtocolPresenterRemotePacket = z.infer<typeof ProtocolPresenterRemotePacketZ>;

export type PresenterMessage = ProtocolBridgeButtonPressed | ProtocolPresenterRemotePacket | ProtocolPresenterSetState | ProtocolPresenterSetStateDarkState | ProtocolPresenterSetStateEmojiState | ProtocolPresenterSetStateMultipleChoiceState | ProtocolRemoteButtonPressed | ProtocolRemoteHeartBeat | ProtocolRemoteRequestState;

export function parsePresenterMessage(data: object, warn=false) {
    try {
        return ProtocolRemoteButtonPressedZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolBridgeButtonPressedZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolRemoteHeartBeatZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolRemoteRequestStateZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolPresenterSetStateDarkStateZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolPresenterSetStateMultipleChoiceStateZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolPresenterSetStateEmojiStateZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolPresenterSetStateZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

    try {
        return ProtocolPresenterRemotePacketZ.parse(data);
    } catch(e) { if (warn) console.warn(e)}

}
export const MessageCreators = {
  PresenterSetStateDarkState(
   wasm: any,
   to_mac: string
  ) {
    let func_name = 'create_PresenterSetStateDarkState';
    return wasm.ccall(func_name, 'string', ["string"], [to_mac]);
  },
  PresenterSetStateMultipleChoiceState(
   wasm: any,
   to_mac: string,
   num_choices: ProtocolPresenterSetStateMultipleChoiceState['num_choices'],
   question_text: ProtocolPresenterSetStateMultipleChoiceState['question_text']
  ) {
    let func_name = 'create_PresenterSetStateMultipleChoiceState';
    return wasm.ccall(func_name, 'string', ["string",'number','string'], [to_mac,num_choices,question_text]);
  },
  PresenterSetStateEmojiState(
   wasm: any,
   to_mac: string,
   emoji_choice_1: ProtocolPresenterSetStateEmojiState['emoji_choice_1'],
   emoji_choice_2: ProtocolPresenterSetStateEmojiState['emoji_choice_2'],
   emoji_choice_3: ProtocolPresenterSetStateEmojiState['emoji_choice_3'],
   emoji_choice_4: ProtocolPresenterSetStateEmojiState['emoji_choice_4']
  ) {
    let func_name = 'create_PresenterSetStateEmojiState';
    return wasm.ccall(func_name, 'string', ["string",'number','number','number','number'], [to_mac,emoji_choice_1,emoji_choice_2,emoji_choice_3,emoji_choice_4]);
  },
  PresenterSetState(
   wasm: any,
   to_mac: string,
   state_name: ProtocolPresenterSetState['state_name']
  ) {
    let func_name = 'create_PresenterSetState';
    return wasm.ccall(func_name, 'string', ["string",'string'], [to_mac,state_name]);
  },
};
