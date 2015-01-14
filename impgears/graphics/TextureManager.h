#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "../base/impBase.hpp"
#include <map>
#include "Texture.h"

IMPGEARS_BEGIN

typedef std::map<std::string, Texture*> TextureMap;

class TextureManager
{
    public:
        TextureManager();
        virtual ~TextureManager();

        static Texture* getTexture(std::string name);
        static void addTexture(std::string name, Texture* texture);
    protected:
    private:
        static TextureMap textures;
};

IMPGEARS_END

#endif // TEXTUREMANAGER_H
