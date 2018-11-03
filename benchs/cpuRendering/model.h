    
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateTorch(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius,radius,radius));
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "torch geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}     
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateRock(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(0.7);
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
    geometry.sphericalNormalization(1.0);
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
    geometry.scale(Vec3(radius,radius,0.1));
    geometry.noiseBump(6, 0.7, 1.0, 0.3);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "plane geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}
   
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.reserve(buf.size());
      
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
    colorBuffer.reserve(buf.size());
      
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
    colorBuffer.reserve(buf.size());
      
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
    colorBuffer.reserve(buf.size());
      
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
struct DefaultRenderFrag : public FragCallback
{
	virtual void operator()(Interpolator& interpo,int x, int y, imp::ImageData** targetArr)
	{
		// depth test
		float new_depth = (std::min(1.0, abs(interpo2.get(Varying_MVVert).z()-0.1) / 20.0)) * 255.f;
		float curr_depth = targetArr[1]->getPixel(x, y)[0];
		if( new_depth < curr_depth)
		{
			Vec3 zero(0.0,0.0,0.0);
			Vec3 normal = interpo.get(Varying_Vert); normal.normalize();
			Vec3 light_dir = light_1.position - interpo.get(Varying_MVert);
			light_dir.normalize();
			 
			const float* md = state.view.getData();
			Vec3 cam_dir(md[12],md[13],md[14]); cam_dir = (cam_dir) - interpo.get(Varying_MVert); cam_dir.normalize();
			 
			float a = light_dir.dot(normal);
			clamp(a,0.0,1.0);
			 
			Vec3 reflection = (normal * 2.0 * a) - light_dir;
			 
			float s = reflection.dot(cam_dir);
			clamp(s,0.0,1.0);
			 
			Vec3 spec = Vec3(1.0,1.0,1.0)*s;
			 
			Vec3 light_color = Vec3(0.1,0.1,0.1) + Vec3(0.7,0.7,0.7)*a + pow(spec, 8);
			clamp(light_color);
			 
			Vec3 base_color = light_color * interpo.get(Varying_Color);
			clamp(base_color);
			Vec4 depth(new_depth,new_depth,new_depth,255.f);
			targetArr[0]->setPixel(x,y,base_color * 255.f);
			targetArr[1]->setPixel(x,y,depth);
		}
	}
};
// -----------------------------------------------------------------------------------------------------------------------
struct TerrRenderFrag : public FragCallback
{
	virtual void operator()(Interpolator& interpo,int x, int y, imp::ImageData** targetArr)
	{
		// depth test
		float new_depth = (std::min(1.0, abs(interpo2.get(Varying_MVVert).z()-0.1) / 20.0)) * 255.f;
		float curr_depth = targetArr[1]->getPixel(x, y)[0];
		if( new_depth < curr_depth)
		{
			Vec3 zero(0.0,0.0,0.0);
			Vec3 normal(0.0,0.0,1.0);
			Vec3 light_dir = light_1.position - interpo.get(Varying_MVert);
			light_dir.normalize();
			 
			const float* md = state.view.getData();
			Vec3 cam_dir(md[12],md[13],md[14]); cam_dir = (cam_dir) - interpo.get(Varying_MVert); cam_dir.normalize();
			 
			float a = light_dir.dot(normal);
			a = a<0.0?0.0:a;
			a = a>1.0?1.0:a;
			 
			Vec3 reflection = (normal * 2.0 * a) - light_dir;
			 
			float s = reflection.dot(cam_dir);
			s = s<0.0?0.0:s;
			s = s>1.0?1.0:s;
			 
			Vec3 spec = Vec3(1.0,1.0,1.0)*s;
			 
			Vec3 light_color = Vec3(0.1,0.1,0.1) + Vec3(0.7,0.7,0.7)*a + pow(spec, 8);
			clamp(light_color);
			 
			Vec3 base_color = light_color * interpo.get(Varying_Color);
			clamp(base_color);
			Vec4 depth(new_depth,new_depth,new_depth,255.f);
			targetArr[0]->setPixel(x,y,base_color * 255.f);
			targetArr[1]->setPixel(x,y,depth);
		}
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct LightRenderFrag : public FragCallback
{
	virtual void operator()(Interpolator& interpo,int x, int y, imp::ImageData** targetArr)
	{
		// depth test
		float new_depth = (std::min(1.0, abs(interpo2.get(Varying_MVVert).z()-0.1) / 20.0)) * 255.f;
		float curr_depth = targetArr[1]->getPixel(x, y)[0];
		if( new_depth < curr_depth)
		{
			Vec4 white(1,1,1,1);
			Vec4 depth(new_depth,new_depth,new_depth,255.f);
			targetArr[0]->setPixel(x,y,white * 255.f);
			targetArr[1]->setPixel(x,y,depth);
		}
	}
};

// -----------------------------------------------------------------------------------------------------------------------
 struct DefaultVertCallback : public VertCallback
 {
    virtual Triangle operator()(float* vert_in, float* vert_in2, Triangle* vert_out)
    {
        imp::Matrix4 mv = state.model * state.view;
        imp::Matrix4 mvp = mv * state.projection;
        Vec3 win(state.viewport[2]*0.5, state.viewport[3]*0.5, 1.0);
        Vec3 win2(state.viewport[2]*0.5, state.viewport[3]*0.5, 0.0);
        
        vert_out[Varying_Vert] = Triangle(vert_in);
        vert_out[Varying_MVert] = vert_out[Varying_Vert] * state.model;
        vert_out[Varying_MVVert] = vert_out[Varying_Vert] * mv;
        vert_out[Varying_MVPVert] = (vert_out[Varying_Vert] * mvp * win) + win2;
        vert_out[Varying_Color] = Triangle(vert_in2);
        
        return vert_out[Varying_MVPVert];
    }
 };
 
// -----------------------------------------------------------------------------------------------------------------------
 struct ClearVertCallback : public VertCallback
 {
    virtual Triangle operator()(float* vert_in, float* vert_in2, Triangle* vert_out)
    {
        Vec3 win(state.viewport[2]*0.5, state.viewport[3]*0.5, 1.0);
        Vec3 win2(state.viewport[2]*0.5, state.viewport[3]*0.5, 0.0);
        
        vert_out[Varying_Vert] = Triangle(vert_in);
        vert_out[Varying_MVert] = Triangle(vert_in);
        vert_out[Varying_MVVert] = Triangle(vert_in);
        vert_out[Varying_MVPVert] = (vert_out[Varying_Vert] * win) + win2;
        vert_out[Varying_Color] = Triangle(vert_in2);
        
        return vert_out[Varying_MVPVert];
    }
 };
 
// -----------------------------------------------------------------------------------------------------------------------
struct ClearFragCallback : public FragCallback
{
	virtual void operator()(Interpolator& interpo,int x, int y, imp::ImageData** targetArr)
	{
        Vec4 col(0.7,0.7,1.0,1.0);
        Vec4 depth(1.0,1.0,1.0,1.0);
		targetArr[0]->setPixel(x,y,col * 255);
		targetArr[1]->setPixel(x,y,depth * 255);
	}
};
 
 DefaultVertCallback defaultVert;
 DefaultRenderFrag defaultFrag;
 LightRenderFrag lightFrag;
 TerrRenderFrag terrFrag;
 ClearVertCallback clearVert;
 ClearFragCallback clearFrag;
