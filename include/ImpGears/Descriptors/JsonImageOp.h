#ifndef IMP_JSON_IMAGEOP_H
#define IMP_JSON_IMAGEOP_H

#include <Graphics/ImageOperation.h>
#include <Descriptors/JsonReaderWriter.h>

// #include <ctime>
// #include <iostream>
#include <map>

IMPGEARS_BEGIN

//-------------------------------------------------------
void IMP_API generateImageFromJson(const std::string filename);

//-------------------------------------------------------
struct IMP_API ImgOperation : public Object
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
	
	ImgOperation(const std::string& opName);	
	void addDepend(const std::string& name);
	virtual std::string tryString(JsonObject* obj, const std::string& optname);
	virtual float tryNumeric(JsonObject* obj, const std::string& optname);
	bool isReady() const;
	virtual bool load(JsonObject* jo) = 0;
	virtual void apply() = 0;
};

//-------------------------------------------------------
ImgOperation::Ptr IMP_API createFromJsonObj(JsonObject* obj);

//-------------------------------------------------------
template<typename OpTy>
struct IMP_API ImageGenerator : public ImgOperation
{
	Meta_Class(ImageGenerator);
	ImageGenerator(const std::string& name) : ImgOperation(name), _id("no_name") {}

	bool load(JsonObject* jo)
	{
		_id = tryString(jo,"id");
		if( jo->getNumeric("width") ) _width = jo->getNumeric("width")->value;
		else _width = s_width;
		if( jo->getNumeric("height") ) _height = jo->getNumeric("height")->value;
		else _height = s_height;
		return true;
	}

	void apply()
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

//-------------------------------------------------------
template<typename OpTy>
struct IMP_API ImageTransform : public ImageGenerator<OpTy>
{
	Meta_Class(ImageTransform);
	ImageTransform(const std::string& name) : ImageGenerator<OpTy>(name), _input_id("no_name") {}

	bool load(JsonObject* jo)
	{
		_input_id = ImageGenerator<OpTy>::tryString(jo,"input_id");
		ImageGenerator<OpTy>::addDepend(_input_id);
		return ImageGenerator<OpTy>::load(jo);
	}
	
	virtual void generate(Image::Ptr& in, Image::Ptr& out) = 0;

	void generate(Image::Ptr& out)
	{
		Image::Ptr input = ImageGenerator<OpTy>::s_finished[_input_id];
		generate(input,out);
	}
	
	std::string _input_id;
};

//-------------------------------------------------------
struct IMP_API JsonHashOperation : public ImageGenerator<HashOperation>
{
	Meta_Class(JsonHashOperation);
	JsonHashOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonNoiseOperation : public ImageTransform<NoiseOperation>
{
	Meta_Class(JsonNoiseOperation);
	JsonNoiseOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonVoronoiOperation : public ImageTransform<VoronoiOperation>
{
	Meta_Class(JsonVoronoiOperation);
	JsonVoronoiOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonFbmOperation : public ImageTransform<FbmOperation>
{
	Meta_Class(JsonFbmOperation);
	JsonFbmOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonSignalOperation : public ImageGenerator<SignalOperation>
{
	Meta_Class(JsonSignalOperation);
	JsonSignalOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& out);
	
	bool _addNoise;
	std::string _opt_noise_id;
	float _noiseIntensity;
};

//-------------------------------------------------------
struct IMP_API JsonBumpToNormalOperation : public ImageTransform<BumpToNormalOperation>
{
	Meta_Class(JsonBumpToNormalOperation);
	JsonBumpToNormalOperation();
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonPerturbateOperation : public ImageTransform<PerturbateOperation>
{
	Meta_Class(JsonPerturbateOperation);
	JsonPerturbateOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
	
	std::string _uvmap_id;
};

//-------------------------------------------------------
struct IMP_API JsonColorMixOperation : public ImageTransform<ColorMixOperation>
{
	Meta_Class(JsonColorMixOperation);
	JsonColorMixOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
	
	Vec4 _c1, _c2;
};

//-------------------------------------------------------
struct IMP_API JsonSaveOperation : public ImgOperation
{
	Meta_Class(JsonSaveOperation);
	JsonSaveOperation();
	virtual bool load(JsonObject* jo);
	virtual void apply();
	
	std::string _input_id;
	std::string _filename;
};

//-------------------------------------------------------
struct IMP_API JsonBlurOperation : public ImageTransform<Conv2dOperation>
{
	Meta_Class(JsonBlurOperation);
	JsonBlurOperation();
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonDilatationOperation : public ImageTransform<MorphoOperation>
{
	Meta_Class(JsonDilatationOperation);
	JsonDilatationOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

//-------------------------------------------------------
struct IMP_API JsonErosionOperation : public ImageTransform<MorphoOperation>
{
	Meta_Class(JsonErosionOperation);
	JsonErosionOperation();
	virtual bool load(JsonObject* jo);
	virtual void generate(Image::Ptr& in, Image::Ptr& out);
};

IMPGEARS_END


#endif // IMP_JSON_IMAGEOP_H
