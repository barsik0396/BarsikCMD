const { app, BrowserWindow } = require('electron');

function createWindow() {
  const win = new BrowserWindow({
    width: 900,
    height: 600,
    backgroundColor: '#0a0a0a', // Чтобы не было белой вспышки при запуске
    autoHideMenuBar: true,      // СКРЫТЬ МЕНЮ (Alt всё равно его вызовет)
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false
    }
  });

  win.loadFile('index.html');
}

app.whenReady().then(createWindow);