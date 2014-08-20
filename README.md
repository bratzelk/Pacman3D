Pacman 3D
=========


To run the game simply build the project by running ```make``` and then run ```./Pacman``` from the command line.
Optionally to load a level run ```./Pacman ./levels/levelX.txt``` where x is the level you would like to load.

I haven't run this recently on a non OS X machine, you may need to remove the OSX flag in the Makefile to compile.

In game commands
--

 - To move, use the arrow keys.
 - Pressing down once will stop the player anywhere on the grid.
 - Pressing down a second time will reverse the player (180 degrees),you could also press up to continue moving forward.

When you die (collide with a ghost) you are reset back to the centre of the map.
You must then press the up arrow to begin moving again.

 - 'r' - Will refresh the landscape, generating a new height map based on a plasma fractal.
 - ‘f' - Toggles the FPS (in the terminal)
 - ‘p' - pauses the game
 - ‘c' - Changes the camera to classic mode (Birds eye view of landscape)
 - ‘m' - Will flatten the landscape
 - ‘w' - Play the game in cool wireframe mode
 - ‘q' - will quit the game 

Features
--

 - Four connected (grid) movement based on navigation graph
 - Smooth camera transitions (when turning the player) 
 - Ghosts will move slower up hills
 - Ghosts will chase the player when within a certain distance
 - Ghosts are drawn using a spline
 - Ghosts and pills are drawn with less quantisation as their distance from the camera increases
 - Edible items (Teapot instead of fruit,normal pills, power pills)
 - Players can hunt down a ghost after eating a power pill
 - Score and lives are displayed on screen
 - When all normal pills are gone the level is reset
 - PacLand Arena Banner displayed in 3D
 - Classic Retro Mode
 - Python Level Generator and Level Loader

Modelling Game Objects
--

 - Pacman is displayed using either a spline or a solid sphere
 - Ghosts are modelled using a bernstein polynomial spline (quantisation of the spline is dependent on the distance from the camera)
 - Pills are displayed using a sphere (the number of hemispheres is distance dependent)
 - The landscape uses the plasma fractal from project one
 - The edible item is displayed using the built in teapot object

Camera Motion
--

 - The camera is positioned behind the player
 - The height and angle of the camera is relative to the players position on the landscape and their direction of motion (the heading)
 - When the player turns, the camera is smoothly rotated towards the new direction

