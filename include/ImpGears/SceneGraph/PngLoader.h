#ifndef PNGLOADER_H
#define PNGLOADER_H

#include <GL/glew.h>
#include <GL/gl.h>
// #include "Graphics/png.h"
#include <cstdio>
#include <cstdlib>
#include "Graphics/Texture.h"
#include "Graphics/ImageData.h"


typedef struct IMP_API
{
    int width;
    int height;
    int bpp;
    int channels;
    char* data;
} priv_pngimage_t;

typedef priv_pngimage_t* pngimage_t;


/// Source : http://www.piko3d.net/tutorials/libpng-tutorial-loading-png-files-from-streams/
class IMP_API PngLoader
{
    public:
        PngLoader();
        virtual ~PngLoader();

    //static void readData(png_structp png_ptr, png_bytep png_data, png_size_t length);

        static pngimage_t loadFrom(const char* filename);
        static imp::ImageData* loadImageDataFromFile(const char* filename);
        static imp::Texture* loadFromFile(const char* filename);

        /// Source : http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures#A_simple_libpng_example
        //static GLuint png_texture_load(const char * file_name, int * width, int * height);
    protected:
    private:
};

#endif // PNGLOADER_H
