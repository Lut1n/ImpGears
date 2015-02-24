#ifndef IMP_TEXTUREMANAGER_H
#define IMP_TEXTUREMANAGER_H

#include "../base/impBase.h"
#include <map>
#include "Texture.h"

IMPGEARS_BEGIN

typedef std::map<std::string, Texture*> TextureMap;

class IMP_API TextureManager
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

#endif // IMP_TEXTUREMANAGER_H
