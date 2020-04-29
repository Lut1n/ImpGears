

IMPGEARS_BEGIN
class VegetalNode : public GeoNode
{
    public:
    Meta_Class(VegetalNode)

    VegetalNode(const Polyhedron& buf, bool wireframe = false) : GeoNode(buf,wireframe) {}
    VegetalNode(const Geometry::Ptr& geo, bool wireframe = false) : GeoNode(geo,wireframe) {}
};
IMPGEARS_END

struct TreeDescriptor
{
    float _baseRadius;
    float _baseHeight;
    int _nodeCount;
    int _divisionCount;
    float _theta;
    float _radiusFactor;
    float _heightFactor;
    float _leafSize;
    int _leafCount;
    Vec3 _leafColor;

    TreeDescriptor()
        : _baseRadius(0.5)
        , _baseHeight(5.0)
        , _nodeCount(5)
        , _divisionCount(3)
        , _theta(3.0/4.0)
        , _radiusFactor(0.7)
        , _heightFactor(0.7)
        , _leafSize(1.0)
        , _leafCount(3)
        , _leafColor(0.0,1.0,0.0)
    {}
};


class PlantGenerator
{
public:

    Geometry::Ptr _cyl;
    Geometry::Ptr _sph;
    Geometry::Ptr _leaf;

    PlantGenerator()
    {
        _cyl = Geometry::create();
        *_cyl = Geometry::cone(16, 1.0, 1.0, 0.7);
        // *_cyl = _cyl->subdivise(16);
        _cyl->rotX(-3.141592*0.5);
        Geometry::intoCCW(*_cyl);
        _cyl->generateColors(Vec3(1.0));
        _cyl->generateNormals( Geometry::NormalGenMode_PerFace );
        _cyl->generateTexCoords( Geometry::TexGenMode_Cubic, 4.0);
        _cyl->interpolateNormals();

        _leaf = Geometry::create();
        *_leaf = Geometry::cylinder(5,1.0,1.0);
        // _leaf->rotX(-3.141592*0.5);
        // _leaf->generateColors(_leafColor);
        _leaf->generateNormals( Geometry::NormalGenMode_PerFace );
        _leaf->generateTexCoords( Geometry::TexGenMode_Cubic, 1.0 );
        _leaf->scale( Vec3(0.5,0.5,0.025) );
        (*_leaf)+=Vec3(0.0,0.5,0.0);
        // _leaf->interpolateNormals();
    }
    
    void buildInstancedLeaf_sub(int index, float radius, float height, Vec3 position, Vec3 rotation, const TreeDescriptor& desc, std::vector<Matrix4>& transforms, const Matrix4& parent)
    {
        Matrix4 curr = Matrix4::rotationXYZ(rotation) * Matrix4::translation(position) * parent;

        if(index < desc._nodeCount-1)
        {
            Vec3 p( 0.0, height, 0.0 );

            for(int i=0;i<desc._divisionCount;++i)
            {
                const float pi2 = 3.141592 * 2.0;
                Vec3 r1( desc._theta, pi2 * float(i) / desc._divisionCount, 0.0 );
                buildInstancedLeaf_sub(index+1, radius*desc._radiusFactor, height*desc._heightFactor, p,r1, desc, transforms, curr);
            }

            // leaf
            for(int i=0;i<desc._leafCount;++i)
            {
                const float pi2 = 3.141592 * 2.0;
                float ang_pos = pi2 * float(i) / float(desc._leafCount);

                Matrix4 tilt = Matrix4::rotationX(desc._theta*2.0);
                Matrix4 pos = Matrix4::translation(0.0, height, radius);
                Matrix4 rot = Matrix4::rotationY(ang_pos);
                Matrix4 leafMat = tilt * pos * rot;

                Matrix4 trans = leafMat * curr;
                transforms.push_back(trans);
            }
        }
    }

    Node::Ptr buildInstancedLeaf(const TreeDescriptor& desc, Material::Ptr mat, ReflexionModel::Ptr refl)
    {
        InstancedGeometry::Ptr instanced = InstancedGeometry::create();
        instanced->clone(_leaf);
        instanced->scale(desc._leafSize);
        instanced->generateColors(desc._leafColor);

        std::vector<Matrix4> transforms;
        buildInstancedLeaf_sub(0,desc._baseRadius, desc._baseHeight, Vec3(0.0), Vec3(0.0), desc, transforms, Matrix4());
        instanced->setTransforms(transforms);
        instanced->setCount(transforms.size());

        GeoNode::Ptr node = GeoNode::create(instanced,false);
        node->setMaterial(mat);
        node->getState()->setReflexion(refl);
        return node;
    }
    
    void buildInstancedBranch(int index, float radius, float height, Vec3 position, Vec3 rotation, const TreeDescriptor& desc, std::vector<Matrix4>& transforms, const Matrix4& parent)
    {
        static const float pi2 = 3.141592 * 2.0;
        
        Matrix4 curr = Matrix4::rotationXYZ(rotation) * Matrix4::translation(position) * parent;
        Matrix4 trans = Matrix4::scale(radius, height, radius) * curr;
        transforms.push_back(trans);

        if(index < desc._nodeCount-1)
        {
            Vec3 p( 0.0, height, 0.0 );

            for(int i=0;i<desc._divisionCount;++i)
            {
                Vec3 r1( desc._theta, pi2 * float(i) / desc._divisionCount, 0.0 );
                buildInstancedBranch(index+1, radius*desc._radiusFactor, height*desc._heightFactor, p, r1, desc, transforms,curr);
            }
        }
    }

    Node::Ptr buildInstancedTree(const TreeDescriptor& desc, Material::Ptr mat, ReflexionModel::Ptr refl)
    {
        InstancedGeometry::Ptr instanced = InstancedGeometry::create();
        instanced->clone(_cyl);
        
        std::vector<Matrix4> transforms;
        buildInstancedBranch(0,desc._baseRadius, desc._baseHeight, Vec3(0.0), Vec3(0.0), desc, transforms, Matrix4());
        instanced->setTransforms(transforms);
        instanced->setCount(transforms.size());
        
        GeoNode::Ptr node = GeoNode::create(instanced,false);
        node->setMaterial(mat);
        node->getState()->setReflexion(refl);
        return node;
    }

    Node::Ptr generateGrass(const TreeDescriptor& desc, Material::Ptr mat, ReflexionModel::Ptr reflexion, bool wireframe, Geometry::Ptr refTerrain, const Vec3& scale, const Vec3& pos)
    {
        InstancedGeometry::Ptr instanced = InstancedGeometry::create();
        instanced->clone(_cyl);
        Vec3 nullVec(0.0);

        bool first = true;

        int p = 11;
        std::vector<Matrix4> transforms;

        for(int i=0;i<refTerrain->size();++i)
        {
            if( refTerrain->_normals[i].dot(Vec3::Y) > 0.5 && (i%11 == 0) )
            {
                Vec3 center = refTerrain->_vertices[i] * scale + pos;
                nullVec += Vec3(0.0,2.15647,0.0);
                buildInstancedBranch(0, desc._baseRadius, desc._baseHeight, center, nullVec, desc, transforms, Matrix4());
            }
        }

        instanced->setTransforms(transforms);
        GeoNode::Ptr node = GeoNode::create(instanced,false);
        node->setMaterial(mat);
        node->getState()->setReflexion(reflexion);

        return node;
    }

};
