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

	Filter(imp::ImageData* img, imp::JsonObject* json)
	{
		data = img;
		this->json = json;
		ready = false;
	}

	~Filter(){}
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

imp::Pixel toPixel(imp::JsonArray* arr)
{
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
		isReady = filters[id]->ready;
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

void makeFilter(Filter* filter)
{
	imp::JsonObject* json = filter->json;
	imp::ImageData& img = *(filter->data);
	std::cout << "make filter " << json->getString("id")->value << "\n";

	if( json->getString("type")->value == "sinus" )
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return;

		double dirX = json->getNumeric("dirX")->value;
		double dirY = json->getNumeric("dirY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		if( json->getNumeric("ratio") )
			ratio = json->getNumeric("ratio")->value;

		drawDirectionnalSinus(img, dirX, dirY, freq, ampl, ratio, perturbation, perturbIntensity);
		filter->ready = true;
	}
	else if( json->getString("type")->value == "square" )
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return;

		double dirX = json->getNumeric("dirX")->value;
		double dirY = json->getNumeric("dirY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		if( json->getNumeric("ratio") )
			ratio = json->getNumeric("ratio")->value;

		drawDirectionnalSignal(img, imp::SignalType_Square, dirX, dirY, freq, ampl, ratio, perturbation, perturbIntensity);
		filter->ready = true;
	}
	else if( json->getString("type")->value == "triangle" )
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return;

		double dirX = json->getNumeric("dirX")->value;
		double dirY = json->getNumeric("dirY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		if( json->getNumeric("ratio") )
			ratio = json->getNumeric("ratio")->value;

		drawDirectionnalSignal(img, imp::SignalType_Triangle, dirX, dirY, freq, ampl, ratio, perturbation, perturbIntensity);
		filter->ready = true;
	}
	else if(json->getString("type")->value == "noise")
	{
		double frequency = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double octaveCount = json->getNumeric("octaves")->value;
		double persistence = json->getNumeric("persistence")->value;
		
		double tiles = 1.0;
		if(json->getNumeric("tiles"))
		{
			tiles = json->getNumeric("tiles")->value;
		}

		for(unsigned int i=0; i<img.getWidth(); ++i)
		{
			for(unsigned int j=0; j<img.getHeight(); ++j)
			{
				double v = imp::perlinOctave(((double)i)/img.getWidth(), ((double)j)/img.getHeight(), 0, octaveCount, persistence, frequency, 1.0/tiles);
				imp::Uint8 comp = (v+1.0)/2.0 * ampl;
				imp::Pixel px = {comp,comp,comp,255};
				img.setPixel(i,j,px);
			}
		}
		filter->ready = true;
	}
	else if(json->getString("type")->value == "heightToNormal")
	{
		bool perturbReady = false;
		imp::ImageData* height = getImage(json, "heightmapID", perturbReady);

		if(height && perturbReady)
		{
			double force = json->getNumeric("force")->value;
			double prec = json->getNumeric("prec")->value;
			imp::heightToNormal(*height, img, force, prec);
			filter->ready = true;
		}
	}
	else if(json->getString("type")->value == "colorization")
	{
		bool perturbReady = false;
		imp::ImageData* grey = getImage(json, "greymapID", perturbReady);

		if(grey && perturbReady)
		{
			bool hasDistrib = json->getString("distribID") != NULL;

			img.clone(*grey);

			imp::Pixel color1 = toPixel(json->getArray("color1"));
			imp::Pixel color2 = toPixel(json->getArray("color2"));

			if(hasDistrib)
				applyColorization(img, color1, color2, getDistrib(json, "distribID"));
			else
				applyColorization(img, color1, color2);

			filter->ready = true;
		}
	}
	else if(json->getString("type")->value == "radSinus")
	{
		bool perturbReady = false;
		imp::ImageData* perturbation = getImage(json, "perturbMapID", perturbReady);

		if( perturbation && !perturbReady )
			return;

		double posX = json->getNumeric("posX")->value;
		double posY = json->getNumeric("posY")->value;
		double freq = json->getNumeric("freq")->value;
		double ampl = json->getNumeric("ampl")->value;
		double perturbIntensity = json->getNumeric("perturb")->value;

		double ratio = 1.0;
		if( json->getNumeric("ratio") )
			ratio = json->getNumeric("ratio")->value;

		imp::drawRadialSinus(img, posX, posY, freq, ampl, ratio, perturbation, perturbIntensity);
		filter->ready = true;

	}
	else if(json->getString("type")->value == "blend")
	{
		bool srcReady = false;
		imp::ImageData* src = getImage(json, "srcID", srcReady);
		bool dstReady = false;
		imp::ImageData* dst = getImage(json, "dstID", dstReady);


		if(src && dst && srcReady && dstReady)
		{
			img.clone(*dst);
			double alpha = json->getNumeric("alpha")->value;
			imp::blend(img, *src, alpha);
			filter->ready = true;
		}
	}
	else if(json->getString("type")->value == "maximization")
	{
		bool ready = false;
		imp::ImageData* grey = getImage(json, "greymapID", ready);

		if(grey && ready)
		{
			img.clone(*grey);
			imp::applyMaximization(img);
			filter->ready = true;
		}
	}
	else
	{
		std::cout << "impError : type unsupported - " << json->getString("type")->value << "\n";
	}
}

void generateFilters(const std::string& json)
{
	std::string jsonWithoutSpace = imp::removeSpace(json);
	imp::JsonObject* root = dynamic_cast<imp::JsonObject*>( imp::parse(jsonWithoutSpace) );
	imp::JsonArray* array = root->getArray("filters");
	imp::JsonArray* distribArr = root->getArray("utils");

	double width = root->getNumeric("resolutionX")->value;
	double height = root->getNumeric("resolutionY")->value;
	
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
				makeFilter(filters[id]);
				if(filters[id]->ready)
					readyCount++;
			}
		}
	}

	if(previousReadyCount == readyCount)
	{
		std::cout << "impError : loop detected. One or more filter is not ready\n";
	}

	for(unsigned int i=0; i<array->value.size(); ++i)
	{
		imp::JsonObject* filter = array->getObject(i);
		std::string id = filter->getString("id")->value;
		BmpLoader::saveToFile(filters[id]->data, (id+".bmp").c_str());
	}
}

// void test();

int main(int argc, char* argv[])
{
	generateFilters( loadJson("procedure.json") );
	return 0;
}
