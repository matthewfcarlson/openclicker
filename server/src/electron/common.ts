import type {PortInfo} from "@serialport/bindings-interface";

export type ElectronAPI = {
    listSerialPorts: () => Promise<PortInfo[]>,
    openFile(): Promise<String|undefined>,
    listenToPort(path:String) : Promise<void>,
    serialStartMesh(path:String): Promise<boolean>,
    serialReset() : Promise<void>,
    serialMessageCallback (callback: (value: string) => void): void
}

export const IPCNames = {
    // These serial commands need to go away and the mesh is handled in the app process
    serial: {
        message: "serial:message",
        listenTo: "serial:listenTo",
        meshStart: "serial:meshStart",
        list: "serial:list",
        reset: "serial:reset",
    },
    mesh: {
        listen: "mesh:join",
        send: "mesh:send",
        leave: "mesh:leave",
    }
}