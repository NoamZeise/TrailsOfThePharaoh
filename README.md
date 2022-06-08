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

### linux debian/ubuntu

#### STEP 1
make sure you have cmake, and ninja-build otherwise do
```
sudo apt install cmake ninja-build
```

#### STEP 2 (vulkan version):

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

#### STEP 2 (OpenGL version)
generate OpenGL glad header files from https://glad.dav1d.de/ 
select gl version 4.6, and click generate, download the glad.zip, unzip and place include folders in /usr/local/include/ (glad/ and KHR/)

Warning: if you pick a different OpenGL version, replace glad.c in the repo (under src/opengl-render/glad.c) with your generated glad.c

#### STEP 3 - install dependancies
```
sudo apt install libglfw3-dev libglm-dev libfreetype-dev libsndfile1-dev libasound-dev portaudio19-dev
```

#### STEP 4 - build with buildscripts
from project root -> replace debugVulkan.sh, with (releaseVulkan.sh, debugOpenGL.sh, releaseOpenGL.sh) for whatever build you want
```
mkdir build
chmod +x resources/buildscripts/linux/debugVulkan.sh
./resources/buildscrips/linux/debugVulkan.sh
```
the game should open automatically after it has build, otherwise errors will show what the problem is 
