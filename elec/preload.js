const { contextBridge } = require('electron')
const { Player } = require("D:\\node-addon\\build\\Release\\addon");

window.addEventListener('DOMContentLoaded', () => {
  console.log(JSON.stringify(process.env));
  var can = document.getElementsByTagName("canvas")[0];


  const player = new Player();
  let width = 1920;
  let height = 1080;

  can.width = `${width}`;
  can.height = `${height}`

  can.style.width = `${width}px`;
  can.style.height = `${height}px`;

  player.setVideo("D:\\00021.MTS");
  player.setDimensions(width, height);
  player.setRenderCallback(result => {
    try {
      var data = new ImageData(result, width, height);
      const ctx = can.getContext('2d');
      ctx.putImageData(data, 0, 0);
    }
    catch (e) {
      console.log(result);
    }
  });

  player.play();

})

