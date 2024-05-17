import { join } from 'path';
import {
    app,
    BrowserWindow,
    ipcMain,
    dialog,
    ipcRenderer
} from 'electron';
import { SerialPort } from 'serialport';
import { IPCNames } from '../common';
import { ReadlineParser } from '@serialport/parser-readline';
import fs from "fs";

const isDev = process.env.npm_lifecycle_event === "app:dev" ? true : false;

async function handleFileOpen() {
    const { canceled, filePaths } = await dialog.showOpenDialog({ title: "Open File" })
    if (canceled) {
        return "";
    }
    const data = await fs.promises.readFile(filePaths[0], {encoding:'utf-8'});
    return data;
}
async function handleSerialListenTo(_event: Electron.IpcMainInvokeEvent, path:string) {
    console.log(path)
    const port = new SerialPort({path, baudRate:115200})
    const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))
    port.on('error', function(err) {
        console.error("Failed to open port")
    })
    parser.on('data', function(data:string){
        console.log(data);
        // Send this message to all windows
        BrowserWindow.getAllWindows().forEach((x)=>x.webContents.send(IPCNames.serial.message, data));
    })
}

function createWindow() {
    // Create the browser window.
    const mainWindow = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: join(__dirname, '../preload/preload.js'),
        },
    });

    // and load the index.html of the app.
    if (isDev) {
        mainWindow.loadURL('http://localhost:5173');// Open the DevTools.
        mainWindow.webContents.openDevTools();
    } else {
        mainWindow.loadFile(join(__dirname, '../../index.html'));
    }
    // mainWindow.loadURL( //this doesn't work on macOS in build and preview mode
    //     isDev ?
    //     'http://localhost:3000' :
    //     join(__dirname, '../../index.html')
    // );
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
    ipcMain.handle('dialog:openFile', handleFileOpen)
    ipcMain.handle(IPCNames.serial.list, SerialPort.list)
    ipcMain.handle(IPCNames.serial.listenTo, handleSerialListenTo)
    createWindow()
    app.on('activate', function () {
        // On macOS it's common to re-create a window in the app when the
        // dock icon is clicked and there are no other windows open.
        if (BrowserWindow.getAllWindows().length === 0) createWindow()
    })
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});