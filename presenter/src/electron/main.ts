import { app, BrowserWindow, dialog, ipcMain, IpcMainInvokeEvent, Menu } from 'electron';
import path from 'path';
import fs from "fs";

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) {
  app.quit();
}

app.setName("OpenClicker Presenter")

async function handleFileOpen() {
  const { canceled, filePaths } = await dialog.showOpenDialog({ title: "Open File" })
  if (canceled) {
    return "";
  }
  const data = await fs.promises.readFile(filePaths[0], { encoding: 'utf-8' });
  return data;
}

async function handleMessageToPresenter(event: IpcMainInvokeEvent, message: string) {
  await broadcastEventToAllWindows(event.sender.id, 'message-to-presenter', message);
}

async function handleMessageFromPresenter(event: IpcMainInvokeEvent, message: string) {
  await broadcastEventToAllWindows(event.sender.id, 'message-from-presenter', message);
}

async function broadcastEventToAllWindows(senderId: number, eventName:string, message:string){
  BrowserWindow.getAllWindows().forEach((x)=>{
    console.log(`From ${senderId}->${x.id} ${message}`);
    if (x.id == senderId) return;
    x.webContents.send(eventName, message);
  })
}

const createMainWindow = () => {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    backgroundColor: "#ccc",

    webPreferences: {
      //preload: path.join(__dirname, 'preload.js'),
      contextIsolation: false, // allow use with Electron 12+
      nodeIntegration: true, // to allow require
      nodeIntegrationInWorker: true,
      allowRunningInsecureContent: false,
    },
  });


  // and load the index.html of the app.
  if (MAIN_WINDOW_VITE_DEV_SERVER_URL) {
    mainWindow.loadURL(MAIN_WINDOW_VITE_DEV_SERVER_URL);
    // Open the DevTools.
  } else {
    mainWindow.loadFile(path.join(__dirname, `../electron/renderer/${MAIN_WINDOW_VITE_NAME}/index.html`));
  }
  mainWindow.webContents.openDevTools();

};

const createRemoteWindow = () => {
  if (MAIN_WINDOW_VITE_DEV_SERVER_URL) {
    const remoteWindow = new BrowserWindow({
      width: 350,
      height: 230,
      webPreferences: {
        preload: path.join(__dirname, './remote_preload.js'),
      }
    })
    remoteWindow.loadURL(`${MAIN_WINDOW_VITE_DEV_SERVER_URL}/remote.html`);
    // Open the DevTools.
    remoteWindow.webContents.openDevTools({
      mode:'detach',
      title: "OpenClicker Remote Console"
    });
    remoteWindow.setTitle("OpenClicker Remote")
  }
}

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
// Except for us, we are going to close anyway
app.on('window-all-closed', () => {
  app.quit();
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createMainWindow();
  }
});

app.whenReady().then(() => {
  ipcMain.handle('dialog:openFile', handleFileOpen)
  ipcMain.handle('message-from-presenter', handleMessageFromPresenter)
  ipcMain.handle('message-to-presenter', handleMessageToPresenter)
  createMainWindow();
  createRemoteWindow()
});

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and import them here.
const isMac = process.platform === 'darwin'
const isDev = (MAIN_WINDOW_VITE_DEV_SERVER_URL != null);
const edit = {
  label: "Edit",
  submenu: [
    {
      label: "Undo",
      accelerator: "CmdOrCtrl+Z",
      selector: "undo:"
    },
    {
      label: "Redo",
      accelerator: "Shift+CmdOrCtrl+Z",
      selector: "redo:"
    },
    {
      type: "separator"
    },
    {
      label: "Cut",
      accelerator: "CmdOrCtrl+X",
      selector: "cut:"
    },
    {
      label: "Copy",
      accelerator: "CmdOrCtrl+C",
      selector: "copy:"
    },
    {
      label: "Paste",
      accelerator: "CmdOrCtrl+V",
      selector: "paste:"
    },
    {
      label: "Select All",
      accelerator: "CmdOrCtrl+A",
      selector: "selectAll:"
    }
  ]
}

const template = [
  // { role: 'appMenu' }
  ...(isMac
    ? [{
      label: app.name,
      submenu: [
        { role: 'about' },
        { type: 'separator' },
        { role: 'services' },
        { type: 'separator' },
        { role: 'hide' },
        { role: 'hideOthers' },
        { role: 'unhide' },
        { type: 'separator' },
        { role: 'quit' }
      ]
    }]
    : []),
  // { role: 'fileMenu' },
  edit,
  // { role: 'windowMenu' }
  {
    label: 'Window',
    submenu: [
      ...(isDev ? [{
        label: 'New Remote Window',
        click: async () => {
          createRemoteWindow()
        },
        accelerator: process.platform === 'darwin' ? 'Shift+Cmd+R' : 'Ctrl+Shift+R',
      }]: []),
      { role: 'minimize' },
      { role: 'zoom' },
      { role: 'close' },
      { role: 'reload' },
      ...(isMac
        ? [
          { type: 'separator' },
          { role: 'front' },
          { type: 'separator' },
          { role: 'window' }
        ]
        : [
        ]),
    ]
  },
]

const menu = Menu.buildFromTemplate(template)
Menu.setApplicationMenu(menu)