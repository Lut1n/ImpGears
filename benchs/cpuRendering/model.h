

#define TARGET_RGB 0
#define TARGET_DEPTH 1
    
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<Vec3> generateTorch(const Vec3& center, float radius, int sub)
{
    imp::Geometry geometry = imp::Geometry::tetrahedron();
	geometry = geometry.subdivise(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius));
	Geometry::intoCCW( geometry );
	
    return geometry._vertices;
}     

// -----------------------------------------------------------------------------------------------------------------------  
std::vector<Vec3> generateRockHat(const Vec3& center, float radius, int sub)
{
    imp::Geometry geometry = imp::Geometry::cube();
	geometry *= 0.5;
	geometry = geometry.subdivise(sub);
    geometry.sphericalNormalization(0.5);
    geometry.scale(Vec3(radius * 0.7,radius * 0.7,radius * 1.5));
	
	
    imp::Geometry geo2 = imp::Geometry::cube();
	geo2 *= 0.5; // side size = one
	geo2 *= Vec3(1.0,1.0,0.5); /* h = 0.25*/ geo2 += Vec3(0.0,0.0,0.25); // demi-cube
	geo2 = geo2.subdivise(sub);
    geo2.sphericalNormalization(0.7);
    geo2.scale(Vec3(radius*2.0,radius*2.0,radius*2.0));
	geo2 += Vec3(0.0,0.0,1.0);
	
	geometry += geo2;
	Geometry::intoCCW( geometry );
       
    return geometry._vertices;
}  
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<Vec3> generatePlane(const Vec3& center, float radius, int sub)
{
    imp::Geometry geometry = imp::Geometry::cube();
	geometry *= 0.5;
	geometry = geometry.subdivise(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius,radius,radius*0.25));
    // geometry.noiseBump(6, 0.7, 1.0, 0.3);
	Geometry::intoCCW( geometry );
       
    return geometry._vertices;
}
   
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<Vec3> generateColors(std::vector<Vec3>& buf)
{
    std::vector<Vec3> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); ++i)
    {
        float disturb = imp::perlinOctave(buf[i].x(), buf[i].y(), buf[i].z(), 8, 0.7, 1.0/32.0) * 2.0 - 1.0;
         
        float a = std::atan2(buf[i].x(),buf[i].y());
         
        float signal = (1.0+std::sin((a + disturb*4.0) * 16.0))*0.5;
         
        Vec3 colorA(0.0,0.7,0.3);
        Vec3 colorB(0.3,0.0,1.0);
         
        Vec3 finalColor = (colorB - colorA)*signal + colorA;
         
        colorBuffer[i] = finalColor;
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<Vec3> generateColors2(std::vector<Vec3>& buf)
{
    std::vector<Vec3> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); ++i)
    {
        // float disturb = (perlin(v * (1.0/4.0)))*2.0 - 1.0;
        float disturb = (imp::perlinOctave(buf[i].x(), buf[i].y(), buf[i].z(), 8, 0.7, 1.0/32.0) + 0.5) * 4.0;
        // disturb=disturb<0.0?0.0:disturb;
        // disturb=disturb>1.0?1.0:disturb;
         
        Vec3 colorA(0.0,0.3,0.0);
        Vec3 colorB(0.2,1.0,0.2);
         
        Vec3 finalColor = (colorB - colorA)*disturb + colorA;
         
        colorBuffer[i] = finalColor;
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<Vec3> generateColors3(std::vector<Vec3>& buf)
{
    std::vector<Vec3> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    Vec3 color(1.0,1.0,1.0);
    for(unsigned int i=0; i<buf.size(); ++i)
    {
        colorBuffer[i] = color;
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}

// -----------------------------------------------------------------------------------------------------------------------
struct DepthFrag : public imp::FragCallback
{
    Meta_Class(DepthFrag)
    
    Rasterizer* _rast;
    
    DepthFrag(Rasterizer* rast)
        : _rast(rast)
    {}
        
    virtual void exec(ImageBuf& targets, const Vec3& pt, Uniforms* uniforms = nullptr)
    {
        float depth = -uniforms->get(Varying_MVVert).z();
        Vec4 color = _rast->_defaultColor * clamp( linearstep(0.1f, 20.f, depth)  );
        color[3] = 255;
        targets[TARGET_RGB]->setPixel(pt[0],pt[1],color);
    }
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct DepthTestFrag : public imp::FragCallback
{
    Meta_Class(DepthTestFrag)
	virtual void apply(int x, int y, Uniforms& uniforms, Image::Ptr* targets) = 0;
	
	void phong(int x, int y, Uniforms& uniforms, Image::Ptr* targets, const Vec3& normal)
	{
		Vec3 light_pos = light_1.position;
		Vec3 frag_pos = uniforms.get(Varying_MVert);
		const float* md = state.view.data();
		Vec3 cam_pos(md[12],md[13],md[14]);
		
		Vec3 light_dir = light_pos - frag_pos;
		Vec3 cam_dir = cam_pos - frag_pos;
		light_dir.normalize();
		cam_dir.normalize();
		 
		float a = clamp( light_dir.dot(normal) );
		Vec3 reflection = (normal * 2.0 * a) - light_dir;
		float s = clamp( reflection.dot(cam_dir) );
		
		float light_lvl = clamp( 0.2 + 0.6*a + pow(s, 8) );
		Vec4 base_color = Vec4(dotClamp( Vec3(light_lvl) * uniforms.get(Varying_Color) ));
        base_color *= 255.f;
		targets[TARGET_RGB]->setPixel(x,y,base_color);
	}
	
	virtual void exec(ImageBuf& targets, const Vec3& pt, Uniforms* uniforms = nullptr)
    {
        depthTest(pt[0],pt[1],*uniforms,targets.data());
    }
	
	virtual void depthTest(int x, int y, Uniforms& uniforms, Image::Ptr* targets)
	{
		// depth test
		float depth = -uniforms.get(Varying_MVVert).z();
		
		float depthPx = clamp( linearstep(0.1f, 20.f, depth )  )* 255.0;
        Vec4 depthV = targets[TARGET_DEPTH]->getPixel(x, y);
		float curr_depth = depthV[0];
		if( depthPx < curr_depth)
		{
			apply(x, y, uniforms, targets);
			
			Vec4 depth(depthPx);
			targets[TARGET_DEPTH]->setPixel(x,y,depth);
		}
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct DefaultRenderFrag : public DepthTestFrag
{
    Meta_Class(DefaultRenderFrag)
	virtual void apply(int x, int y, Uniforms& uniforms, Image::Ptr* targets)
	{
		Vec3 normal = uniforms.get(Varying_Vert); normal.normalize();
		phong(x,y,uniforms, targets, normal);
	}
};
// -----------------------------------------------------------------------------------------------------------------------
struct TerrRenderFrag : public DepthTestFrag
{
    Meta_Class(TerrRenderFrag)
	virtual void apply(int x, int y, Uniforms& uniforms, Image::Ptr* targets)
	{
		Vec3 normal(0.0,0.0,1.0);
		phong(x,y,uniforms, targets, normal);
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct LightRenderFrag : public DepthTestFrag
{
    Meta_Class(LightRenderFrag)
	virtual void apply(int x, int y, Uniforms& uniforms, Image::Ptr* targets)
	{
		Vec4 white(255.f);
		targets[0]->setPixel(x,y,white);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
 struct DefaultVertCallback : public VertCallback
 {
    virtual void operator()(Vec3& vert_in, Vec3& vert_in2, Uniforms& out_uniforms)
    {
        imp::Matrix4 mv = state.model * state.view;
        imp::Matrix4 mvp = mv * state.projection;
        Vec4 win(state.viewport[2]*0.5, state.viewport[3]*0.5, 1.0, 1.0);
        Vec4 win2(state.viewport[2]*0.5, state.viewport[3]*0.5, 0.0, 0.0);
        
		Vec4 vertex = vert_in;
		Vec4 mvertex = vertex * state.model;
		Vec4 mvvertex = vertex * mv;
		Vec4 mvpvertex = vertex * mvp;
		
		mvertex /= mvertex.w();
		mvvertex /= mvvertex.w();
		mvpvertex /= mvpvertex.w();
		
		mvpvertex *= win; mvpvertex += win2;
		
		out_uniforms.set(Varying_Vert,vert_in);
		out_uniforms.set(Varying_MVert,mvertex);
		out_uniforms.set(Varying_MVVert,mvvertex);
		out_uniforms.set(Varying_MVPVert,mvpvertex);
		out_uniforms.set(Varying_Color,vert_in2);
    }
 };
 
// -----------------------------------------------------------------------------------------------------------------------
 struct ClearVertCallback : public VertCallback
 {
    virtual void operator()(Vec3& vert_in, Vec3& vert_in2, Uniforms& out_uniforms)
    {
        Vec3 win(state.viewport[2]*0.5, state.viewport[3]*0.5, 1.0);
        Vec3 win2(state.viewport[2]*0.5, state.viewport[3]*0.5, 0.0);
		
		Vec3 mvpvertex = vert_in * win; mvpvertex += win2;
		
		out_uniforms.set(Varying_Vert,vert_in);
		out_uniforms.set(Varying_MVert,vert_in);
		out_uniforms.set(Varying_MVVert,vert_in);
		out_uniforms.set(Varying_MVPVert,mvpvertex);
		out_uniforms.set(Varying_Color,vert_in2);
    }
 };
 
// -----------------------------------------------------------------------------------------------------------------------
struct ClearFragCallback : public imp::FragCallback
{
    Meta_Class(ClearFragCallback)
	virtual void exec(ImageBuf& targets, const Vec3& pt, Uniforms* uniforms = nullptr)
    {
		targets[TARGET_RGB]->setPixel(pt[0],pt[1],Vec4(0.7,0.7,1.0,1.0) * 255);
		targets[TARGET_DEPTH]->setPixel(pt[0],pt[1],Vec4(255));
    }
};
 
 DefaultVertCallback defaultVert;
 DefaultRenderFrag::Ptr defaultFrag;
 LightRenderFrag::Ptr lightFrag;
 TerrRenderFrag::Ptr terrFrag;
 ClearVertCallback clearVert;
 ClearFragCallback::Ptr clearFrag;
 DepthFrag::Ptr depthFrag;
