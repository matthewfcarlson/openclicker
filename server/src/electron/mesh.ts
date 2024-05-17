// TODO: move these to Zod

type TIME_DELAY = 3;
type TIME_SYNC = 4;
type NODE_SYNC_REQUEST = 5;
type NODE_SYNC_REPLY = 6;
type MSG_BROADCAST = 8;
type MSG_SINGLE = 9;

export interface MeshMessage {
    dest: number
    from: number
    type: TIME_DELAY | TIME_SYNC | NODE_SYNC_REQUEST | NODE_SYNC_REPLY | MSG_SINGLE | MSG_BROADCAST
}

export interface MeshMessageTimeSync extends MeshMessage {
    type: TIME_SYNC
    msg: {
        type: 0
    }
}

export interface MeshMessageTimeAdoptSync extends MeshMessage {
    type: TIME_SYNC
    msg: {
        type: 1
        t0: number
    }
}

export interface MeshMessageTimeAdoptResponseSync extends MeshMessage {
    type: TIME_SYNC
    msg: {
        type: 2
        t0: number
        t1: number
        t2: number
    }
}

type RoutingSubs = {
    nodeId: number,
    root?: boolean,
    subs: RoutingSubs[]
}

export  interface MeshMessageRouting extends MeshMessage {
    type: NODE_SYNC_REQUEST|NODE_SYNC_REPLY,
    msg: {
        nodeId: number,
        root?: boolean,
        subs: RoutingSubs[]
    }
}

export interface MeshMessageBroadcast extends MeshMessage {
    type: MSG_BROADCAST,
    msg: string
}

export interface MeshMessageSingle extends MeshMessage {
    type: MSG_SINGLE,
    msg: string
}

// Singleton mesh router that directs messages to the right place
// It works like a node