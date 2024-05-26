import { defineStore } from 'pinia'
const serial = require("serialport"); // eslint-disable-line

const serialPortStore = {
    id: 'serial',
    state: () => ({
      setup: false,
    }),
    getters: {
        isReady: (state) => {return state.setup == true},
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
            console.log(path);
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