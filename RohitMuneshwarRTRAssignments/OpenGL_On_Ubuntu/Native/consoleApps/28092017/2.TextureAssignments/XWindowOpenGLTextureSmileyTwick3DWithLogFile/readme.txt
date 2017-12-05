 First download the SOIL.h header file from its website http://www.lonesock.net/soil.html

Place the header file in your project directory and include it in your project file.

#include "SOIL.h" 

After that you have to install the soil library to use -lSOIL. To install the library use the command

sudo apt-get install libsoil-dev
 


 gcc XWindowOpenGLTrianglePersp3DRotation.cpp -lX11 -lSOIL -lGL -lGLU -L/usr/lib64 -lstdc++
