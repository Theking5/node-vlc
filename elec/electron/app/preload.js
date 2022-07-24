const { contextBridge } = require('electron');
const { read } = require('original-fs');
const { Player } = require("D:\\node-addon\\build\\Release\\addon");
var player = new Player();
//player.setDimensions(1920, 1080);

var lastCalledTime;
var fps;
var ready = true;
function requestAnimFrame() {

  if (!lastCalledTime) {
    lastCalledTime = performance.now();
    fps = 0;
    return;
  }
  delta = (performance.now() - lastCalledTime) / 1000;
  lastCalledTime = performance.now();
  fps = 1 / delta;

  document.querySelector("#fps").innerHTML = Math.floor(fps);
}

var drag = false;
var x = 0;
var lastxd = 0;
window.addEventListener("mousedown", (e) => {
  drag = true;
  x = e.clientX;
})

window.addEventListener("mousemove", (e) => {
  if(drag ) {

    var xd = (e.pageX / window.innerWidth);
    var t = 43107 * xd;

    if(t<0)
    t = 0
    console.log(xd)
    player.setPosition(t);

}
});

window.addEventListener("mouseup", () => {
  drag = false;
})
window.addEventListener("DOMContentLoaded", () => {
  document.querySelector("#btnPause").onclick = () => {
    player.pause();
  }

  document.querySelector("#btnSeek").onclick = () => {
    player.setPosition(Number(document.querySelector("#text").value));
  }

  document.querySelector("#btnPlay").onclick = () => {
    player.play();
  }
  
  document.querySelector("#btnLoad").onclick = () => {
    player.setVideo(document.querySelector("#text").value);
  }
  
  let width, height;
  var can = document.getElementsByTagName("canvas")[0];
  const ctx = can.getContext('2d');

  player.onSize((w, h) => {
    width = w;
    height = h;

    can.width = `${width}`;
    can.height = `${height}`
  });

  player.onRender(result => {

    try {

      var data = new ImageData(result, width, height);


      ctx.fillRect(0, 0, width, height);
      ctx.putImageData(data, 0, 0);
      requestAnimFrame();
    }
    catch (e) {

      console.log(e);
      return;
    }
  });



});
