import { defineStore } from 'pinia'
import type {ElectronAPI} from '../electron/common'

const serialPortStore = {
    id: 'serial',
    state: () => ({
      setup: false,
      api: ((window as any).electronAPI as ElectronAPI),
    }),
    getters: {
        isReady: (state) => {return state.setup},
    },
    actions: {
        async getPorts() {
            // TODO: check if we recently requested the ports list
            const ports = await this.api.listSerialPorts();
            console.log(ports);
            return ports;
        },
        async listenTo(path:string) {
            this.api.serialMessageCallback((x)=>{
                console.log(x);
            })
            await this.api.listenToPort(path);
            this.setup = true;
        },
        skipListen() {
            this.setup = true;
        },
        async reset() {
            await this.api.serialReset()
            this.setup = false
        }
    }
}
export type SerialPortInfos = Awaited<ReturnType<typeof serialPortStore.actions.getPorts>>

export const useSerialPortStore = defineStore(serialPortStore)