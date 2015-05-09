# ImpGears

This is a project of C++ 3D Engine using OpenGL. A simple voxel system is available within.

Interesting points :
- We can use a custom scene camera : Free fly camera and Strategic camera are already available.
- Adapter can be developped for adding new window system like FreeGlut, GLFW, SDL, etc. A SFML adapter is already available.
- Render target can be used for Deferred rendering.
- VBO Manager ensures the correct using of the GPU memory.
- Texture manager is able to get textures already loaded.
- Mesh models can be loaded (with OBJ files)
- Random terrain can be generated with Perlin noise technic (2D example [here](https://www.dropbox.com/s/wchzmdgojrvp1mz/infinite-perlin.avi?dl=0)).
- Using of a Crysis inspired technic of SSAO (Screen Space Ambient Occlusion)
- Dynamic shadows
- Blinn-Phong lighting
- Optimized bloom effect on sky and self-illumination

Further work :
- Add a material system.
- Add an optimised système of Skybox/Skydome
- Work with advanced particles system
- Work with on a dynamic water motion simulation
- Work with more advanced lighting

Sorry for the lack of comments. There are still some major code changes that could be made, the comments will come after ;)

Some screenshots of the result with a voxel terrain and dynamic shadow, SSAO, bloom and light system :
![screenshot](http://uprapide.com/images/Lut1n/impgears-5-2015.png "Screenshot of the result")
![screenshot](http://uprapide.com/images/Lut1n/impgears-2-2015-b.png "Screenshot of the result")
![screenshot](http://uprapide.com/images/Lut1n/impgears-2-2015-a.png "Screenshot of the result")

You can also find some short videos demonstration [here](https://www.dropbox.com/s/46u9sc8ovv2soy0/3d-engine.avi?dl=0) and [here](https://www.dropbox.com/s/7l09s7f07hehs3l/voxel-video.mp4?dl=0).
