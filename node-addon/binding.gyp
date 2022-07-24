{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "Main.cpp", "Player.cpp", "Player.h" ],
      "include_dirs": [
          "<!(node -p \"require('node-addon-api').include_dir\")",
          "C:\\Users\\David\\Desktop\\vlc-3.0.17.4-win64\\vlc-3.0.17.4\\sdk\\include"
       ],
      "libraries": [ 
        "C:\\Users\\David\\Desktop\\vlc-3.0.17.4-win64\\vlc-3.0.17.4\\sdk\\lib\\libvlc.lib",
        "C:\\Users\\David\\Desktop\\vlc-3.0.17.4-win64\\vlc-3.0.17.4\\sdk\\lib\\libvlccore.lib"
      ]
    }
  ]
}