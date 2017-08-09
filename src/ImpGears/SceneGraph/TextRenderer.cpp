#include <SceneGraph/TextRenderer.h>

#include <SceneGraph/BmpLoader.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
TextRenderer* TextRenderer::_instance = IMP_NULL;

//--------------------------------------------------------------
TextRenderer::TextRenderer(const std::string& filename)
{
	if( !filename.empty() )
		loadFontSprite(filename);
}

//--------------------------------------------------------------
TextRenderer::~TextRenderer()
{
	
}

//--------------------------------------------------------------
void TextRenderer::loadFontSprite(const std::string& filename)
{
	std::string jsondata = imp::removeSpace( loadJson(filename.c_str()) );
	
	imp::JsonObject* root = dynamic_cast<imp::JsonObject*>( imp::parse(jsondata) );
	imp::JsonObject* config = root->getObject("spritefont-config");
	
	std::string spritefilename = config->getString("file")->value;
	
	imp::ImageData* temp = IMP_NULL;
	BmpLoader::loadFromFile(spritefilename.c_str(), &temp);
	_sprite = new imp::ImageData();
	_sprite->convert(*temp, imp::PixelFormat_BGRA8);
	delete temp;
	
	_charWidth = (unsigned int) config->getArray("caracter-size")->getNumeric(0)->value;
	_charHeight = (unsigned int) config->getArray("caracter-size")->getNumeric(1)->value;
	_mapping = config->getObject("mapping");
}

//--------------------------------------------------------------
bool TextRenderer::findGlyphPosition(char glyph, unsigned int& x, unsigned int& y)
{
	std::string glyphStr = "";
	glyphStr += glyph;
	
	imp::JsonArray* cInfo = _mapping->getArray(glyphStr);
	if(cInfo)
	{
		x = (unsigned int) cInfo->getNumeric(0)->value;
		y = (unsigned int) cInfo->getNumeric(1)->value;
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------
void TextRenderer::renderText(const std::string& text, imp::ImageData* targetImg, const imp::Pixel& fg, const imp::Pixel& bg)
{
	if(_mapping == IMP_NULL)
		return;
	
	targetImg->create(_charWidth * text.size(), _charHeight, imp::PixelFormat_BGRA8);
	
	for(unsigned int i=0; i<text.size(); ++i)
	{
		unsigned int x, y;
		if( findGlyphPosition(text[i],x,y) )
		{
			imp::Rect srcR = {x*_charWidth, _sprite->getHeight() - (y+1)*_charHeight, _charWidth, _charHeight};
			imp::Rect dstR = {i*_charWidth, 0, _charWidth, _charHeight};
			
			targetImg->draw( *_sprite, srcR, dstR );
		}
	}
}

//--------------------------------------------------------------
TextRenderer* TextRenderer::getInstance() { if(_instance == IMP_NULL)_instance = new TextRenderer(); return _instance; }


//--------------------------------------------------------------
std::string TextRenderer::loadJson(const char* filename)
{
	std::string buffer;
	std::ifstream is;
	is.open(filename);
	while( !is.eof() )
	{
		buffer.push_back(is.get());
	}
	is.close();
	return buffer;
}

IMPGEARS_END