compile :
cl.exe /c /EHsc abc.cpp

link:
link.exe abc.obj user32.lib gdi32.lib

Compile and Link at one go:
cl.exe /EHsc abc.cpp /link user32.lib gdi32.lib ab.res

GLUT-
compile:
cl.exe /c /EHsc /I C:\freeglut\include ConcentricTrianglesStructure.cpp

link:
link.exe ogl.obj /LIBPATH:C:\freeglut\lib freeglut.lib /SUBSYSTEM:CONSOLE


