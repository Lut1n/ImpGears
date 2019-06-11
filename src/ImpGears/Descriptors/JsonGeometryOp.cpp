#include <Descriptors/JsonGeometryOp.h>
#include <Descriptors/FileInfo.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <map>

IMPGEARS_BEGIN

//-------------------------------------------------------
GeoOperation::GeoOperation(const std::string& opName) : _opName(opName), _done(false) {}

//-------------------------------------------------------
void GeoOperation::addDepend(const std::string& name)
{
	_deps.push_back(name);
}

//-------------------------------------------------------
std::string GeoOperation::tryString(JsonObject* obj, const std::string& optname)
{
	if(obj->getString(optname))
		return obj->getString(optname)->value;
	else
		std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
	return std::string();
}

//-------------------------------------------------------
float GeoOperation::tryNumeric(JsonObject* obj, const std::string& optname)
{
	if(obj->getNumeric(optname))
		return obj->getNumeric(optname)->value;
	else
		std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
	return 0.0;
}

//-------------------------------------------------------
Vec3 GeoOperation::tryVec3(JsonObject* obj, const std::string& optname)
{
	Vec3 res;
	JsonArray* array = obj->getArray(optname);
	if(array != nullptr)
	{
		int s = std::min((int)3,(int)array->value.size());
		for(int i=0;i<s;++i)
			if(array->getNumeric(i))
				res[i] = array->getNumeric(i)->value;
	}
	else
	{
		std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
	}
		
	return res;
}

//-------------------------------------------------------
Vec4 GeoOperation::tryVec4(JsonObject* obj, const std::string& optname)
{
	Vec3 res;
	JsonArray* array = obj->getArray(optname);
	if(array != nullptr)
	{
		int s = std::min((int)4,(int)array->value.size());
		for(int i=0;i<s;++i)
			if(array->getNumeric(i))
				res[i] = array->getNumeric(i)->value;
	}
	else
	{
		std::cerr << "error in " << _opName << " : cannot read option " << optname << std::endl;
	}
		
	return res;
}

//-------------------------------------------------------
bool GeoOperation::isReady() const
{
	for(auto d : _deps)
		if(s_finished.find(d)==s_finished.end())
			return false;
	return true;
}

//-------------------------------------------------------
std::map<std::string,Geometry> GeoOperation::s_finished;
float GeoOperation::s_subdiv = 1.0;

//-------------------------------------------------------
using OperationCache = std::vector<GeoOperation::Ptr>;

//-------------------------------------------------------
void generateGeometryFromJson(const std::string filename, std::map<std::string,Geometry>& geo_out)
{
	std::cout << "loading json data..." << std::endl;
	OperationCache opCache;
	std::string json = loadText(filename);
	std::string jsonWithoutSpace = removeSpace(json);
	
	JsonObject* root = dynamic_cast<JsonObject*>( parse(jsonWithoutSpace) );
	JsonArray* array = root->getArray("operations");

	JsonObject* prop = root->getObject("properties");
	GeoOperation::s_subdiv = prop->getNumeric("subdiv")->value;
	
	int s = array->value.size();
	for(int i=0; i<s; ++i)
	{
		JsonObject* jo = array->getObject(i);
		GeoOperation::Ptr op = createFromJsonGeoObj(jo);
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
	
	geo_out = GeoOperation::s_finished;
}

//-------------------------------------------------------
JsonCylinderOperation::JsonCylinderOperation() : GeometryGenerator("cylinder") {}

//-------------------------------------------------------
bool JsonCylinderOperation::load(JsonObject* jo)
{
	_subdiv = tryNumeric(jo,"subdiv");
	_length = tryNumeric(jo,"length");
	_radius = tryNumeric(jo,"radius");
	return GeometryGenerator::load(jo);
}
Geometry JsonCylinderOperation::generate() { return Geometry::cylinder(s_subdiv*_subdiv, _length, _radius); }

//-------------------------------------------------------
JsonConeOperation::JsonConeOperation() : GeometryGenerator("cone") {}

//-------------------------------------------------------
bool JsonConeOperation::load(JsonObject* jo)
{
	_subdiv = tryNumeric(jo,"subdiv");
	_length = tryNumeric(jo,"length");
	_radius1 = tryNumeric(jo,"radius1");
	_radius2 = tryNumeric(jo,"radius2");
	return GeometryGenerator::load(jo);
}

//-------------------------------------------------------
Geometry JsonConeOperation::generate()
{
	return Geometry::cone(s_subdiv*_subdiv, _length, _radius1, _radius2);
}

//-------------------------------------------------------
JsonCubeOperation::JsonCubeOperation() : GeometryGenerator("cube") {}

//-------------------------------------------------------
bool JsonCubeOperation::load(JsonObject* jo)
{
	_size = tryNumeric(jo,"size");
	return GeometryGenerator::load(jo);
}

//-------------------------------------------------------
Geometry JsonCubeOperation::generate()
{
	Geometry res = Geometry::cube();
	res.scale(_size);
	return res;
}

//-------------------------------------------------------
JsonSphereOperation::JsonSphereOperation() : GeometryGenerator("sphere") {}

//-------------------------------------------------------
bool JsonSphereOperation::load(JsonObject* jo)
{
	_subdiv = tryNumeric(jo,"subdiv");
	_radius = tryNumeric(jo,"radius");
	return GeometryGenerator::load(jo);
}

//-------------------------------------------------------
Geometry JsonSphereOperation::generate()
{
	return Geometry::sphere(s_subdiv*_subdiv, _radius);
}


//-------------------------------------------------------
JsonTranslateOperation::JsonTranslateOperation() : GeometryTransform("translation") {}

//-------------------------------------------------------
bool JsonTranslateOperation::load(JsonObject* jo)
{
	_translation = tryVec3(jo,"translation");
	return GeometryTransform::load(jo);
}

//-------------------------------------------------------
Geometry JsonTranslateOperation::generate(Geometry& in)
{
	Geometry res = in;
	res += _translation;
	return res;
}

//-------------------------------------------------------
JsonAddOperation::JsonAddOperation() : GeometryTransform("add") {}

//-------------------------------------------------------
bool JsonAddOperation::load(JsonObject* jo)
{
	_in2 = tryString(jo,"input_id2");
	addDepend(_in2);
	return GeometryTransform::load(jo);
}

//-------------------------------------------------------
Geometry JsonAddOperation::generate(Geometry& in)
{
	Geometry res = in;
	res += s_finished[_in2];
	return res;
}

//-------------------------------------------------------
JsonColorOperation::JsonColorOperation() : GeometryTransform("color") {}

//-------------------------------------------------------
bool JsonColorOperation::load(JsonObject* jo)
{
	_color = tryVec4(jo,"color");
	return GeometryTransform::load(jo);
}

//-------------------------------------------------------
Geometry JsonColorOperation::generate(Geometry& in)
{
	Geometry res = in;
	res.generateColors(_color);
	return res;
}

//-------------------------------------------------------
JsonTexCoordsOperation::JsonTexCoordsOperation() : GeometryTransform("texCoords") {}

//-------------------------------------------------------
bool JsonTexCoordsOperation::load(JsonObject* jo)
{
	_factor = tryNumeric(jo,"factor");
	return GeometryTransform::load(jo);
}

//-------------------------------------------------------
Geometry JsonTexCoordsOperation::generate(Geometry& in)
{
	Geometry res = in;
	res.generateTexCoords( Geometry::TexGenMode_Cubic, _factor);
	return res;
}

//-------------------------------------------------------
JsonNormalsOperation::JsonNormalsOperation() : GeometryTransform("normals") {}

//-------------------------------------------------------
bool JsonNormalsOperation::load(JsonObject* jo)
{
	return GeometryTransform::load(jo);
}

//-------------------------------------------------------
Geometry JsonNormalsOperation::generate(Geometry& in)
{
	Geometry res = in;
	res.generateNormals();
	return res;
}

//-------------------------------------------------------
GeoOperation::Ptr createFromJsonGeoObj(JsonObject* obj)
{
	GeoOperation::Ptr res;
	
	std::string opname = obj->getString("op")->value;
	if(opname == "cylinder") res = JsonCylinderOperation::create();
	else if(opname == "cone") res = JsonConeOperation::create();
	else if(opname == "cube") res = JsonCubeOperation::create();
	else if(opname == "sphere") res = JsonSphereOperation::create();
	else if(opname == "translate") res = JsonTranslateOperation::create();
	else if(opname == "add") res = JsonAddOperation::create();
	else if(opname == "color") res = JsonColorOperation::create();
	else if(opname == "texcoords") res = JsonTexCoordsOperation::create();
	else if(opname == "normals") res = JsonNormalsOperation::create();
	
	if(res == nullptr)
		std::cerr << "cannot load operation " << opname << std::endl;
	else
		res->load(obj);
	return res;
}

IMPGEARS_END
