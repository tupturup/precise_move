'use strict';
const electron = require('electron');
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

let mainWindow = null;
let subpy;
let apiUrl = 'http://localhost:5000';

app.on('window-all-closed', function() {
    subpy.kill('SIGINT');
    app.quit();
});

app.on('quit', function() {
    subpy.kill('SIGINT');
});

app.on('ready', function() {
    subpy = require('child_process').spawn('python', ['./main.py'], {detached: true});

    var openWindow = function() {
        mainWindow = new BrowserWindow({width: 800, height: 600});
        mainWindow.loadURL('http://localhost:5000');
        //mainWindow.loadURL('file://' + __dirname + '/index.html');
        mainWindow.webContents.openDevTools();
        mainWindow.on('closed', function() {
            mainWindow = null;
            subpy.kill('SIGINT');
        });
    };

    var startUp = function() {
        require('request-promise')(apiUrl).then(function() {
            openWindow();
        }).catch(function(err) {
            startUp();
        });
    };
    openWindow();
    //startUp();
});
