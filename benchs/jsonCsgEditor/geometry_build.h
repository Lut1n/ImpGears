
static const float PI = 3.141592;
static const float HALF_PI = PI*0.5;
static const float TWO_PI = PI*2.0;

struct GeoSampler
{
    virtual Vec3 operator[](int index) {return Vec3();}
};

struct CircleSampler
{
    Vec3 _center;
    Vec3 _dir;
    float _radius;
    int _count;
    
    std::vector<Vec3> _samples;
    
    CircleSampler(const Vec3& center = Vec3(0.0), float radius = 1.0, Vec3 dir = Vec3::Y, int count = 16)
        : _center(center)
        , _dir(dir)
        , _radius(radius)
        , _count(count)
    {
        _samples.reserve(count);
        
        dir.normalize();
        
        Vec3 t1 = Vec3::Y.cross(dir);
        Vec3 t2 = Vec3::Z.cross(dir);
        
        Vec3 tangent, bitan;
        if( t1.length2() >= t2.length2() ) tangent = t1;
        else tangent = t2;
        
        tangent.normalize();
        bitan = dir.cross(tangent);
        bitan.normalize();
    
        for(int i=0;i<count;++i)
        {
            float f = float(i)/count;
            float c1 = std::cos(f * TWO_PI);
            float s1 = std::sin(f * TWO_PI);
            
            Vec3 pt = tangent * c1 + bitan * s1;
            pt *= _radius;
            
            _samples.push_back( _center+pt );
        }
    }
    
    const Vec3& at(float angle) const
    {
        int index = angle * _count / TWO_PI;
        return _samples[index];
    }
    
    Vec3 operator[](int index) const
    {
        return _samples[index];
    }
};

void add_cap_linear(Geometry::Ptr res, const CircleSampler& circle, const Vec3& t)
{
    int size = circle._count;
    int i2 = size-1;
    
    for(int i=0;i<size;++i)
    {
        const Vec3& bt1 = circle[i2];
        const Vec3& bt2 = circle[i];
        
        res->add( {bt1, t, bt2} );

        i2=i;
    }
}

void add_cap_round(Geometry::Ptr res, const CircleSampler& base, const Vec3& t, int capsize = 16)
{
    int size = base._count;
    int i2 = size-1;
    
    CircleSampler capSampler(Vec3(0.0),base._radius,base._dir,capsize);
    
    for(int i=0;i<size;++i)
    {
        const Vec3& bt1 = base[i2];
        const Vec3& bt2 = base[i];
        
        Vec3 r1 = Vec3(bt1.x(),0.0,bt1.z());
        Vec3 r2 = Vec3(bt2.x(),0.0,bt2.z());
        
        Vec3 h1 = Vec3(0.0,bt1.y(),0.0);
        Vec3 h2 = Vec3(0.0,bt2.y(),0.0);
        
        int j2=0;
        for(int j=1; j<capsize; ++j)
        {
            Vec3 pt1 = capSampler[j];
            Vec3 pt2 = capSampler[j2];
            
            Vec3 b1 = r1*pt2[0] + mix(h1,t, pt2[2]);
            Vec3 b2 = r2*pt2[0] + mix(h2,t, pt2[2]);
            
            Vec3 t1 = r1*pt1[0] + mix(h1,t, pt1[2]);
            Vec3 t2 = r2*pt1[0] + mix(h2,t, pt1[2]);
            
            res->add( {b2,b1,t2, t1,t2,b1} );
            
            if(j==capsize-1) res->add( {t2,t1,t} );
            
            j2=j;
        }

        i2=i;
    }
}

void cylinder_side(Geometry::Ptr res, const CircleSampler& bottom, const CircleSampler& top)
{
    int size = bottom._count;
    int i2 = size-1;
    
    for(int i=0;i<size;++i)
    {
        Vec3 bt1 = bottom[i2];
        Vec3 bt2 = bottom[i];
        Vec3 tp1 = top[i2];
        Vec3 tp2 = top[i];

        res->add( {bt2,bt1,tp2} );
        res->add( {tp1,tp2,bt1} );

        i2=i;
    }
}

Geometry::Ptr createCapsule(int sub = 16, float r = 1.0, float dlen = 1.0)
{
    float ex = dlen+r;
    
    CircleSampler bot_base(Vec3(0.0,-dlen,0.0),r,Vec3::Y,sub);
    CircleSampler top_base(Vec3(0.0,dlen,0.0),r,Vec3::Y,sub);
    
    Geometry::Ptr res = Geometry::create();
    cylinder_side(res, bot_base, top_base);
    add_cap_round(res, bot_base, Vec3(0.0,-ex,0.0), sub);
    add_cap_round(res, top_base, Vec3(0.0,ex,0.0), sub);

    res->setPrimitive(Geometry::Primitive_Triangles);
    return res;
}

Geometry::Ptr createCrystal(int sub = 16, float r = 1.0, float dlen = 1.0)
{
    float ex = dlen+r;
    
    CircleSampler bot_base(Vec3(0.0,-dlen,0.0),r,Vec3::Y,sub);
    CircleSampler top_base(Vec3(0.0,dlen,0.0),r,Vec3::Y,sub);
    
    Geometry::Ptr res = Geometry::create();
    cylinder_side(res, bot_base, top_base);
    add_cap_linear(res, bot_base, Vec3(0.0,-ex,0.0));
    add_cap_linear(res, top_base, Vec3(0.0,ex,0.0));

    res->setPrimitive(Geometry::Primitive_Triangles);
    return res;
}

Geometry::Ptr createCylinder(int sub = 16, float r = 1.0, float dlen = 1.0)
{
    CircleSampler bot_base(Vec3(0.0,-dlen,0.0),r,Vec3::Y,sub);
    CircleSampler top_base(Vec3(0.0,dlen,0.0),r,Vec3::Y,sub);
    
    Geometry::Ptr res = Geometry::create();
    cylinder_side(res, bot_base, top_base);
    add_cap_linear(res, bot_base, Vec3(0.0,-dlen,0.0));
    add_cap_linear(res, top_base, Vec3(0.0,dlen,0.0));

    res->setPrimitive(Geometry::Primitive_Triangles);
    return res;
}


Geometry::Ptr createTorus(int sub = 16, float mr = 1.0, float r = 0.5)
{
    CircleSampler major_circle(Vec3(0.0),mr,Vec3::Y,sub);
    std::vector<CircleSampler> sub_circles;
    
    int i2 = sub-1, i3 = sub-2;
    for(int i=0;i<sub;++i)
    {
        Vec3 dir = (major_circle[i] - major_circle[i3]); dir.normalize();
        sub_circles.push_back( CircleSampler(major_circle[i2],r,dir,sub) );
        
        i3=i2;
        i2=i;
    }
    
    Geometry::Ptr res = Geometry::create();
    i2 = sub-1;
    for(int i=0;i<sub;++i)
    {
        cylinder_side(res, sub_circles[i2], sub_circles[i]);
        i2 = i;
    }
    res->setPrimitive(Geometry::Primitive_Triangles);
    return res;
}

void interolateNormal_cyl(Geometry::Ptr& geo)
{
    for(int i=0;i<(int)geo->_vertices.size();++i)
    {
        Vec3 n = geo->_normals[i];
        float nh_len = n[1];
        
        if(std::abs(nh_len) <= 0.001)
        {
            float nb_len = std::sqrt(n[0]*n[0] + n[2]*n[2]);
            Vec3 base = geo->_vertices[i]; base[1] = 0.0; base.normalize();
            Vec3 n2 = base * nb_len; n2[1] = nh_len;
            geo->_normals[i] = n2;
        }
    }
}

void createMesh( std::vector<ModelState>& states, std::vector<GeoNode::Ptr>& meshes, Node::Ptr& root, const ModelState& a, ReflexionModel::Ptr refl, RenderPass::Ptr flags )
{
    Geometry::Ptr newGeo = Geometry::create();
                
    if(a.primitive == Sphere)
    {                    
        *newGeo = Geometry::sphere(16,1.0);
        Geometry::intoCCW(*newGeo, true);
        newGeo->generateColors(Vec3(1.0,1.0,1.0));
        newGeo->generateNormals(Geometry::NormalGenMode_Spheric);
        newGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    }
    else if(a.primitive == Cube)
    {
        *newGeo = Geometry::cube();
        Geometry::intoCCW(*newGeo, true);
        newGeo->generateColors(Vec3(1.0,1.0,1.0));
        newGeo->generateNormals(Geometry::NormalGenMode_PerFace);
        newGeo->generateTexCoords(Geometry::TexGenMode_Cubic);
    }
    else if(a.primitive == Torus)
    {
        newGeo = createTorus();
        // Geometry::intoCCW(*newGeo, true);
        newGeo->generateColors(Vec3(1.0,1.0,1.0));
        newGeo->generateNormals(Geometry::NormalGenMode_PerFace);
        newGeo->generateTexCoords(Geometry::TexGenMode_Cubic);
    }
    else
    {
        if(a.primitive == Cylinder) newGeo = createCylinder();
        else if(a.primitive == Capsule) newGeo = createCapsule();
        else if(a.primitive == Crystal) newGeo = createCrystal(5);
        
        Geometry::intoCCW(*newGeo, true);
        newGeo->generateColors(Vec3(1.0,1.0,1.0));
        newGeo->generateNormals(Geometry::NormalGenMode_PerFace);
        newGeo->generateTexCoords(Geometry::TexGenMode_Cubic);
        
        if(a.primitive == Cylinder) interolateNormal_cyl(newGeo);
        else if(a.primitive == Capsule) newGeo->interpolateNormals();
    }
    
    GeoNode::Ptr newNode = GeoNode::create(newGeo, false);
    newNode->setPosition(Vec3(0.0,0,0.0));
    newNode->setReflexion(refl);
    newNode->getState()->setRenderPass(flags);
    
    Material::Ptr newmat = Material::create(a.color, 64.0);
    newNode->setMaterial(newmat);
    
    newNode->setPosition(a.position);
    newNode->setRotation(a.rotation);
    newNode->setScale(a.scale);
    
    meshes.push_back(newNode);
    root->addNode(newNode);
    states.push_back(a);
}
