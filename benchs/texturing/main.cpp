#include <iostream>
#include <fstream>

#include <SceneGraph/ImageData.h>
#include <SceneGraph/BmpLoader.h>
#include <Texture/ImageUtils.h>
#include <Utils/JsonReaderWriter.h>
#include <Core/Perlin.h>
#include <Core/BasicFunctions.h>

#include <map>

using namespace imp;


enum SignalType
{
	SignalType_Unknown = 0,
	SignalType_Sinus,
	SignalType_Square,
	SignalType_Triangle,
};

class Distribution
{
	public:

	struct KV
	{
		double first;
		double second;
	};

	typedef std::vector<KV> KVVector;

	Distribution()
	{
	}

	double operator()(double t)
	{
		double result = 0.0;
		for(unsigned int i=0; i<_values.size(); ++i)
		{
			if(i+1 <_values.size() && _values[i].first <= t && _values[i+1].first > t)
			{
				double local = t - _values[i].first;
				double length = _values[i+1].first - _values[i].first;
				result = imp::Lerp(_values[i].second, _values[i+1].second, local/length);
				break;
			}
			else
			{
				result = _values[i].second;
			}
		}
		return result;
	}

	~Distribution()
	{
	}

	KVVector _values;
};


class IMP_API Randomizer
{
public:
	Randomizer(int seed = 0)
	{
		std::srand(std::time(NULL));
	}

	~Randomizer(){}

	int pull()
	{
		return std::rand();
	}
};

static Randomizer randomizer;



void heightToNormal(ImageData& in, ImageData& out, float force, float prec);

void blend(ImageData& dst, ImageData& src, float alpha);

void blend(ImageData& dst, ImageData& src, ImageData& alphaMap, double threshold);

void applyBilinearInterpo(ImageData& bitmap, float frqX, float frqY);

inline Vec4 mirGet(imp::ImageData& img, unsigned int x, unsigned int y);

void drawDirectionnalSinus(imp::ImageData& img, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio = 0.5, const imp::ImageData* perturbation = nullptr, float perturbIntensity = 1.0);

void drawDirectionnalSignal(imp::ImageData& img, SignalType signaltype, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio = 0.5, const imp::ImageData* perturbation = nullptr, float perturbIntensity = 1.0);

void drawRadialSinus(imp::ImageData& img, double posX, double posY, float freq, float ampl, float maxPeriodRatio = 0.5, const imp::ImageData* perturbation = nullptr, float perturbIntensity = 1.0);

void applyPerturbation(imp::ImageData& img, const imp::ImageData& normals, float intensity);

void applyColorization(imp::ImageData& img, const Vec4& color1, const Vec4& color2);

void applyColorization(imp::ImageData& img, const Vec4& color1, const Vec4& color2, Distribution& distrib);

void applyMaximization(imp::ImageData& img);

void drawCellularNoise(imp::ImageData& img, unsigned int cellcount, const imp::ImageData* noisemap);


void initCells(double* xcell, double* ycell, int size, const imp::ImageData* noise)
{
    double fieldW = (noise->getWidth()/size);
    double fieldH = (noise->getHeight()/size);
    
    // initialize cell centers
    for(int i=0; i<size; ++i)
    for(int j=0; j<size; ++j)
    {
        
        double x = (double)i * fieldW;
        double y = (double)j * fieldH;
        
        xcell[j*size+i] = x + double(noise->getPixel(x+fieldW/2.0,y).r)/255.0 * fieldW;
        ycell[j*size+i] = y + double(noise->getPixel(x,y+fieldH/2.0).r)/255.0 * fieldH;
        
        std::cout << "x=" << xcell[j*size+i] << "; y=" << ycell[j*size+i] << std::endl;
    }
}

double voronoi(double* xcell, double* ycell, int count, double x, double y);

void drawCellularNoise_local(imp::ImageData& img, unsigned int cellcount, const imp::ImageData* noisemap)
{
    double xcell[cellcount*cellcount];
    double ycell[cellcount*cellcount];
    initCells(xcell, ycell, cellcount, noisemap);
    
    // draw
    for(unsigned int i=0; i<img.getWidth(); ++i)
    {
        for(unsigned int j=0; j<img.getHeight(); ++j)
        {
            double i2 = i + (double(noisemap->getPixel(i,j).r)/255.0 - 0.5) * 20.0;
            double j2 = j + (double(noisemap->getPixel(img.getWidth()-1-i,j).r)/255.0 - 0.5) * 20.0;
            
            double value = voronoi(xcell, ycell, cellcount*cellcount, i2, j2);
            
            // apply new value
            Uint8 newt = (Uint8)Lerp( 0.0, 255.0, value);
            Pixel px = {newt,newt,newt, 255};
            img.setPixel(i,j,px);
        }
    }
    
}

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
		std::string actionID = "";
        if(json->getString("id"))
        {
            actionID = json->getString("id")->value;
            isReady = filters[id]->ready;
            filters[id]->addDependanced( filters[actionID] );
            return filters[id]->data;
        }
        else
        {
            std::cout << "[texgen error] cannot found the image " <<  string->value << std::endl;
            return IMP_NULL;
        }
	}
	else
	{
           std::cout << "[texgen error] cannot found the image " <<  keyname << std::endl;
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

        drawCellularNoise_local(img, cellcount, perturbation);
		return true;
	}
};

class AddFunctor : public Functor
{
	public:
	AddFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool srcReady = false;
		imp::ImageData* src = getImage(json, "srcID", srcReady);
		bool dstReady = false;
		imp::ImageData* dst = getImage(json, "dstID", dstReady);
		if( src && dst && (!srcReady || !dstReady) ) return false;

        for(unsigned int i=0; i<img.getWidth(); ++i)
		{
			for(unsigned int j=0; j<img.getHeight(); ++j)
			{
                double r = src->getPixel(i,j).red/255.0 + dst->getPixel(i,j).red/255.0;
                double g = src->getPixel(i,j).green/255.0 + dst->getPixel(i,j).green/255.0;
                double b= src->getPixel(i,j).blue/255.0 + dst->getPixel(i,j).blue/255.0;
				
                if(r<0.0) r=0.0; else if(r>1.0)r=1.0;
                if(g<0.0) g=0.0; else if(g>1.0)g=1.0;
                if(b<0.0) b=0.0; else if(b>1.0)b=1.0;
                
				imp::Pixel px = {(unsigned int)(r*255),(unsigned int)(g*255),(unsigned int)(b*255),255};
				img.setPixel(i,j,px);
			}
		}
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

class DilationFunctor : public Functor
{
	public:
	DilationFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
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
            { 1.0,      1.0,      1.0 },
            { 1.0,      1.0,      1.0 },
            { 1.0,      1.0,      1.0 }
        };
        
        
		bool inputReady = false;
		imp::ImageData* input = getImage(json, "inputID", inputReady);
		if( input && !inputReady ) return false;
        
		double force = json->getNumeric("force")->value;
        
        for(unsigned int i = 0; i<img.getWidth(); ++i)
        {
            for(unsigned int j=0; j<img.getHeight(); ++j)
            {
                
                double v=  0.0;
                
                for(int fi = -1; fi<=1; ++fi)
                {
                    for(int fj=-1; fj<=1; ++fj)
                    {
                        v += input->getRepeatPixel((int)i+fi,(int)j+fj).red/255.0 * (hpfilter[fi+1][fj+1] *force);
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

class BinarizationFunctor : public Functor
{
	public:
	BinarizationFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool inputReady = false;
		imp::ImageData* input = getImage(json, "inputID", inputReady);
		if( input && !inputReady ) return false;
        
		double threshold = json->getNumeric("threshold")->value;
        
        for(unsigned int i = 0; i<img.getWidth(); ++i)
        {
            for(unsigned int j=0; j<img.getHeight(); ++j)
            {
                
                double v=  input->getRepeatPixel(i,j).red/255.0;
                
                if(v<threshold)
                    v = 0.0;
                else
                    v = 1.0;
                
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

class AdjustmentsFunctor : public Functor
{
	public:
	AdjustmentsFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
	virtual bool apply(imp::JsonObject* json, imp::ImageData& img)
	{
		bool inputReady = false;
		imp::ImageData* input = getImage(json, "inputID", inputReady);
		if( input && !inputReady ) return false;
        
		double contrast = json->getNumeric("contrast")->value;
		double brightness = json->getNumeric("brightness")->value;
        
        for(unsigned int i = 0; i<img.getWidth(); ++i)
        {
            for(unsigned int j=0; j<img.getHeight(); ++j)
            {
                
                double r=  input->getRepeatPixel(i,j).red/255.0;
                double g=  input->getRepeatPixel(i,j).green/255.0;
                double b=  input->getRepeatPixel(i,j).blue/255.0;
                
                r = contrast*(r-0.5) + 0.5 + brightness;
                g = contrast*(g-0.5) + 0.5 + brightness;
                b = contrast*(b-0.5) + 0.5 + brightness;
                
                if(r<0.0) r = 0.0; else if(r>1.0) r = 1.0;
                if(g<0.0) g = 0.0; else if(g>1.0) g = 1.0;
                if(b<0.0) b = 0.0; else if(b>1.0) b = 1.0;
                
                imp::Pixel pixel;
                pixel.red = (unsigned char)(r * 255);
                pixel.green = (unsigned char)(g * 255);
                pixel.blue = (unsigned char)(b * 255);
                img.setPixel(i,j,pixel);
            }
        }
        
		return true;
	}
};

class SobelFunctor : public Functor
{
	public:
	SobelFunctor(FilterMap* filters, DistribMap* distribs) : Functor(filters, distribs){}
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
        /*const double  hpfilter[3][3] = 
        {
            { -1.0,      -1.0,      -1.0 },
            { -1.0,     9.0,        -1.0 },
            { -1.0,      -1.0,      -1.0 }
        };*/
        
        const double  vfilter[3][3] = 
        {
            { -1.0,     0.0,      1.0 },
            { -2.0,     0.0,      2.0 },
            { -1.0,     0.0,      1.0 }
        };
        const double  hfilter[3][3] = 
        {
            { -1.0,     -2.0,      1.0 },
            { 0.0,     0.0,      0.0 },
            { 1.0,     2.0,      1.0 }
        };
        
        
		bool inputReady = false;
		imp::ImageData* input = getImage(json, "inputID", inputReady);
		if( input && !inputReady ) return false;
        
        
        for(unsigned int i = 1; i<img.getWidth()-1; ++i)
        {
            for(unsigned int j=1; j<img.getHeight()-1; ++j)
            {
                
                double v=  0.0;
                double h=  0.0;
                
                for(int fi = -1; fi<=1; ++fi)
                {
                    for(int fj=-1; fj<=1; ++fj)
                    {
                        v += input->getPixel(i+fi,j+fj).red/255.0 * vfilter[fi+1][fj+1] ;
                        h += input->getPixel(i+fi,j+fj).red/255.0 * hfilter[fi+1][fj+1] ;
                    }
                }
                
                v = (v+h)/2.0;
                
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
    functors["sobel"] = new SobelFunctor(&filters, &distribs);
    functors["dilation"] = new DilationFunctor(&filters, &distribs);
    functors["binarization"] = new BinarizationFunctor(&filters, &distribs);
    functors["adjustments"] = new AdjustmentsFunctor(&filters, &distribs);
    functors["add"] = new AddFunctor(&filters, &distribs);

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
















// -----------------------
double distance2(double x, double y, double x2, double y2)
{
    double dx = (x2 - x);
    double dy = (y2 - y);
    return dx*dx + dy*dy;
}



// -----------------------
void find2Closest(double* xcell, double* ycell, int count, double x, double y, double& fx, double& fy, double& fx2, double& fy2)
{
    double d1 = -1.0;
    double d2 = -1.0;
    
    for(int cell=0; cell<count; ++cell)
    {
        
        double dx = xcell[cell];
        double dy = ycell[cell];
        double d = distance2(dx, dy,x,y);
        
        if(d1<0.0 || d < d1)
        {
            d2 = d1;
            fx2 = fx;
            fy2 = fy;
            
            d1 = d;
            fx = dx;
            fy = dy;
        }
        else if(d2<0.0 || d < d2)
        {
            d2 = d;
            fx2 = dx;
            fy2 = dy;
        }
    }
}

// -----------------------
double voronoi(double* xcell, double* ycell, int count, double x, double y)
{
    double dx1 = 0.0;
    double dy1 = 0.0;
    double dx2 = 0.0;
    double dy2 = 0.0;
    find2Closest(xcell, ycell, count, x, y,
                        dx1, dy1,
                        dx2, dy2);
    
    double f1 = sqrt(distance2(dx1,dy1,x,y));
    double f2 = sqrt(distance2(dx2,dy2,x,y));
    
    double result = f1/(f2+f1);
    
    return result;//>0.4?1.0:0.0;
}












void applyBilinearInterpo(ImageData& bitmap, float frqX, float frqY)
{
	ImageData temp;
	temp.clone(bitmap);

	for(std::uint32_t i=0; i<bitmap.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<bitmap.getHeight(); ++j)
		{
			float periodX = bitmap.getWidth() / frqX;
			float periodY = bitmap.getHeight() / frqY;

			int x0 = floor(i/periodX) * periodX;
			int y0 = floor(j/periodY) * periodY;
			int x1 = x0 + periodX;
			int y1 = y0 + periodY;

			float dx = frac(i/periodX);
			float dy = frac(j/periodY);

			Vec4 oo = temp.getPixel(x0, y0);
			Vec4 xo = temp.getPixel(x1, y0);
			Vec4 oy = temp.getPixel(x0, y1);
			Vec4 xy = temp.getPixel(x1, y1);

			Vec4 color = bilerp(oo, xo, oy, xy, dx, dy);
			//Vec4 color = bismooth(oo, xo, oy, xy, dx, dy);

			bitmap.setPixel(i,j, color);
		}
	}
}

void blend(ImageData& dst, ImageData& src, float alpha)
{
	for(std::uint32_t i=0; i<dst.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<dst.getHeight(); ++j)
		{
			Vec4 srcColor, dstColor;
			srcColor = src.getPixel(i,j);
			dstColor = dst.getPixel(i,j);

			dstColor = lerp(dstColor, srcColor, alpha);
			dst.setPixel(i,j,dstColor);
		}
	}
}

void blend(ImageData& dst, ImageData& src, ImageData& alphaMap, double threshold)
{
   	for(std::uint32_t i=0; i<dst.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<dst.getHeight(); ++j)
		{
			Vec4 srcColor, dstColor;
			srcColor = src.getPixel(i,j);
			dstColor = dst.getPixel(i,j);
            
            double alpha = (double)(alphaMap.getPixel(i,j)[0]) / 255.0;
            /*if(alpha > threshold)
                dstColor = srcColor;*/
            
			dstColor = lerp(dstColor, srcColor, alpha);
			dst.setPixel(i,j,dstColor);
		}
	}
}

void heightToNormal(ImageData& in, ImageData& out, float force, float prec)
{
	int dist = 1.0;// prec * 1.0;
    force = 1.0;

	out.clone(in); 

	for(std::uint32_t i=0; i<in.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<in.getHeight(); ++j)
		{

			float h1 = (float)(in.getPixel(i-dist,j)[0])/255.0;
			float h2 = (float)(in.getPixel(i+dist,j)[0])/255.0;
			float h3 = (float)(in.getPixel(i,j-dist)[0])/255.0;
			float h4 = (float)(in.getPixel(i,j+dist)[0])/255.0;

			Vec3 nx, ny, normal;

			nx[0] = 2.0;
			nx[1] = 0.0;
			nx[2] = (h2-h1) * force;

			ny[0] = 0.0;
			ny[1] = 2.0;
			ny[2] = (h4-h3) * force;

			cross(nx, ny, normal);
			normalize(normal);

			normal[0] += 1.0; normal[0] /= 2.0;
			normal[1] += 1.0; normal[1] /= 2.0;
			normal[2] += 1.0; normal[2] /= 2.0;

			Vec4 pixel;
			pixel[2] = (unsigned char)(normal[0] * 255); // red
			pixel[1] = (unsigned char)(normal[1] * 255); //green
			pixel[0] = (unsigned char)(normal[2] * 255); // blue
            pixel[3] = 255;
			out.setPixel(i,j,pixel);
		}
	}
}

inline Vec4 mirGet(imp::ImageData& img, unsigned int x, unsigned int y)
{
	if(x < 0) x = -x;
	else if(x>=img.getWidth()) x = img.getWidth() - (x-img.getWidth());
	if(y < 0) y= -y;
	else if(y>=img.getHeight()) y = img.getHeight() - (y-img.getHeight());

	return img.getPixel(x,y);
}

void drawDirectionnalSinus(imp::ImageData& img, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio,
	const imp::ImageData* perturbation, float perturbIntensity)
{
	float diag = sqrtf(img.getHeight()/2.0*img.getHeight()/2.0 + img.getWidth()/2.0*img.getWidth()/2.0);

	float dirLength = sqrtf(dirX*dirX + dirY*dirY);
	dirX /= dirLength;
	dirY /= dirLength;

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float dirDot = (i*dirX + j*dirY);
		float t = dirDot;

		if(perturbation != NULL)
			t += perturbIntensity * perturbation->getPixel(i,j)[0];//imp::perlinMain(x/64,y/64,perturb/64);

		float v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		Vec4 px(comp,comp,comp,255);
		img.setPixel(i,j,px);
	}}
}

void drawDirectionnalSignal(imp::ImageData& img, SignalType signalType, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio,
	const imp::ImageData* perturbation, float perturbIntensity)
{
	float diag = sqrtf(img.getHeight()/2.0*img.getHeight()/2.0 + img.getWidth()/2.0*img.getWidth()/2.0);

	float dirLength = sqrtf(dirX*dirX + dirY*dirY);
	dirX /= dirLength;
	dirY /= dirLength;

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float dirDot = (i*dirX + j*dirY);
		float t = dirDot;

		if(perturbation != NULL)
			t += perturbIntensity * perturbation->getPixel(i,j)[0];//imp::perlinMain(x/64,y/64,perturb/64);

		float v = 0;
		if(signalType == SignalType_Sinus)
			v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		else if(signalType == SignalType_Square)
			v = imp::SquareSignal(t/diag * 3.141592 * freq, maxPeriodRatio);
		else if(signalType == SignalType_Triangle)
			v = imp::TriangleSignal(t/diag * 3.141592 * freq, maxPeriodRatio);

		unsigned int comp = (v+1.0)/2.0 * ampl;
		Vec4 px(comp,comp,comp,255);
		img.setPixel(i,j,px);
	}}
}

void drawRadialSinus(imp::ImageData& img, double posX, double posY, float freq, float ampl, float maxPeriodRatio,
	const imp::ImageData* perturbation, float perturbIntensity)
{
	float diag = sqrtf(img.getHeight()/2.0*img.getHeight()/2.0 + img.getWidth()/2.0*img.getWidth()/2.0);

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float distX = abs(i - posX);
		float distY = abs(j - posY);
		float t = sqrtf(distX*distX + distY*distY);

		if(perturbation != NULL)
			t += perturbIntensity * perturbation->getPixel(i,j)[0];//imp::perlinMain(x/64,y/64,perturb/64);

		float v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		Vec4 px(comp,comp,comp,255);
		img.setPixel(i,j,px);
	}}
}

void applyPerturbation(imp::ImageData& img, const imp::ImageData& normals, float intensity)
{
	imp::ImageData cloned;
	cloned.clone(img);

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float depX = (normals.getPixel(i,j)[0])/255.0;
		float depY = (normals.getPixel(i,j)[1])/255.0;

		depX = depX*2.0 - 1.0;
		depY = depY*2.0 - 1.0;


		Vec4 px = mirGet(cloned, i+depX*intensity, j+depY*intensity);

		img.setPixel(i,j,px);
	}}
}

void applyColorization(imp::ImageData& img, const imp::Vec4& color1, const imp::Vec4& color2)
{
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		imp::Vec4 px = img.getPixel(i,j);

		px[0] = (unsigned char)((px[0]/255.0) * ((double)color2[0]-(double)color1[0]) + (double)color1[0]);
		px[1] = (unsigned char)((px[1]/255.0) * ((double)color2[1]-(double)color1[1]) + (double)color1[1]);
		px[2] = (unsigned char)((px[2]/255.0) * ((double)color2[2]-(double)color1[2]) + (double)color1[2]);

		img.setPixel(i,j,px);
	}}
}

void applyColorization(imp::ImageData& img, const imp::Vec4& color1, const imp::Vec4& color2, Distribution& distrib)
{
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		imp::Vec4 px = img.getPixel(i,j);

		double t1 = (px[0]/255.0);
		double t2 = (px[1]/255.0);
		double t3 = (px[2]/255.0);

		t1 = distrib(t1);
		t2 = distrib(t2);
		t3 = distrib(t3);

		px[0] = (unsigned char)(t1 * ((double)color2[0]-(double)color1[0]) + (double)color1[0]);
		px[1] = (unsigned char)(t2 * ((double)color2[1]-(double)color1[1]) + (double)color1[1]);
		px[2] = (unsigned char)(t3 * ((double)color2[2]-(double)color1[2]) + (double)color1[2]);

		img.setPixel(i,j,px);
	}}
}

void applyMaximization(imp::ImageData& img)
{
	std::uint8_t maxR = 0;
	std::uint8_t minR = 255;

	// scan
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		std::uint8_t r = img.getPixel(i,j)[0];
		if(r < minR)
			minR = r;
		else if(r > maxR)
			maxR = r;
	}}

	// maximize
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		double t = double(img.getPixel(i,j)[0]);
		std::uint8_t newt = (std::uint8_t)Lerp( 0.0, 255.0, (t-minR)/(maxR-minR) );
		Vec4 px(newt,newt,newt, 255);
		img.setPixel(i,j,px);
	}}
}

void drawCellularNoise(imp::ImageData& img, unsigned int cellcount, const imp::ImageData* noisemap)
{
    double xcell[cellcount*cellcount];
    double ycell[cellcount*cellcount];

    double fieldW = (noisemap->getWidth()/cellcount);
    double fieldH = (noisemap->getHeight()/cellcount);
    
    // initialize cell centers
    for(unsigned int i=0; i<cellcount; ++i)
    for(unsigned int j=0; j<cellcount; ++j)
    {
        
        double x = (double)i * fieldW;
        double y = (double)j * fieldH;
        
        xcell[j*cellcount+i] = x + double(noisemap->getPixel(x,y)[0])/255.0 * fieldW;
        ycell[j*cellcount+i] = y + double(noisemap->getPixel(x,y)[0])/255.0 * fieldH;
        
        std::cout << "x=" << xcell[j*cellcount+i] << "; y=" << ycell[j*cellcount+i] << std::endl;
    }
    
    // draw
    for(unsigned int i=0; i<img.getWidth(); ++i)
    {
        for(unsigned int j=0; j<img.getHeight(); ++j)
        {
            // find closest cell center
            double d = -1.0;
            for(unsigned int cell=0; cell<cellcount*cellcount; ++cell)
            {
                
                double dx = xcell[cell] - (double)i;
                double dy = ycell[cell] - (double)j;
                double d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                
                // ---- tiling effect -----
                dx = (xcell[cell]-img.getWidth()) - (double)i;
                dy = (ycell[cell]-img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                dx = (xcell[cell]+img.getWidth()) - (double)i;
                dy = (ycell[cell]-img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                dx = (xcell[cell]-img.getWidth()) - (double)i;
                dy = (ycell[cell]+img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                dx = (xcell[cell]+img.getWidth()) - (double)i;
                dy = (ycell[cell]+img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                // -------------------
            }
            
            // apply new value
            const double maxR = (fieldW)*(fieldW);
            std::uint8_t newt = (std::uint8_t)Lerp( 0.0, 255.0, d/maxR);
            Vec4 px(newt,newt,newt, 255);
            img.setPixel(i,j,px);
        }
    }
    
}






