const { contextBridge } = require('electron')
const player = require("D:\\node-addon\\build\\Release\\addon");

window.addEventListener('DOMContentLoaded', () => {
  var can = document.getElementsByTagName("canvas")[0];

  player.play(result => {
try {
    var data = new ImageData(result, 1920, 1080);
    const ctx = can.getContext('2d');
    ctx.putImageData(data, 0, 0);
}
catch (e) {
  alert(e);
}
   
  });
})

