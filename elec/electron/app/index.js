const { app, BrowserWindow, crashReporter} = require('electron');
const path = require('path')

function createWindow () {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
 //   autoHideMenuBar: true,
    webPreferences: {
      contextIsolation: false,
      preload: path.join(__dirname, 'preload.js')
    }
  })

  win.maximize();

  //win.webContents.openDevTools();
  win.loadURL('http://localhost:5000/index.html?adadad');
}

app.whenReady().then(() => {
   createWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow()
    }
  })


})

crashReporter.start({
  submitURL: "https://example.com",
  uploadToServer: false,
})
console.log(`Crash dumps directoryssss: ${app.getPath('crashDumps')}`)


app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})