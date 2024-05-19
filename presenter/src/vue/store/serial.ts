import { defineStore } from 'pinia'
const serial = require("serialport");

const serialPortStore = {
    id: 'serial',
    state: () => ({
      setup: false,
      api: ((window as any).electronAPI as any),
    }),
    getters: {
        isReady: (state) => {return state.setup},
    },
    actions: {
        async getPorts() {
            const ports = await serial.SerialPort.list();
            return ports;
        },
        async listenTo(path:string) {
            // this.api.serialMessageCallback((x)=>{
            //     console.log(x);
            // })
            // await this.api.listenToPort(path);
            this.setup = true;
        },
        skipListen() {
            this.setup = true;
        },
        async reset() {
            // await this.api.serialReset()
            this.setup = false
        }
    }
}
export type SerialPortInfos = Awaited<ReturnType<typeof serialPortStore.actions.getPorts>>

export const useSerialPortStore = defineStore(serialPortStore)