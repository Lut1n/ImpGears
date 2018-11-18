

#define TARGET_RGB 0
#define TARGET_DEPTH 1
    
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateTorch(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius));
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "torch geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}     
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateRock(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(0.5);
    geometry.scale(Vec3(radius * 0.7,radius * 0.7,radius * 1.5));
    geometry.noiseBump(6, 0.7, 1.0, 0.2);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "rock geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}   
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateHat(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(0.7);
    geometry.scale(Vec3(radius,radius,radius*0.3));
    geometry.noiseBump(6, 0.7, 1.0, 0.2);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "hat geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generatePlane(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius,radius,radius*0.25));
    geometry.noiseBump(6, 0.7, 1.0, 0.3);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "plane geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}
   
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 v(buf[i+0],buf[i+1],buf[i+2]);
        float disturb = imp::perlinOctave(v.x(), v.y(), v.z(), 8, 0.7, 1.0/32.0) * 2.0 - 1.0;
         
        float a = std::atan2(buf[i+0],buf[i+1]);
         
        float signal = (1.0+std::sin((a + disturb*4.0) * 16.0))*0.5;
         
        Vec3 colorA(0.0,0.7,0.3);
        Vec3 colorB(0.3,0.0,1.0);
         
        Vec3 finalColor = (colorB - colorA)*signal + colorA;
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors2(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 v(buf[i+0],buf[i+1],buf[i+2]);
        // float disturb = (perlin(v * (1.0/4.0)))*2.0 - 1.0;
        float disturb = (imp::perlinOctave(v.x(), v.y(), v.z(), 8, 0.7, 1.0/32.0) + 0.5) * 4.0;
        // disturb=disturb<0.0?0.0:disturb;
        // disturb=disturb>1.0?1.0:disturb;
         
        Vec3 colorA(0.0,0.3,0.0);
        Vec3 colorB(0.2,1.0,0.2);
         
        Vec3 finalColor = (colorB - colorA)*disturb + colorA;
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors3(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 finalColor(1.0,1.0,1.0);
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors4(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.resize(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 v(buf[i+0],buf[i+1],buf[i+2]);
        float disturb = imp::perlinOctave(v.x(), v.y(), v.z(), 8, 0.7, 1.0/32.0) * 2.0 - 1.0;
         
        float a = std::atan2(buf[i+0],buf[i+1]);
         
        float signal = (1.0+std::sin((a + disturb*4.0) * 16.0))*0.5;
         
        Vec3 colorA(0.3,0.7,0.7);
        Vec3 colorB(0.7,0.2,0.3);
         
        Vec3 finalColor = mix(colorA,colorB,signal);
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
 
 
// -----------------------------------------------------------------------------------------------------------------------
struct DepthTestFrag : public FragCallback
{
	virtual void apply(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets) = 0;
	
	void phong(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets, const Vec3& normal)
	{
		Vec3 light_pos = light_1.position;
		Vec3 frag_pos = uniforms.get(Varying_MVert);
		const float* md = state.view.getData();
		Vec3 cam_pos(md[12],md[13],md[14]);
		
		Vec3 light_dir = light_pos - frag_pos;
		Vec3 cam_dir = cam_pos - frag_pos;
		light_dir.normalize();
		cam_dir.normalize();
		 
		float a = clamp( light_dir.dot(normal) );
		Vec3 reflection = (normal * 2.0 * a) - light_dir;
		float s = clamp( reflection.dot(cam_dir) );
		
		float light_lvl = clamp( 0.2 + 0.6*a + pow(s, 8) );
		Vec3 base_color = clamp( Vec3(light_lvl) * uniforms.get(Varying_Color) );
		targets[TARGET_RGB].setPixel(x,y,base_color * 255.f);
	}
	
	virtual void operator()(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets)
	{
		// depth test
		float depth = -uniforms.get(Varying_MVVert).z();
		
		float depthPx = clamp( linearstep(0.1f, 20.f, depth )  )* 255.0;
		float curr_depth = targets[TARGET_DEPTH].getPixel(x, y)[0];
		if( depthPx < curr_depth)
		{
			apply(x, y, uniforms, targets);
			
			Vec4 depth(depthPx);
			targets[TARGET_DEPTH].setPixel(x,y,depth);
		}
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct DefaultRenderFrag : public DepthTestFrag
{
	virtual void apply(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets)
	{
		Vec3 normal = uniforms.get(Varying_Vert); normal.normalize();
		phong(x,y,uniforms, targets, normal);
	}
};
// -----------------------------------------------------------------------------------------------------------------------
struct TerrRenderFrag : public DepthTestFrag
{
	virtual void apply(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets)
	{
		Vec3 normal(0.0,0.0,1.0);
		phong(x,y,uniforms, targets, normal);
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct LightRenderFrag : public DepthTestFrag
{
	virtual void apply(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets)
	{
		Vec4 white(255.f);
		targets[0].setPixel(x,y,white);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
 struct DefaultVertCallback : public VertCallback
 {
    virtual void operator()(Vec3& vert_in, Vec3& vert_in2, UniformBuffer& out_uniforms)
    {
        imp::Matrix4 mv = state.model * state.view;
        imp::Matrix4 mvp = mv * state.projection;
        Vec3 win(state.viewport[2]*0.5, state.viewport[3]*0.5, 1.0);
        Vec3 win2(state.viewport[2]*0.5, state.viewport[3]*0.5, 0.0);
        
		Vec4 vertex = Vec4(vert_in);
		Vec3 mvertex = Vec3(vertex * state.model);
		Vec3 mvvertex = Vec3(vertex * mv);
		Vec3 mvpvertex = Vec3(vertex * mvp);
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
    virtual void operator()(Vec3& vert_in, Vec3& vert_in2, UniformBuffer& out_uniforms)
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
struct ClearFragCallback : public FragCallback
{
	virtual void operator()(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets)
	{
        Vec4 col(0.7,0.7,1.0,1.0);
        Vec4 depth(1.0,1.0,1.0,1.0);
		targets[TARGET_RGB].setPixel(x,y,col * 255);
		targets[TARGET_DEPTH].setPixel(x,y,depth * 255);
	}
};
 
 DefaultVertCallback defaultVert;
 DefaultRenderFrag defaultFrag;
 LightRenderFrag lightFrag;
 TerrRenderFrag terrFrag;
 ClearVertCallback clearVert;
 ClearFragCallback clearFrag;
