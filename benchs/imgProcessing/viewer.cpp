#include <Graphics/ImageOperation.h>
#include <Utils/ImageIO.h>
#include <Utils/JsonReaderWriter.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <map>

using namespace imp;

std::string loadText(const std::string& filename)
{
	std::ifstream ifs(filename);
	std::string buffer( (std::istreambuf_iterator<char>(ifs) ),
						(std::istreambuf_iterator<char>()    ) );
	return buffer;
}


// -----------------------------------------------------------------------------------------------------------------------
struct ImgOperation
{
	Meta_Class(ImgOperation);
	
	using ImageBucket = std::map<std::string, Image::Ptr>;
	
	static ImageBucket s_finished;
	static int s_width;
	static int s_height;
	static int s_idInc;
	
	std::vector<std::string> _deps;
	std::string _opName;
	bool _done;
	
	ImgOperation(const std::string& opName) : _opName(opName), _done(false) {}
	
	void addDepend(const std::string& name)
	{
		_deps.push_back(name);
	}
	
	virtual std::string tryString(JsonObject* obj, const std::string& optname)
	{
		if(obj->getString(optname))
			return obj->getString(optname)->value;
		else
			std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
		return std::string();
	}
	virtual float tryNumeric(JsonObject* obj, const std::string& optname)
	{
		if(obj->getNumeric(optname))
			return obj->getNumeric(optname)->value;
		else
			std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
		return 0.0;
	}
	
	bool isReady() const
	{
		for(auto d : _deps)
			if(s_finished.find(d)==s_finished.end())
				return false;
		return true;
	}
	
	virtual bool load(JsonObject* jo) = 0;
	virtual void apply() = 0;
};

// -----------------------------------------------------------------------------------------------------------------------
ImgOperation::ImageBucket ImgOperation::s_finished;
int ImgOperation::s_width = 16;
int ImgOperation::s_height = 16;
int ImgOperation::s_idInc = 0;

// -----------------------------------------------------------------------------------------------------------------------
ImgOperation::Ptr createFromJsonObj(JsonObject* obj);

// -----------------------------------------------------------------------------------------------------------------------
using OperationCache = std::vector<ImgOperation::Ptr>;

// -----------------------------------------------------------------------------------------------------------------------
void generateFromJson(const std::string filename)
{
	std::cout << "loading json data..." << std::endl;
	OperationCache opCache;
	std::string json = loadText(filename);
	std::string jsonWithoutSpace = removeSpace(json);
	
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
// -----------------------------------------------------------------------------------------------------------------------
template<typename OpTy>
struct ImageGenerator : public ImgOperation
{
	Meta_Class(ImageGenerator);
	
	ImageGenerator(const std::string& name) : ImgOperation(name), _id("no_name") {}
	
	virtual bool load(JsonObject* jo)
	{
		_id = tryString(jo,"id");
		if( jo->getNumeric("width") ) _width = jo->getNumeric("width")->value;
		else _width = s_width;
		if( jo->getNumeric("height") ) _height = jo->getNumeric("height")->value;
		else _height = s_height;
		return true;
	}
	
	virtual void apply()
	{
		if( !isReady() ) return;
		
		Image::Ptr res = Image::create(_width,_height,3);
		generate(res);
		s_finished[_id] = res;
		_done = true;
	}
	
	virtual void generate(Image::Ptr& out) = 0;
	
	std::string _id;
	OpTy op;
	int _width;
	int _height;
};

// -----------------------------------------------------------------------------------------------------------------------
template<typename OpTy>
struct ImageTransform : public ImageGenerator<OpTy>
{
	Meta_Class(ImageTransform);
	
	ImageTransform(const std::string& name) : ImageGenerator<OpTy>(name), _input_id("no_name") {}
	
	virtual bool load(JsonObject* jo)
	{
		_input_id = ImageGenerator<OpTy>::tryString(jo,"input_id");
		ImageGenerator<OpTy>::addDepend(_input_id);
		return ImageGenerator<OpTy>::load(jo);
	}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out) = 0;
	virtual void generate(Image::Ptr& out)
	{
		Image::Ptr input = ImageGenerator<OpTy>::s_finished[_input_id];
		generate(input,out);
	}
	
	std::string _input_id;
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonHashOperation : public ImageGenerator<HashOperation>
{
	Meta_Class(JsonHashOperation);
	
	JsonHashOperation() : ImageGenerator("hash") {}
	
	virtual bool load(JsonObject* jo)
	{
		op.setSeed( tryNumeric(jo,"seed") );
		return ImageGenerator::load(jo);
	}
	virtual void generate(Image::Ptr& out) { op.execute(out); }
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonNoiseOperation : public ImageTransform<NoiseOperation>
{
	Meta_Class(JsonNoiseOperation);
	
	JsonNoiseOperation() : ImageTransform("noise") {}
	
	virtual bool load(JsonObject* jo)
	{
		op.setFreq( tryNumeric(jo,"freq") );
		return ImageTransform::load(jo);
	}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setHashmap(in);
		op.execute(out);
	}
};


// -----------------------------------------------------------------------------------------------------------------------
struct JsonVoronoiOperation : public ImageTransform<VoronoiOperation>
{
	Meta_Class(JsonVoronoiOperation);
	
	JsonVoronoiOperation() : ImageTransform("voronoi") {}
	
	virtual bool load(JsonObject* jo)
	{
		op.setFreq( tryNumeric(jo,"freq") );
		return ImageTransform::load(jo);
	}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setHashmap(in);
		op.execute(out);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonFbmOperation : public ImageTransform<FbmOperation>
{
	Meta_Class(JsonFbmOperation);
	
	JsonFbmOperation() : ImageTransform("fbm") {}
	
	virtual bool load(JsonObject* jo)
	{
		std::string noiseType = tryString(jo,"noise_type");
		int type = 0; if(noiseType == "voronoi") type = 1;
		op.setOctaves( tryNumeric(jo,"octaves") );
		op.setFreq( tryNumeric(jo,"freq") );
		op.setPersist( tryNumeric(jo,"persist") );
		op.setType(type);
		return ImageTransform::load(jo);
	}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setHashmap(in);
		op.execute(out);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonSignalOperation : public ImageGenerator<SignalOperation>
{
	Meta_Class(JsonSignalOperation);
	
	JsonSignalOperation() : ImageGenerator("signal") {}
	
	virtual bool load(JsonObject* jo)
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
	
	virtual void generate(Image::Ptr& out)
	{
		if(_addNoise) op.setNoise( s_finished[_opt_noise_id], _noiseIntensity );
		op.execute(out);
	}
	
	bool _addNoise;
	std::string _opt_noise_id;
	float _noiseIntensity;
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonBumpToNormalOperation : public ImageTransform<BumpToNormalOperation>
{
	Meta_Class(JsonBumpToNormalOperation);
	JsonBumpToNormalOperation() : ImageTransform("bumpToNormal") {}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setTarget(in);
		op.execute(out);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonPerturbateOperation : public ImageTransform<PerturbateOperation>
{
	Meta_Class(JsonPerturbateOperation);
	
	JsonPerturbateOperation() : ImageTransform("perturbate") {}
	
	virtual bool load(JsonObject* jo)
	{
		op.setIntensity( tryNumeric(jo,"intensity") );
		_uvmap_id = tryString(jo,"uvmap");
		addDepend(_uvmap_id);
		return ImageTransform::load(jo);
	}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setTarget(in);
		op.setUVMap( s_finished[_uvmap_id] );
		op.execute(out);
	}
	
	std::string _uvmap_id;
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonColorMixOperation : public ImageTransform<ColorMixOperation>
{
	Meta_Class(JsonColorMixOperation);
	
	JsonColorMixOperation() : ImageTransform("colorMix") {}
	
	virtual bool load(JsonObject* jo)
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
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setTarget(in);
		op.execute(out);
	}
	
	Vec4 _c1, _c2;
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonSaveOperation : public ImgOperation
{
	Meta_Class(JsonSaveOperation);
	
	JsonSaveOperation() : ImgOperation("save") {}
	
	virtual bool load(JsonObject* jo)
	{
		_input_id = tryString(jo,"input_id");
		_filename = tryString(jo,"filename");
		addDepend(_input_id);
		return true;
	}
	virtual void apply()
	{
		if( !isReady() ) return;
		ImageIO::save(s_finished[_input_id], _filename);
		_done = true;
	}
	
	std::string _input_id;
	std::string _filename;
};


// -----------------------------------------------------------------------------------------------------------------------
struct JsonBlurOperation : public ImageTransform<Conv2dOperation>
{
	Meta_Class(JsonBlurOperation);
	
	JsonBlurOperation() : ImageTransform("blur") {}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
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
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonDilatationOperation : public ImageTransform<MorphoOperation>
{
	Meta_Class(JsonDilatationOperation);
	
	JsonDilatationOperation() : ImageTransform("dilatation") {}
	
	virtual bool load(JsonObject* jo)
	{
		const float morpho_data[9] = {
			1.0,1.0,1.0,
			1.0,1.0,1.0,
			1.0,1.0,1.0};
			
		Matrix3 morpho(morpho_data,true);
		op.setMatrix(morpho);
		return ImageTransform::load(jo);
	}
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setTarget(in);
		op.execute(out);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct JsonErosionOperation : public ImageTransform<MorphoOperation>
{
	Meta_Class(JsonErosionOperation);
	
	JsonErosionOperation() : ImageTransform("erosion") {}
	
	virtual bool load(JsonObject* jo)
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
	virtual void generate(Image::Ptr& in, Image::Ptr& out)
	{
		op.setTarget(in);
		op.execute(out);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
ImgOperation::Ptr createFromJsonObj(JsonObject* obj)
{
	ImgOperation::Ptr res;
	
	std::string opname = obj->getString("op")->value;
	if(opname == "hash") res = JsonHashOperation::create();
	else if(opname == "noise") res = JsonNoiseOperation::create();
	else if(opname == "voronoi") res = JsonVoronoiOperation::create();
	else if(opname == "save") res = JsonSaveOperation::create();
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


// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{	
	if(argc <2)
	{
		std::cout << "no file given" << std::endl;
		return 0;
	}
   
	generateFromJson(argv[1]);
   
    return 0;
}


