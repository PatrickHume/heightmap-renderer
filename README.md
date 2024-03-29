# heightmap-renderer

### Roughness-Based Heightmap Tessellation (OpenGL)

<img src="https://github.com/PatrickHume/heightmap-renderer/blob/main/coverImage.png?raw=true" width="480">

Reducing a heightmap mesh's poly-count by pre-calculating per-patch tessellation levels.

Based on the excellent LearnOpenGL guest articles Dr. Jeffrey Paone. <br>
https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map <br>
https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation

Read about it [here.](https://patrickhume.vercel.app/heightmap-tessellation)

#### Running the program
Included is the tasks.json file used in development. <br>
Ideally there would be a makefile instead, this is yet to be written.

Once you have it compiled, try the following command:<br>
**./app alps**

Or load your own heightmaps with Tangrams Heightmapper. <br>
https://tangrams.github.io/heightmapper/

#### Controls
* Use the **mouse** to look around.
* Use **WASD** keys to move.
* Use **shift** and **~** to move up and down.
* Hold **shift** to move faster.
* Press **P** to toggle wireframe view.
* Press **0** to toggle fixed-tessellation view.
