#include <iostream>
#include <fstream>

#include <Graphics/ImageData.h>
#include <Graphics/BmpLoader.h>
#include <Data/ImageUtils.h>
#include <Data/JsonReaderWriter.h>
#include <Math/Perlin.h>
#include <System/FileInfo.h>
#include <Math/BasicFunctions.h>

#include <map>

struct Filter
{
	imp::ImageData* data;
	imp::JsonObject* json;
	bool ready;
	std::vector<Filter*> dependanced;

	Filter(imp::ImageData* img, imp::JsonObject* json)
	{
		data = img;
		this->json = json;
		ready = false;
	}

	~Filter(){}

	void addDependanced(Filter* filter)
	{
		for(unsigned int i=0; i<dependanced.size(); ++i)
		{
			if(dependanced[i] == filter)
				return;
		}

		dependanced.push_back(filter);
	}

	bool alldependancedReady()
	{
		for(unsigned int i=0; i<dependanced.size(); ++i)
		{
			if( !dependanced[i]->ready )
				return false;
		}
		return true;
	}
};

typedef std::map<std::string, Filter*> FilterMap;

typedef std::map<std::string, imp::Distribution> DistribMap;

FilterMap filters;

DistribMap distribs;

void loadDistrib(imp::JsonObject* json)
{
	imp::Distribution d;
	std::string id = json->getString("id")->value;

	imp::JsonArray* arr = json->getArray("values");
	if(arr != NULL)
	{
		for(unsigned int i=0; i<arr->value.size(); ++i)
		{
			imp::JsonArray* subarr = arr->getArray(i);
			imp::Distribution::KV kv = { subarr->getNumeric(0)->value, subarr->getNumeric(1)->value };
			d._values.push_back(kv);
		}
	}
	distribs[id] = d;
}

imp::Pixel getPixel(imp::JsonObject* json, std::string id)
{
	imp::JsonArray* arr = json->getArray(id);
	imp::Uint8 b = arr->getNumeric(0)->value;
	imp::Uint8 g = arr->getNumeric(1)->value;
	imp::Uint8 r = arr->getNumeric(2)->value;
	imp::Uint8 a = arr->getNumeric(3)->value;
	imp::Pixel px = {r,g,b,a};
	return px;
}

imp::ImageData* getImage(imp::JsonObject* json, const char* keyname, bool& isReady)
{
	imp::JsonString* string = json->getString(keyname);
	if(string != NULL)
	{
		std::string id = string->value;
		std::string actionID = json->getString("id")->value;
		isReady = filters[id]->ready;
		filters[id]->addDependanced( filters[actionID] );
		return filters[id]->data;
	}
	else
	{
		isReady = false;
		return IMP_NULL;
	}
}

imp::Distribution& getDistrib(imp::JsonObject* json, const char* keyname)
{
	imp::JsonString* string = json->getString(keyname);

	std::string id = string->value;
	return distribs[id];
}

std::string loadJson(const char* filename)
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

bool tryGetString(imp::JsonObject* json, const std::string& id, std::string& str)
{
	if( json->getString(id) )
	{
		str = json->getString(id)->value;
		return true;
	}
	return false;	
}

bool tryGetNumeric(imp::JsonObject* json, const std::string& id, double& num)
{
	if( json->getNumeric(id) )
	{
		num = json->getNumeric(id)->value;
		return true;
	}
	return false;	
}


class Functor
{
	public:

	Functor(FilterMap* filters, DistribMap* distrib)
		: filters(filters)
		, distrib(distrib)
	{}
	virtual ~Functor(){}

	virtual bool apply(imp::JsonObject* json, imp::ImageData& img){return false;}
	
	bool operator()(imp::JsonObject* json, imp::ImageData& img)
	{
		return apply(json, img);
	}

	private:	

	FilterMap* filters;
	DistribMap* distrib;
};

class NoiseFunctor : public Functor
{
	public:
	NoiseFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		double frequency = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double octaveCount = json->getNumeric("octaves")->value;
		double persistence = json->getNumeric("persistence")->value;

		double tiles = 1.0;
		tryGetNumeric(json, "tiles", tiles);

		std::string method = "perlin";
		tryGetString(json, "method", method);

		for(unsigned int i=0; i<img.getWidth(); ++i)
		{
			for(unsigned int j=0; j<img.getHeight(); ++j)
			{
				double nX = ((double)i)/img.getWidth();
				double nY = ((double)j)/img.getHeight();
				double v = 0.0;
				if(method == "perlin")
					v = imp::perlinOctave(nX, nY, 0, octaveCount, persistence, frequency, 1.0/tiles);
				else if(method == "simplex")
					v = imp::simplexOctave(nX, nY, 0, octaveCount, persistence, frequency, 1.0/tiles);
				
				imp::Uint8 comp = (v+1.0)/2.0 * ampl;
				imp::Pixel px = {comp,comp,comp,255};
				img.setPixel(i,j,px);
			}
		}
		return true;
	}
};

class SinusFunctor : public Functor
{
	public:
	SinusFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return false;

		double dirX = json->getNumeric("dirX")->value;
		double dirY = json->getNumeric("dirY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		tryGetNumeric(json, "ratio", ratio);

		drawDirectionnalSinus(img, dirX, dirY, freq, ampl, ratio, perturbation, perturbIntensity);
		return true;
	}
};

class SquareFunctor : public Functor
{
	public:
	SquareFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return false;

		double dirX = json->getNumeric("dirX")->value;
		double dirY = json->getNumeric("dirY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		tryGetNumeric(json, "ratio", ratio);

		drawDirectionnalSignal(img, imp::SignalType_Square, dirX, dirY, freq, ampl, ratio, perturbation, perturbIntensity);
		return true;
	}
};

class CellularNoiseFunctor : public Functor
{
	public:
	CellularNoiseFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "noiseMapID", perturbReady);

		if( perturbation && !perturbReady )
			return false;

		double cellcount = json->getNumeric("count")->value;

        drawCellularNoise(img, cellcount, perturbation);
		return true;
	}
};

class TriangleFunctor : public Functor
{
	public:
	TriangleFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return false;

		double dirX = json->getNumeric("dirX")->value;
		double dirY = json->getNumeric("dirY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		tryGetNumeric(json, "ratio", ratio);

		drawDirectionnalSignal(img, imp::SignalType_Triangle, dirX, dirY, freq, ampl, ratio, perturbation, perturbIntensity);
		return true;
	}
};

class NormalMapFunctor : public Functor
{
	public:
	NormalMapFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* height = getImage(json, "heightmapID", perturbReady);

		if( height && !perturbReady ) return false;

		double force = json->getNumeric("force")->value;
		double prec = json->getNumeric("prec")->value;
		imp::heightToNormal(*height, img, force, prec);
		return true;
	}
};

class HighPassFunctor : public Functor
{
	public:
	HighPassFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
        /*
        const double  hpfilter[3][3] = 
        {
            { 0.0,      -0.25,      0.0 },
            { -0.25,    2.0,        -0.25 },
            { 0.0,      -0.25,      0.0 }
        };
        */
        const double  hpfilter[3][3] = 
        {
            { -1.0,      -1.0,      -1.0 },
            { -1.0,     9.0,        -1.0 },
            { -1.0,      -1.0,      -1.0 }
        };
        
        
		bool inputReady = false;
		imp::ImageData* input = getImage(json, "inputID", inputReady);
		if( input && !inputReady ) return false;
        
        
        for(unsigned int i = 1; i<img.getWidth()-1; ++i)
        {
            for(unsigned int j=1; j<img.getHeight()-1; ++j)
            {
                
                double v=  0.0;
                
                for(int fi = -1; fi<=1; ++fi)
                {
                    for(int fj=-1; fj<=1; ++fj)
                    {
                        v += input->getPixel(i+fi,j+fj).red/255.0 * hpfilter[fi+1][fj+1] ;
                    }
                }
                
                if(v<0.0)v = 0.0;
                else if(v>1.0)v = 1.0;
                
                imp::Pixel pixel;
                pixel.red = (unsigned char)(v * 255);
                pixel.green = (unsigned char)(v * 255);
                pixel.blue = (unsigned char)(v * 255);
                img.setPixel(i,j,pixel);
            }
        }
        
		return true;
	}
};

class ColorizationFunctor : public Functor
{
	public:
	ColorizationFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* grey = getImage(json, "greymapID", perturbReady);

		if( grey && !perturbReady )
			return false;

		bool hasDistrib = json->getString("distribID") != NULL;
		img.clone(*grey);

		imp::Pixel color1 = getPixel(json, "color1");
		imp::Pixel color2 = getPixel(json, "color2");

		if(hasDistrib)
			applyColorization(img, color1, color2, getDistrib(json, "distribID"));
		else
			applyColorization(img, color1, color2);

		return true;
	}
};

class MaximizationFunctor : public Functor
{
	public:
	MaximizationFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool ready = false;
		imp::ImageData* grey = getImage(json, "greymapID", ready);

		if( grey && !ready )return false;
		
		img.clone(*grey);
		imp::applyMaximization(img);
		return true;
	}
};

class RadialSinusFunctor : public Functor
{
	public:
	RadialSinusFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return false;

		double posX = json->getNumeric("posX")->value;
		double posY = json->getNumeric("posY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		tryGetNumeric(json, "ratio", ratio);

		imp::drawRadialSinus(img, posX, posY, freq, ampl, ratio, perturbation, perturbIntensity);
		return true;
	}
};

class BlendFunctor : public Functor
{
	public:
	BlendFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool srcReady = false;
		imp::ImageData* src = getImage(json, "srcID", srcReady);
		bool dstReady = false;
		imp::ImageData* dst = getImage(json, "dstID", dstReady);
		if( src && dst && (!srcReady || !dstReady) ) return false;

		img.clone(*dst);
		double alpha = json->getNumeric("alpha")->value;
		imp::blend(img, *src, alpha);
		return true;
	}
};

class BlendMapFunctor : public Functor
{
	public:
	BlendMapFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool srcReady = false;
		imp::ImageData* src = getImage(json, "srcID", srcReady);
		bool dstReady = false;
		imp::ImageData* dst = getImage(json, "dstID", dstReady);
        bool alphaReady = false;
		imp::ImageData* alpha = getImage(json, "alphaID", alphaReady);
		if( src && dst &&  alpha && (!srcReady || !dstReady || !alphaReady) ) return false;

        double threshold = json->getNumeric("threshold")->value;
        
		img.clone(*dst);
		imp::blend(img, *src, *alpha, threshold);
		return true;
	}
};


class SaveFunctor : public Functor
{
	public:
	SaveFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool ready = false;
		imp::ImageData* target = getImage(json, "targetID", ready);
		std::string url = json->getString("url")->value;

		if( target && !ready ) return false;
	
		std::cout << "test\n";
		img.clone(*target);
		BmpLoader::saveToFile(&img, url.c_str());
		return true;
	
	}
};

class CreateFunctor : public Functor
{
	public:
	CreateFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		double width = json->getNumeric("width")->value;
		double height = json->getNumeric("height")->value;
		imp::Pixel color = getPixel(json, "color");
		img.create(width, height, imp::PixelFormat_RGB8);
		img.fill(color);
		return true;
	
	}
};

class DrawFunctor : public Functor
{
	public:
	DrawFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		unsigned int offsetx = (unsigned int)json->getNumeric("offsetx")->value;
		unsigned int offsety = (unsigned int)json->getNumeric("offsety")->value;
	
		bool targetReady = false;
		imp::ImageData* targetImg = getImage(json, "targetID", targetReady);
		bool srcReady = false;
		imp::ImageData* srcImg = getImage(json, "srcID", srcReady);
		if( srcReady && targetReady)
		{
			img.clone(*targetImg);
			imp::Rect srcRect = {0, 0, srcImg->getWidth(), srcImg->getHeight()};
			imp::Rect dstRect = {offsetx, offsety, srcImg->getWidth(), srcImg->getHeight()};
			img.draw(*srcImg, srcRect, dstRect);
			return true;
		}
		else
		{
			return false;
		}
	
	}
};

typedef std::map<std::string, Functor*> FunctorMap;
FunctorMap functors;

void doAction(Filter* filter)
{
	imp::JsonObject* json = filter->json;
	imp::ImageData& img = *(filter->data);
	std::cout << "do action " << json->getString("id")->value << "\n";
	std::string action = json->getString("type")->value;
	if( functors.find( action ) != functors.end() )
	{
		filter->ready = functors[action]->apply(json, img);
	}
	else
	{
		std::cout << "impError : action type unsupported - " << json->getString("type")->value << "\n";
	}
}

void generateFilters(const std::string& json)
{
	functors["sinus"] = new SinusFunctor(&filters, &distribs);
	functors["square"] = new SquareFunctor(&filters, &distribs);
	functors["triangle"] = new TriangleFunctor(&filters, &distribs);
	functors["cellularnoise"] = new CellularNoiseFunctor(&filters, &distribs);
	functors["noise"] = new NoiseFunctor(&filters, &distribs);
	functors["heightToNormal"] = new NormalMapFunctor(&filters, &distribs);
	functors["colorization"] = new ColorizationFunctor(&filters, &distribs);
    functors["highpass"] = new HighPassFunctor(&filters, &distribs);
	functors["maximization"] = new MaximizationFunctor(&filters, &distribs);
	functors["radSinus"] = new RadialSinusFunctor(&filters, &distribs);
	functors["blend"] = new BlendFunctor(&filters, &distribs);
	functors["blendmap"] = new BlendMapFunctor(&filters, &distribs);
	functors["save"] = new SaveFunctor(&filters, &distribs);
	functors["create"] = new CreateFunctor(&filters, &distribs);
	functors["draw"] = new DrawFunctor(&filters, &distribs);

	std::string jsonWithoutSpace = imp::removeSpace(json);
	imp::JsonObject* root = dynamic_cast<imp::JsonObject*>( imp::parse(jsonWithoutSpace) );
	imp::JsonArray* array = root->getArray("actions");
	imp::JsonArray* distribArr = root->getArray("utils");

	imp::JsonObject* config = root->getObject("config");
	double width = config->getNumeric("defaultWidth")->value;
	double height = config->getNumeric("defaultHeight")->value;
	
	for(unsigned int i=0; i<distribArr->value.size(); ++i)
	{
		imp::JsonObject* distrib = distribArr->getObject(i);
		loadDistrib(distrib);
	}

	for(unsigned int i=0; i<array->value.size(); ++i)
	{
		imp::JsonObject* filter = array->getObject(i);
		std::string id = filter->getString("id")->value;
		filters[id] = new Filter(new imp::ImageData(),filter);
		filters[id]->data->create(width, height, imp::PixelFormat_RGB8);
	}

	unsigned int previousReadyCount = -1;
	unsigned int readyCount = 0;

	while( readyCount < array->value.size() && previousReadyCount != readyCount )
	{
		previousReadyCount = readyCount;

		for(unsigned int i=0; i<array->value.size(); ++i)
		{
			imp::JsonObject* filter = array->getObject(i);
			std::string id = filter->getString("id")->value;
			if(filters[id]->ready == false)
			{
				doAction(filters[id]);
				if(filters[id]->ready)
					readyCount++;
			}
		}

		// free image not used anymore
		for(unsigned int i=0; i<array->value.size(); ++i)
		{
			imp::JsonObject* filter = array->getObject(i);
			std::string id = filter->getString("id")->value;
			if( filters[id]->ready && filters[id]->alldependancedReady() )
			{
				filters[id]->data->destroy();
			}
		}
	}

	if(previousReadyCount == readyCount)
	{
		std::cout << "impError : loop detected. One or more filter is not ready\n";
	}

	// debug generated images
	/*for(unsigned int i=0; i<filters.size(); ++i)
	{
		imp::JsonObject* filter = array->getObject(i);
		std::string id = filter->getString("id")->value;
		BmpLoader::saveToFile(filters[id]->data, (id + ".bmp").c_str());
	}*/
}

int main(int argc, char* argv[])
{
	std::string filename = "procedure.json";
	if(argc > 1)
		filename = argv[1];

	generateFilters( loadJson(filename.c_str()) );
	return 0;
}
