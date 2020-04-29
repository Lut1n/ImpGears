

enum GeoPrimitive
{
    Cube,
    Sphere,
    Cylinder,
    Capsule,
    Crystal,
    Torus
};

#define CASE_TO_STRING(case_name,in_case,out_string) if(in_case==case_name)out_string=#case_name;
#define STRING_TO_CASE(case_name,in_string,out_case) if(in_string==#case_name)out_case=case_name;


struct ModelState
{
   Vec3 position;
   Vec3 rotation;
   Vec3 scale;
   Vec3 color;
   GeoPrimitive primitive;
   
   ModelState()
   {
       scale = Vec3(1.0);
       color = Vec3(1.0);
   }
};


struct ViewState
{
   Vec3 camera_rad;
   
   ViewState() : camera_rad(0.25,0.25,10.0)
   {
   }
};


std::ostream& operator<<(std::ostream& ss, const Vec3& v)
{
    ss << "[" << v[0]  << "," << v[1]  << "," << v[2] << "]";
    return ss;
}


void saveToFile(const std::string& filename, const std::string& content)
{
    std::ofstream of(filename);
    of.write(content.c_str(), content.size());
}


std::string tab(int n)
{
    return std::string(n,'\t');
}

void wrap(std::string& s, char c)
{
    s.insert(0,1,c); s.push_back(c);
}

void save(const std::string& filename, const std::vector<ModelState>& models, const ViewState& view)
{
    std::stringstream ss;
    
    {
        ss << "{" << std::endl;
    
        
         ss << tab(1) << "\"editor_view\":"<< std::endl;
        {
            ss << tab(1) << "{" << std::endl;
            ss << tab(2) << "\"camera_rad\":" << view.camera_rad << std::endl;
            ss << tab(1) << "}," << std::endl;
        }
        
        
         ss  << tab(1)<< "\"models\":" << std::endl;
        {
            ss  << tab(1) << "["  << std::endl;
            for(auto m : models)
            {
                ss  << tab(2) << "{" ;
                std::string p;
                CASE_TO_STRING(Cube,m.primitive,p);
                CASE_TO_STRING(Sphere,m.primitive,p);
                CASE_TO_STRING(Cylinder,m.primitive,p);
                CASE_TO_STRING(Capsule,m.primitive,p);
                CASE_TO_STRING(Crystal,m.primitive,p);
                CASE_TO_STRING(Torus,m.primitive,p);
                wrap(p,'"');

                // = m.primitive==Cube ? "\"cube\"" : "\"sphere\"";
                ss << "\"primitive\":" << p << ",";
                ss << "\"position\":" << m.position << ",";
                ss << "\"rotation\":" << m.rotation << ",";
                ss << "\"scale\":" << m.scale << ",";
                ss << "\"color\":" << m.color << ",";
                ss << "},";
                ss << std::endl;
            }
            ss << tab(1) << "]" << std::endl;
        }
        
        
        ss << "}";
        ss << std::endl;
    }
    
    std::cout << "--- save info ---" << std::endl;
    std::cout << ss.str() << std::endl;
    
    saveToFile(filename, ss.str());
}


Vec3 parseVec3(JsonArray* json)
{
    std::cout << "array size =" << json->value.size() << std::endl;
    Vec3 v;
    v[0] = json->getNumeric(0)->value;
    v[1] = json->getNumeric(1)->value;
    v[2] = json->getNumeric(2)->value;
    return v;
}


void load(const std::string& filename, std::vector<ModelState>& models, ViewState& view)
{
    std::string json = loadText(filename);
    std::string jsonWithoutSpace = removeSpace(json);
    
    JsonObject* root = dynamic_cast<JsonObject*>( parse(jsonWithoutSpace) );
    JsonObject* editorview = root->getObject("editor_view");
    view.camera_rad = parseVec3(editorview->getArray("camera_rad"));
    
    JsonArray* array = root->getArray("models");
    
    int s = array->value.size();
    for(int i=0;i<s;++i)
    {
        ModelState state;
        JsonObject* statejson = array->getObject(i);
        
        std::string p = statejson->getString("primitive")->value;
        STRING_TO_CASE(Cube,p,state.primitive);
        STRING_TO_CASE(Sphere,p,state.primitive);
        STRING_TO_CASE(Cylinder,p,state.primitive);
        STRING_TO_CASE(Capsule,p,state.primitive);
        STRING_TO_CASE(Crystal,p,state.primitive);
        STRING_TO_CASE(Torus,p,state.primitive);
        
        state.position = parseVec3(statejson->getArray("position"));
        state.rotation = parseVec3(statejson->getArray("rotation"));
        state.scale = parseVec3(statejson->getArray("scale"));
        state.color = parseVec3(statejson->getArray("color"));
        
        models.push_back(state);
    }
}