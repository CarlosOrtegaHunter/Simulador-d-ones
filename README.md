Includes a basic rendering engine with OpenGL and a Doppler Wave simulator on a 2D grid. 

This is the function approach version of an old project of mine, which does not work on the principle of locality but computes values as an 'action at a distance' and therefore cannot simulate 'immovable' surfaces. 
On the other hand, the Doppler calculation is interesting and ingenious, and works by mathematical triangulation (`fieldShader.vert`). 

I also implemented an early version of the simulation acting as an automaton cellular, which I have not made available on GitHub yet (which worked implementing Hooke's law and setting solid surfaces to be immovable). 

<img width="2000" height="1125" alt="image" src="https://github.com/user-attachments/assets/7cccbc99-b8d5-4413-a2cf-0e9a2ba79fb0" />
