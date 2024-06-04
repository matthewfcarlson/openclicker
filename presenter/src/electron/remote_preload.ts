// All of the Node.js APIs are available in the preload process.
// It has the same sandbox as a Chrome extension.
import { contextBridge, ipcRenderer } from 'electron'

contextBridge.exposeInMainWorld('electronAPI', {
    sendMessageToPresenter: (msg: string) => ipcRenderer.invoke("message-to-presenter", msg),
    openFile: () => ipcRenderer.invoke('dialog:openFile'),
    listenToPresenterMessage: (callback: (value:string)=>void) => ipcRenderer.on("message-from-presenter", (_event, value) => callback(value))
})