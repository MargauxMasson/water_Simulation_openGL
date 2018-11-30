# water_simulation:		water_simulation.cpp
# 		g++  -o  water_simulation    water_simulation.cpp    -lm  -framework  OpenGL  -framework  GLUT 
water_simulation:		water_simulation.cpp
		g++ -o water_simulation  water_simulation.cpp  -lGL  -lGLU  libglut.so  -lm  -ljpeg 
