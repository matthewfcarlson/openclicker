// All of the Node.js APIs are available in the preload process.
// It has the same sandbox as a Chrome extension.
import { contextBridge, ipcRenderer } from 'electron'
import { type ElectronAPI } from '../common'
window.addEventListener('DOMContentLoaded', () => {
  const replaceText = (selector: any, text: any) => {
    const element = document.getElementById(selector)
    if (element) element.innerText = text
  }

  for (const dependency of ['chrome', 'node', 'electron']) {
    replaceText(`${dependency}-version`, process.versions[dependency])
  }
})

function handleSerialMessageCallback (callback: (value:string)=>void) {
  ipcRenderer.on("serial:message", (_event, value) => callback(value))
}

const electronAPI : ElectronAPI = {
  openFile: () => ipcRenderer.invoke('dialog:openFile'),
  listSerialPorts: () => ipcRenderer.invoke('serial:list'),
  listenToPort: (path: string) => ipcRenderer.invoke("serial:listenTo", path),
  serialMessageCallback: handleSerialMessageCallback,
  serialStartMesh: function (path: String): Promise<boolean> {
    throw new Error('Function not implemented.')
  },
  serialReset: () => ipcRenderer.invoke("serial:reset"),
}

contextBridge.exposeInMainWorld('electronAPI', electronAPI)