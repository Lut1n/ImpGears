#include <SceneGraph/TextureManager.h>
#include <SceneGraph/PngLoader.h>

IMPGEARS_BEGIN

TextureMap TextureManager::textures;

TextureManager::TextureManager()
{
    //ctor
}

TextureManager::~TextureManager()
{
    //dtor
}

Texture* TextureManager::getTexture(std::string name){
    Texture* result = textures[name];

    if(result == IMP_NULL){
        result = PngLoader::loadFromFile(name.c_str());
        textures[name] = result;
    }

    return result;
}

void TextureManager::addTexture(std::string name, Texture* texture){
    textures[name] = texture;
}

IMPGEARS_END
