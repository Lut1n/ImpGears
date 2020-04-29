

struct LeafDescription
{
   int branchCount;
   int branchDepth;
   int texOctave;
   float texPersist;
   float texFreq;
   float shapeFq1;
   float shapeFq2;
   float shapeMin;
   float shapeMax;
   float thickness;
   float length;
   float lengthFct;
   float tilt;
   
   LeafDescription()
    : branchCount(6)
    , branchDepth(2)
    , texOctave(2)
    , texPersist(0.7)
    , texFreq(2.0)
    , shapeFq1(2.0)
    , shapeFq2(1.0)
    , shapeMin(0.0)
    , shapeMax(0.3)
    , thickness(0.02)
    , length(0.13)
    , lengthFct(0.9)
    , tilt(0.5)
   {}
};


class LeafRenderer : public RenderToSamplerNode
{
   public:
   
   Meta_Class(LeafRenderer)
   
   // iTexOctave, iTexFreq, iTexPersist
   Uniform::Ptr u_branchDepth;
   Uniform::Ptr u_branchCount;
   Uniform::Ptr u_texOctave;
   Uniform::Ptr u_texPersist;
   Uniform::Ptr u_texFreq;
    Uniform::Ptr u_ShapeFq1;
    Uniform::Ptr u_ShapeFq2;
    Uniform::Ptr u_ShapeMin;
    Uniform::Ptr u_ShapeMax;
    Uniform::Ptr u_Tilt;
    Uniform::Ptr u_Len;
    Uniform::Ptr u_LenFct;
    Uniform::Ptr u_Thickness;
   
   LeafDescription desc;
   
   LeafRenderer()
   {
        Graph::Ptr graph = Graph::create();
        graph->getInitState()->setViewport( Vec4(0.0,0.0,512.0,512.0) );
        graph->getInitState()->setOrthographicProjection(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
        graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));
        
        QuadNode::Ptr quad = QuadNode::create();
        u_branchDepth = Uniform::create("iDepth",Uniform::Type_1i);
        u_branchCount = Uniform::create("iBranchs",Uniform::Type_1i);
        u_texOctave = Uniform::create("iTexOctave",Uniform::Type_1i);
        u_texPersist = Uniform::create("iTexPersist",Uniform::Type_1f);
        u_texFreq = Uniform::create("iTexFreq",Uniform::Type_1f);
        u_ShapeFq1 = Uniform::create("iShapeFq1",Uniform::Type_1f);
        u_ShapeFq2 = Uniform::create("iShapeFq2",Uniform::Type_1f);
        u_ShapeMin = Uniform::create("iShapeMin",Uniform::Type_1f);
        u_ShapeMax = Uniform::create("iShapeMax",Uniform::Type_1f);
        u_Tilt = Uniform::create("iTilt",Uniform::Type_1f);
        u_Len = Uniform::create("iLen",Uniform::Type_1f);
        u_LenFct = Uniform::create("iLenFct",Uniform::Type_1f);
        u_Thickness = Uniform::create("iThickness",Uniform::Type_1f);

        quad->getState()->setUniform(u_branchDepth);
        quad->getState()->setUniform(u_branchCount);
        quad->getState()->setUniform(u_texOctave);
        quad->getState()->setUniform(u_texPersist);
        quad->getState()->setUniform(u_texFreq);
        quad->getState()->setUniform(u_ShapeFq1);
        quad->getState()->setUniform(u_ShapeFq2);
        quad->getState()->setUniform(u_ShapeMin);
        quad->getState()->setUniform(u_ShapeMax);
        quad->getState()->setUniform(u_Tilt);
        quad->getState()->setUniform(u_Len);
        quad->getState()->setUniform(u_LenFct);
        quad->getState()->setUniform(u_Thickness);
                
        ReflexionModel::Ptr shader = ReflexionModel::create(ReflexionModel::Lighting_Phong, ReflexionModel::Texturing_Customized, ReflexionModel::MRT_2_Col_Emi, "glsl gen texture");
        shader->_fragCode_texturing = loadText("./leaf_shader.frag");

        quad->setReflexion(shader);
        graph->setRoot(quad);

        setScene(graph);
        updateDesc(desc);
        
        
        
        ImageSampler::Ptr genResult = ImageSampler::create(512, 512, 3, Vec4(0.0,0.0,1.0,1.0));
        setTexture(genResult);
        Material::Ptr material = Material::create(Vec3(1.0), 32.0);
        material->_baseColor = genResult;
        QuadNode::Ptr displayquad = QuadNode::create();
        displayquad->setMaterial(material);
        ReflexionModel::Ptr displayshader = ReflexionModel::create(ReflexionModel::Lighting_None, ReflexionModel::Texturing_Samplers_CN, ReflexionModel::MRT_1_Col, "glsl texture display");
        displayquad->setReflexion(displayshader);
        addNode(displayquad);
   }
   
   void updateDesc(const LeafDescription& ld)
   {
        desc = ld;
        u_branchDepth->set(desc.branchDepth);
        u_branchCount->set(desc.branchCount);
        u_texOctave->set(desc.texOctave);
        u_texPersist->set(desc.texPersist);
        u_texFreq->set(desc.texFreq);
        u_ShapeFq1->set(desc.shapeFq1);
        u_ShapeFq2->set(desc.shapeFq2);
        u_ShapeMin->set(desc.shapeMin);
        u_ShapeMax->set(desc.shapeMax);
        u_Tilt->set(desc.tilt);
        u_Len->set(desc.length);
        u_LenFct->set(desc.lengthFct);
        u_Thickness->set(desc.thickness);
        makeDirty();
   }
};
