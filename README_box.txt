Ligne de compilation : 

Simulation et test des fonctions en 2D : 
g++ test2D.cpp Boite.cpp Particule.cpp -o test2D -lsfml-graphics -lsfml-window -lsfml-system

Simulation et test des fonctions en 3D : 
g++ test3D.cpp Boite.cpp Particule.cpp -o test3D -lraylib -lGL -lm -lpthread -ldl -lrt -lX11