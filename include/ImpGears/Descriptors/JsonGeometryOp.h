#ifndef IMP_JSON_GEOMETRYOP_H
#define IMP_JSON_GEOMETRYOP_H

#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/Descriptors/JsonReaderWriter.h>
#include <ImpGears/Descriptors/Export.h>

#include <map>

IMPGEARS_BEGIN

//-------------------------------------------------------
void IG_DESC_API generateGeometryFromJson(const std::string filename, std::map<std::string,Geometry>& geo_out);

//-------------------------------------------------------
struct IG_DESC_API GeoOperation : public Object
{
	Meta_Class(GeoOperation);
	
	static std::map<std::string, Geometry> s_finished;
	static float s_subdiv;
	
	std::vector<std::string> _deps;
	std::string _opName;
	bool _done;
	
	GeoOperation(const std::string& opName);
	void addDepend(const std::string& name);
	virtual std::string tryString(JsonObject* obj, const std::string& optname);
	virtual Vec3 tryVec3(JsonObject* obj, const std::string& optname);
	virtual Vec4 tryVec4(JsonObject* obj, const std::string& optname);
	virtual float tryNumeric(JsonObject* obj, const std::string& optname);
	bool isReady() const;
	virtual bool load(JsonObject* jo) = 0;
	virtual void apply() = 0;
};

//-------------------------------------------------------
GeoOperation::Ptr IG_DESC_API createFromJsonGeoObj(JsonObject* obj);

//-------------------------------------------------------
struct IG_DESC_API GeometryGenerator : public GeoOperation
{
	Meta_Class(GeometryGenerator);
	GeometryGenerator(const std::string& name) : GeoOperation(name), _id("no_name") {}

	bool load(JsonObject* jo)
	{
		_id = tryString(jo,"id");
		return true;
	}

	void apply()
	{
		if( !isReady() ) return;
		s_finished[_id] = generate();
		_done = true;
	}
	virtual Geometry generate() = 0;
	
	std::string _id;
};

//-------------------------------------------------------
struct IG_DESC_API GeometryTransform : public GeometryGenerator
{
	Meta_Class(GeometryTransform);
	GeometryTransform(const std::string& name) : GeometryGenerator(name), _input_id("no_name") {}

	bool load(JsonObject* jo)
	{
		_input_id = GeometryGenerator::tryString(jo,"input_id");
		GeometryGenerator::addDepend(_input_id);
		return GeometryGenerator::load(jo);
	}
	
	virtual Geometry generate(Geometry& in) = 0;

	virtual Geometry generate()
	{
		Geometry input = GeometryGenerator::s_finished[_input_id];
		return generate(input);
	}
	
	std::string _input_id;
};

//-------------------------------------------------------
struct IG_DESC_API JsonCylinderOperation : public GeometryGenerator
{
	Meta_Class(JsonCylinderOperation);
	JsonCylinderOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate();
	
	float _subdiv;
	float _length;
	float _radius;
};

//-------------------------------------------------------
struct IG_DESC_API JsonConeOperation : public GeometryGenerator
{
	Meta_Class(JsonConeOperation);
	JsonConeOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate();
	
	float _subdiv;
	float _length;
	float _radius1;
	float _radius2;
};

//-------------------------------------------------------
struct IG_DESC_API JsonCubeOperation : public GeometryGenerator
{
	Meta_Class(JsonCubeOperation);
	JsonCubeOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate();
	
	float _size;
};

//-------------------------------------------------------
struct IG_DESC_API JsonSphereOperation : public GeometryGenerator
{
	Meta_Class(JsonSphereOperation);
	JsonSphereOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate();
	
	float _subdiv;
	float _radius;
};

//-------------------------------------------------------
struct IG_DESC_API JsonTranslateOperation : public GeometryTransform
{
	Meta_Class(JsonTranslateOperation);
	JsonTranslateOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate(Geometry& in);
	
	Vec3 _translation;
};

//-------------------------------------------------------
struct IG_DESC_API JsonAddOperation : public GeometryTransform
{
	Meta_Class(JsonAddOperation);
	JsonAddOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate(Geometry& in);
	
	std::string _in2;
};

//-------------------------------------------------------
struct IG_DESC_API JsonColorOperation : public GeometryTransform
{
	Meta_Class(JsonColorOperation);
	JsonColorOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate(Geometry& in);
	
	Vec4 _color;
};

//-------------------------------------------------------
struct IG_DESC_API JsonTexCoordsOperation : public GeometryTransform
{
	Meta_Class(JsonTexCoordsOperation);
	JsonTexCoordsOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate(Geometry& in);
	
	float _factor;
};

//-------------------------------------------------------
struct IG_DESC_API JsonNormalsOperation : public GeometryTransform
{
	Meta_Class(JsonNormalsOperation);
	JsonNormalsOperation();
	virtual bool load(JsonObject* jo);
	virtual Geometry generate(Geometry& in);
};

IMPGEARS_END


#endif // IMP_JSON_GEOMETRYOP_H
