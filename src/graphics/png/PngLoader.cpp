#include "graphics/png/PngLoader.h"

PngLoader::PngLoader()
{
    //ctor
}

PngLoader::~PngLoader()
{
    //dtor
}


imp::PixelData* PngLoader::loadPixelDataFromFile(const char* filename)
{
       FILE *fp = fopen(filename, "rb");
    if (!fp)
       return (IMP_NULL);

    //check
    png_byte pngsig[8];
    fread(pngsig, 1, 8, fp);
    bool is_png = !png_sig_cmp(pngsig, 0, 8);

    if (!is_png)
       return (IMP_NULL);

    //loading
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png_ptr)
    {
        fprintf(stderr, "impError : Cannot load png image %s\n", filename);
        return IMP_NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "impError : Cannot load png image %s\n", filename);
        png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
        return IMP_NULL;
    }

    png_bytep* row_ptrs = IMP_NULL;
    char* data = IMP_NULL;

    if ( setjmp(png_jmpbuf(png_ptr)) )
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,(png_infopp)0);
        if (row_ptrs != NULL) delete [] row_ptrs;
        if (data != NULL) delete [] data;

        fprintf(stderr, "impError : Cannot load png %s\n", filename);
        return IMP_NULL;
    }

    /// set reading callback (readData)
    png_set_read_fn(png_ptr,(png_voidp)fp, PngLoader::readData);

    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);
    png_uint_32 imgWidth =  png_get_image_width(png_ptr, info_ptr);
    png_uint_32 imgHeight = png_get_image_height(png_ptr, info_ptr);
    //bits per CHANNEL! note: not per pixel!
    png_uint_32 bitdepth   = png_get_bit_depth(png_ptr, info_ptr);
    //Number of channels
    png_uint_32 channels   = png_get_channels(png_ptr, info_ptr);
    //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
    png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
            //Don't forget to update the channel info (thanks Tom!)
            //It's used later to know how big a buffer we need for the image
            channels = 3;
            break;
        case PNG_COLOR_TYPE_GRAY:
            if (bitdepth < 8)
            png_set_expand_gray_1_2_4_to_8(png_ptr);
            //And the bitdepth info
            bitdepth = 8;
            break;
        default:
            // nothing to do
            //fprintf(stdout, "impError : cannot load png image %s, color format not supported\n", filename);
        break;
    }

    /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        channels+=1;
    }

    //We don't support 16 bit precision.. so if the image Has 16 bits per channel
        //precision... round it down to 8.
    if (bitdepth == 16)
        png_set_strip_16(png_ptr);

    //// image read
    png_bytep rowPtrs[imgHeight];
    data = new char[imgWidth * imgHeight * bitdepth * channels / 8];
    const unsigned int stride = imgWidth * bitdepth * channels / 8;

    for (size_t i = 0; i < imgHeight; i++) {
        png_uint_32 q = (imgHeight- i - 1) * stride;
        rowPtrs[i] = (png_bytep)data + q;
    }
    png_read_image(png_ptr, rowPtrs);

    ////clean
    //delete[] (png_bytep)rowPtrs;
    png_destroy_read_struct(&png_ptr, &info_ptr,(png_infopp)0);

    imp::PixelData* image = new imp::PixelData();
    imp::PixelFormat format = imp::PixelFormat_RGBA8;
    if(channels == 3)
        format = imp::PixelFormat_RGB8;
    image->loadFromMemory(data, imgWidth, imgHeight, format);

    delete [] data;

    return image;
}

imp::Texture* PngLoader::loadFromFile(const char* filename)
{
    imp::PixelData* data = loadPixelDataFromFile(filename);

    if(data == IMP_NULL)
        fprintf(stderr, "impError : cannot load pixel data from file %s\n", filename);

    imp::Texture* tex = new imp::Texture();
    tex->loadFromPixelData(data);
    delete data;

    return tex;
}

pngimage_t PngLoader::loadFrom(const char* filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
       return (NULL);
    }

    //check
    png_byte pngsig[8];
    fread(pngsig, 1, 8, fp);
    bool is_png = !png_sig_cmp(pngsig, 0, 8);

    if (!is_png)
    {
       return (NULL);
    }

    //loading
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png_ptr)
    {
        fprintf(stderr, "err : load png\n");
        return NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "err load png\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
        return NULL;
    }

    png_bytep* row_ptrs = NULL;
    char* data = NULL;

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr,(png_infopp)0);
        if (row_ptrs != NULL) delete [] row_ptrs;
        if (data != NULL) delete [] data;

        fprintf(stderr, "err load png\n");
        return NULL;
    }

    png_set_read_fn(png_ptr,(png_voidp)fp, PngLoader::readData);

    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);
    png_uint_32 imgWidth =  png_get_image_width(png_ptr, info_ptr);
    png_uint_32 imgHeight = png_get_image_height(png_ptr, info_ptr);
    //bits per CHANNEL! note: not per pixel!
    png_uint_32 bitdepth   = png_get_bit_depth(png_ptr, info_ptr);
    //Number of channels
    png_uint_32 channels   = png_get_channels(png_ptr, info_ptr);
    //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
    png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

        switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
            //Don't forget to update the channel info (thanks Tom!)
            //It's used later to know how big a buffer we need for the image
            channels = 3;
            break;
        case PNG_COLOR_TYPE_GRAY:
            if (bitdepth < 8)
            png_set_expand_gray_1_2_4_to_8(png_ptr);
            //And the bitdepth info
            bitdepth = 8;
            break;
    }

    /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        channels+=1;
    }

    //We don't support 16 bit precision.. so if the image Has 16 bits per channel
        //precision... round it down to 8.
    if (bitdepth == 16)
        png_set_strip_16(png_ptr);

    //// image read
    png_bytep rowPtrs[imgHeight];
    data = new char[imgWidth * imgHeight * bitdepth * channels / 8];
    const unsigned int stride = imgWidth * bitdepth * channels / 8;

    for (size_t i = 0; i < imgHeight; i++) {
        png_uint_32 q = (imgHeight- i - 1) * stride;
        rowPtrs[i] = (png_bytep)data + q;
    }
    png_read_image(png_ptr, rowPtrs);

    ////clean
    //delete[] (png_bytep)rowPtrs;
    png_destroy_read_struct(&png_ptr, &info_ptr,(png_infopp)0);

    pngimage_t img = new priv_pngimage_t;
    img->bpp = bitdepth*channels;
    img->channels = channels;
    img->width = imgWidth;
    img->height = imgHeight;
    img->data = data;

    return img;
}


void PngLoader::readData(png_structp png_ptr, png_bytep png_data, png_size_t length)
{
    FILE* fp = (FILE*) png_get_io_ptr(png_ptr);
    fread(png_data, 1, length, fp);
}

GLuint PngLoader::png_texture_load(const char * file_name, int * width, int * height)
{
    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width){ *width = temp_width; }
    if (height){ *height = temp_height; }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data;
    image_data = (png_byte*) malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep * row_pointers = (png_bytep*) malloc(temp_height * sizeof(png_bytep));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; (unsigned int)i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);

    return texture;
}
