#ifndef IMP_TEXT_RENDERER
#define IMP_TEXT_RENDERER

#include <Core/impBase.h>
#include <Utils/JsonReaderWriter.h>
#include <SceneGraph/ImageData.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API TextRenderer
{
	public:

	TextRenderer(const std::string& filename = "../commun/spritefont-config.json");

	virtual ~TextRenderer();

	void loadFontSprite(const std::string& filename);

	bool findGlyphPosition(char glyph, unsigned int& x, unsigned int& y);
	
	void renderText(const std::string& text, imp::ImageData* targetImg, const imp::Pixel& fg, const imp::Pixel& bg);

	static TextRenderer* getInstance();
	
	float getDefaultSize() const {return (float)_charHeight;}

protected:
	std::string loadJson(const char* filename);
	
	imp::ImageData*		_sprite;
	unsigned int		_charWidth;
	unsigned int		_charHeight;
	imp::JsonObject*	_mapping;

	static TextRenderer* _instance;
};

IMPGEARS_END

#endif // IMP_TEXT_RENDERER