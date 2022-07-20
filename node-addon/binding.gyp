{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "hello.cpp" ],
      "include_dirs": [
          "<!(node -p \"require('node-addon-api').include_dir\")",
          "C:\\Users\\David\\Desktop\\vlc-3.0.9.2-win64\\vlc-3.0.9.2\\sdk\\include"
       ],
      "libraries": [ 
        "C:\\Users\\David\\Desktop\\vlc-3.0.9.2-win64\\vlc-3.0.9.2\\sdk\\lib\\libvlc.lib",
        "C:\\Users\\David\\Desktop\\vlc-3.0.9.2-win64\\vlc-3.0.9.2\\sdk\\lib\\libvlccore.lib"
      ]
    }
  ]
}