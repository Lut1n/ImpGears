# ImpGears

This is a project of C++ 3D Engine using OpenGL. It supports voxel map.

Interesting points :
- We can use a custom scene camera : Free fly camera and Strategic camera are already available.
- Adapter can be developped for adding new window system like FreeGlut, GLFW, SDL, etc. A SFML adapter is already available.
- Render target can be used for Deferred rendering.
- VBO Manager ensures the correct using of the GPU memory.
- Texture manager is able to get textures already loaded.
- Random terrain can be generated with Perlin noise technic (2D example [here](https://www.dropbox.com/s/wchzmdgojrvp1mz/infinite-perlin.avi?dl=0)). 

Further work :
- Continue work on MeshModel loading and rendering.
- Add a material system.
- Add an optimised système of Skybox/Skydome
- Work on dynamic shadow.
- Work on a SSAO(Screen Space Ambient Occlusion) technic.
- Work with more advanced lighting (my own test projet [here](https://www.dropbox.com/s/0q6zmvcgw8a06o2/normal-mapping.avi?dl=0).)

Sorry for the lack of comments. There are still some major code changes that could be made, the comments will come after ;)

A screenshot of the result with a voxel terrain and dynamic shadow :
![screenshot](http://uprapide.com/images/Lut1n/impgears22012015.png "Screenshot of the result")

You can also find some short videos demonstration [here](https://www.dropbox.com/s/46u9sc8ovv2soy0/3d-engine.avi?dl=0) and [here](https://www.dropbox.com/s/7l09s7f07hehs3l/voxel-video.mp4?dl=0).
