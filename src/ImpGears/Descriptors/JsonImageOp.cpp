#include <ImpGears/Graphics/ImageOperation.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/JsonImageOp.h>
#include <ImpGears/Descriptors/FileInfo.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <map>


IMPGEARS_BEGIN

//-------------------------------------------------------
ImgOperation::ImgOperation(const std::string& opName) : _opName(opName), _done(false) {}

//-------------------------------------------------------
void ImgOperation::addDepend(const std::string& name)
{
	_deps.push_back(name);
}

//-------------------------------------------------------
std::string ImgOperation::tryString(JsonObject* obj, const std::string& optname)
{
	if(obj->getString(optname))
		return obj->getString(optname)->value;
	else
		std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
	return std::string();
}

//-------------------------------------------------------
float ImgOperation::tryNumeric(JsonObject* obj, const std::string& optname)
{
	if(obj->getNumeric(optname))
		return obj->getNumeric(optname)->value;
	else
		std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
	return 0.0;
}

//-------------------------------------------------------
bool ImgOperation::isReady() const
{
	for(auto d : _deps)
		if(s_finished.find(d)==s_finished.end())
			return false;
	return true;
}

//-------------------------------------------------------
ImgOperation::ImageBucket ImgOperation::s_finished;
int ImgOperation::s_width = 16;
int ImgOperation::s_height = 16;
int ImgOperation::s_idInc = 0;

//-------------------------------------------------------
using OperationCache = std::vector<ImgOperation::Ptr>;

//-------------------------------------------------------
void generateImageFromJson(const std::string filename)
{
    std::cout << "loading json data..." << std::endl;
	std::string json = loadText(filename);
    generateImageFromJsonData(json);
}
//-------------------------------------------------------
void generateImageFromJsonData(const std::string jsonData)
{
    OperationCache opCache;
    std::string jsonWithoutSpace = removeSpace(jsonData);

    JsonObject* root = dynamic_cast<JsonObject*>( parse(jsonWithoutSpace) );
    JsonArray* array = root->getArray("operations");

    JsonObject* prop = root->getObject("properties");
    ImgOperation::s_width = prop->getNumeric("width")->value;
    ImgOperation::s_height = prop->getNumeric("height")->value;

    int s = array->value.size();
    for(int i=0; i<s; ++i)
    {
        JsonObject* jo = array->getObject(i);
        ImgOperation::Ptr op = createFromJsonObj(jo);
        opCache.push_back(op);
    }
    std::cout << "done" << std::endl;


    std::cout << "start generations... (0/" << opCache.size() << ")" << std::endl;
    int notReady = opCache.size();
    while(notReady)
    {
        for(auto op : opCache) if( !op->_done )
        {
            if( op->isReady() )
            {
                std::cout << "applying " << op->_opName << "..." << std::endl;
                op->apply();
                notReady--;
                std::cout << "done (" << (opCache.size()-notReady) << "/" << opCache.size() << ")" << std::endl;
            }
        }
    }
}

//-------------------------------------------------------
JsonHashOperation::JsonHashOperation() : ImageGenerator("hash") {}

//-------------------------------------------------------
bool JsonHashOperation::load(JsonObject* jo)
{
	op.setSeed( tryNumeric(jo,"seed") );
	return ImageGenerator::load(jo);
}
void JsonHashOperation::generate(Image::Ptr& out) { op.execute(out); }

//-------------------------------------------------------
JsonNoiseOperation::JsonNoiseOperation() : ImageTransform("noise") {}

//-------------------------------------------------------
bool JsonNoiseOperation::load(JsonObject* jo)
{
	op.setFreq( tryNumeric(jo,"freq") );
	return ImageTransform::load(jo);
}

//-------------------------------------------------------
void JsonNoiseOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setHashmap(in);
	op.execute(out);
}

//-------------------------------------------------------
JsonVoronoiOperation::JsonVoronoiOperation() : ImageTransform("voronoi") {}

//-------------------------------------------------------
bool JsonVoronoiOperation::load(JsonObject* jo)
{
	op.setFreq( tryNumeric(jo,"freq") );
	return ImageTransform::load(jo);
}

//-------------------------------------------------------
void JsonVoronoiOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setHashmap(in);
	op.execute(out);
}

//-------------------------------------------------------
JsonFbmOperation::JsonFbmOperation() : ImageTransform("fbm") {}

//-------------------------------------------------------
bool JsonFbmOperation::load(JsonObject* jo)
{
	std::string noiseType = tryString(jo,"noise_type");
	int type = 0; if(noiseType == "voronoi") type = 1;
	op.setOctaves( tryNumeric(jo,"octaves") );
	op.setFreq( tryNumeric(jo,"freq") );
	op.setPersist( tryNumeric(jo,"persist") );
	op.setType(type);
	return ImageTransform::load(jo);
}

//-------------------------------------------------------
void JsonFbmOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setHashmap(in);
	op.execute(out);
}

//-------------------------------------------------------
JsonSignalOperation::JsonSignalOperation() : ImageGenerator("signal") {}

//-------------------------------------------------------
bool JsonSignalOperation::load(JsonObject* jo)
{
	std::string signalType = tryString(jo,"signal_type");
	int type = 0; if(signalType == "radial") type = 1;
	op.setType(type);
	Vec3 dir( tryNumeric(jo,"x"), tryNumeric(jo,"y"), 0.0 );
	op.setTargetVec(dir);
	op.setFreq( tryNumeric(jo,"freq") );
	op.setAmpl( tryNumeric(jo,"ampl") );
	op.setPhase( tryNumeric(jo,"phase") );
	op.setBase( tryNumeric(jo,"base") );
	
	_addNoise = false;
	if(jo->getString("noise"))
	{
		_addNoise = true;
		_opt_noise_id = jo->getString("noise")->value;
		_noiseIntensity = tryNumeric(jo,"noise_intensity");
		addDepend(_opt_noise_id);
	}
	
	return ImageGenerator::load(jo);
}

//-------------------------------------------------------
void JsonSignalOperation::generate(Image::Ptr& out)
{
	if(_addNoise) op.setNoise( s_finished[_opt_noise_id], _noiseIntensity );
	op.execute(out);
}

//-------------------------------------------------------
JsonBumpToNormalOperation::JsonBumpToNormalOperation() : ImageTransform("bumpToNormal") {}

//-------------------------------------------------------
void JsonBumpToNormalOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setTarget(in);
	op.execute(out);
}

//-------------------------------------------------------
JsonPerturbateOperation::JsonPerturbateOperation() : ImageTransform("perturbate") {}

//-------------------------------------------------------
bool JsonPerturbateOperation::load(JsonObject* jo)
{
	op.setIntensity( tryNumeric(jo,"intensity") );
	_uvmap_id = tryString(jo,"uvmap");
	addDepend(_uvmap_id);
	return ImageTransform::load(jo);
}

//-------------------------------------------------------
void JsonPerturbateOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setTarget(in);
	op.setUVMap( s_finished[_uvmap_id] );
	op.execute(out);
}

//-------------------------------------------------------
JsonColorMixOperation::JsonColorMixOperation() : ImageTransform("colorMix") {}

//-------------------------------------------------------
bool JsonColorMixOperation::load(JsonObject* jo)
{
	_c1[0] = tryNumeric(jo,"r1");
	_c1[1] = tryNumeric(jo,"g1");
	_c1[2] = tryNumeric(jo,"b1");
	_c1[3] = 1.0;
	_c2[0] = tryNumeric(jo,"r2");
	_c2[1] = tryNumeric(jo,"g2");
	_c2[2] = tryNumeric(jo,"b2");
	_c2[3] = 1.0;
	op.setColor1( _c1 );
	op.setColor2( _c2 );
	return ImageTransform::load(jo);;
}

//-------------------------------------------------------
void JsonColorMixOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setTarget(in);
	op.execute(out);
}

//-------------------------------------------------------
JsonSaveOperation::JsonSaveOperation() : ImgOperation("save") {}

//-------------------------------------------------------
bool JsonSaveOperation::load(JsonObject* jo)
{
	_input_id = tryString(jo,"input_id");
	_filename = tryString(jo,"filename");
	addDepend(_input_id);
	return true;
}

//-------------------------------------------------------
void JsonSaveOperation::apply()
{
	if( !isReady() ) return;
	ImageIO::save(s_finished[_input_id], _filename);
	_done = true;
}




//-------------------------------------------------------
std::map<std::string,Image::Ptr> JsonFakeSaveOperation::s_fakeFiles;
JsonFakeSaveOperation::JsonFakeSaveOperation() : ImgOperation("fakesave") {}
bool JsonFakeSaveOperation::load(JsonObject* jo)
{
    _input_id = tryString(jo,"input_id");
    _filename = tryString(jo,"filename");
    addDepend(_input_id);
    return true;
}
void JsonFakeSaveOperation::apply()
{
    if( !isReady() ) return;
    s_fakeFiles[_filename] = s_finished[_input_id];
    _done = true;
}
//-------------------------------------------------------





//-------------------------------------------------------
JsonBlurOperation::JsonBlurOperation() : ImageTransform("blur") {}

//-------------------------------------------------------
void JsonBlurOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	const float blur_data[9] = {
		0.111,0.111,0.111,
		0.111,0.111,0.111,
		0.111,0.111,0.111};
		
	Matrix3 blur(blur_data,true);
	op.setTarget(in);
	op.setMatrix(blur);
	op.execute(out);
}

//-------------------------------------------------------
JsonDilatationOperation::JsonDilatationOperation() : ImageTransform("dilatation") {}

//-------------------------------------------------------
bool JsonDilatationOperation::load(JsonObject* jo)
{
	const float morpho_data[9] = {
		1.0,1.0,1.0,
		1.0,1.0,1.0,
		1.0,1.0,1.0};
		
	Matrix3 morpho(morpho_data,true);
	op.setMatrix(morpho);
	return ImageTransform::load(jo);
}

//-------------------------------------------------------
void JsonDilatationOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setTarget(in);
	op.execute(out);
}

//-------------------------------------------------------
JsonErosionOperation::JsonErosionOperation() : ImageTransform("erosion") {}

//-------------------------------------------------------
bool JsonErosionOperation::load(JsonObject* jo)
{
	const float morpho_data[9] = {
		1.0,1.0,1.0,
		1.0,1.0,1.0,
		1.0,1.0,1.0};
		
	Matrix3 morpho(morpho_data,true);
	op.setMatrix(morpho);
	op.setType(-1.0);
	return ImageTransform::load(jo);
}

//-------------------------------------------------------
void JsonErosionOperation::generate(Image::Ptr& in, Image::Ptr& out)
{
	op.setTarget(in);
	op.execute(out);
}

//-------------------------------------------------------
ImgOperation::Ptr createFromJsonObj(JsonObject* obj)
{
	ImgOperation::Ptr res;
	
	std::string opname = obj->getString("op")->value;
	if(opname == "hash") res = JsonHashOperation::create();
	else if(opname == "noise") res = JsonNoiseOperation::create();
	else if(opname == "voronoi") res = JsonVoronoiOperation::create();
	else if(opname == "save") res = JsonSaveOperation::create();
    else if(opname == "fakesave") res = JsonFakeSaveOperation::create();
	else if(opname == "fbm") res = JsonFbmOperation::create();
	else if(opname == "signal") res = JsonSignalOperation::create();
	else if(opname == "colorMix") res = JsonColorMixOperation::create();
	else if(opname == "bumpToNormal") res = JsonBumpToNormalOperation::create();
	else if(opname == "perturbate") res = JsonPerturbateOperation::create();
	else if(opname == "blur") res = JsonBlurOperation::create();
	else if(opname == "dilatation") res = JsonDilatationOperation::create();
	else if(opname == "erosion") res = JsonErosionOperation::create();
	
	if(res == nullptr)
		std::cerr << "cannot load operation " << opname << std::endl;
	else
		res->load(obj);
	return res;
}

IMPGEARS_END
