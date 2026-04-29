# Q2SpriteJSONTool
Convert between Quake ]['s SP2 sprites and valid JSON files.

Drag and drop a JSON file to create a SP2 file, and vice versa. If using the command line, you can just pass the path of the file you want to convert as an argument.

# Examples
![daikatana](https://i.imgur.com/aYdOk7Y.png)
[Quake 2 example](https://i.imgur.com/GTtwFmv.mp4)  
[Heretic 2 example](https://i.imgur.com/KqaRBNA.mp4)

# JSON template
```
{
  "ident": "IDS2",
  "version": 2,
  "numframes": 2,
  "frames": [
    {
      "width": 64,
      "height": 64,
      "origin_x": 32,
      "origin_y": 32,
      "name": "sprites/spriteframe1.pcx"
    },
    {
      "width": 64,
      "height": 64,
      "origin_x": 32,
      "origin_y": 32,
      "name": "sprites/spriteframe2.pcx"
    }
  ]
}
```
# Compile
If using MSVS, open the SLN. If on Linux, you can use the included shell script. Otherwise:
```
g++ Q2SpriteJSONTool.cpp -o Q2SpriteJSONTool_linux -I.
```
# Copyright
This program was made by Jakub Majewski.
JSON parsing library was made by Niels Lohmann.
