// All of the Node.js APIs are available in the preload process.
// It has the same sandbox as a Chrome extension.
import { contextBridge, ipcRenderer } from 'electron'
import type { PortInfo } from '@serialport/bindings-interface';

const electronAPI ={
    openFile: () => ipcRenderer.invoke('dialog:openFile') as Promise<string|null>,
    serialPortsList: () => ipcRenderer.invoke('serial:listPorts') as Promise<PortInfo[]>,
    serialPortOpen: (path: string) => ipcRenderer.invoke('serial:openPort', path) as Promise<boolean>,
    serialPortClose: () => ipcRenderer.invoke('serial:closePort') as Promise<boolean>,
    serialPortListen: (callback: (value:string)=>void) => {
        ipcRenderer.on("serial:message", (_event, value) => callback(value));
        return;
    },
    serialPortSend: (message:string) => ipcRenderer.invoke("serial:message", message) as Promise<boolean>,
    presenterListen: (callback: (value:string)=>void) => {
        ipcRenderer.on("message-to-presenter", (_event, value) => callback(value));
        return;
    },
    presenterSend: (message:string) => ipcRenderer.invoke("message-from-presenter", message) as Promise<boolean>,
}

export type electronAPIType = typeof electronAPI;

contextBridge.exposeInMainWorld('electronAPI', electronAPI)