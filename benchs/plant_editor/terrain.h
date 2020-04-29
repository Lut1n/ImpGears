

struct TreeDescriptor
{
    float _baseRadius;
    float _baseHeight;
    int _nodeCount;
    int _divisionCount;
    float _theta;
    float _radiusFactor;
    float _heightFactor;

    TreeDescriptor()
        : _baseRadius(0.5)
        , _baseHeight(5.0)
        , _nodeCount(7)
        , _divisionCount(3)
        , _theta(3.0/4.0)
        , _radiusFactor(0.7)
        , _heightFactor(0.7)
    {}
};


class PlantGenerator
{
public:

    Geometry::Ptr _cyl;
    Geometry::Ptr _sph;

    PlantGenerator()
    {
        _cyl = Geometry::create();
        *_cyl = Geometry::cone(16, 1.0, 1.0, 0.7);
        _cyl->rotX(-3.141592*0.5);
        _cyl->generateColors(Vec3(1.0));
        _cyl->generateNormals( Geometry::NormalGenMode_PerFace );
        _cyl->generateTexCoords( Geometry::TexGenMode_Cubic, 4.0);
    }


    Geometry buildBranchGeo(int index,
                             float radius, float height, Vec3 position, Vec3 rotation,
                             const TreeDescriptor& desc)
    {
        Geometry g = *_cyl;
        g.scale(Vec3(radius, height, radius));

        if(index < desc._nodeCount-1)
        {
            Vec3 p( 0.0, height, 0.0 );

            for(int i=0;i<desc._divisionCount;++i)
            {
                const float pi2 = 3.141592 * 2.0;
                Vec3 r1( desc._theta, pi2 * float(i) / desc._divisionCount, 0.0 );
                g += buildBranchGeo(index+1, radius*desc._radiusFactor, height*desc._heightFactor, p,r1, desc);
            }
        }

        g.rotX(rotation[0]); g.rotY(rotation[1]); g.rotZ(rotation[2]);
        g += position;

        return g;
    }


    GeoNode::Ptr generateTree(const TreeDescriptor& desc, Material::Ptr mat, ReflexionModel::Ptr reflexion, bool wireframe)
    {
        Geometry::Ptr geo = Geometry::create();
        *geo = buildBranchGeo(0,desc._baseRadius, desc._baseHeight, Vec3(0.0), Vec3(0.0), desc);

        GeoNode::Ptr gn = GeoNode::create(geo, wireframe);
        gn->setMaterial(mat);
        gn->setReflexion(reflexion);

        return gn;
    }

};
