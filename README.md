# Trials of The Pharaoh
[itch page](https://noamzeise.itch.io/pharaoh-trials)

A game made in 10 days for [GDTV Jam 2022](https://itch.io/jam/gamedevtv-jam-2022)

Made using my [Vulkan](https://github.com/NoamZeise/Vulkan-Environment) and [OpenGL](https://github.com/NoamZeise/OpenGL-Environment) Environments, [Tiled Map Parser](https://github.com/NoamZeise/TiledMapParser), and [Audio](https://github.com/NoamZeise/audio) libs.


## Team:

Mick Cooke - MakeFire Music   -> Music

 https://youtube.com/channel/UCs75GjfGdtTS-CekMJOGICA

Thanos Gramosis  -> Art    https://www.artstation.com/tha-com-nos

Paul James - Wafer Audio -> Sound

Paulina Ramirez -  Lady Yami #3939  -> Voice Over/Writing 

https://lady-yami.itch.io/

Noam Zeise -> Programming  https://noamzeise.wordpress.com/


## building from source

### linux debian

make sure you have cmake, and ninja-build otherwise do
```
sudo apt install cmake ninja-build
```

#### for vulkan:

```
sudo apt install vulkan-tools
```
```
sudo apt install libvulkan-dev vulkan-validationlayers-dev spirv-tools
```
check vulkan is working with cube demo:
```
vkcube
```

#### install required libraries (both vulkan and ogl)
```
sudo apt install libglfw3-dev libglm-dev libfreetype-dev libsndfile1-dev libasound-dev portaudio19-dev
```

#### use build scrips
from project root -> build debug vulkan
```
mkdir build
chmod +x resources/buildscripts/linux/debugVulkan.sh
./resources/buildscrips/linux/debugVulkan.sh
```
the game should open automatically after it has build, otherwise errors will show what the problem is 
