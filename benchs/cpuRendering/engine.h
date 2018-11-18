#include <Geometry/ProceduralGeometry.h>
#include <SceneGraph/BmpLoader.h>
#include <Core/Vec4.h>
#include <Core/Perlin.h>
#include <Core/Math.h>
#include <Texture/ImageUtils.h>
  
#include <ctime>
#include <vector>
#include <map>

int s_counter = 0;

void log(const Vec3& v)
{
	std::cout << "vec[" << v[0] << "; " << v[1] << "; " << v[2] << "]" << std::endl;
}

void log(const std::vector<Vec3>& v)
{
	std::cout << "buffer (len=" << v.size() << ") [" << std::endl;
	for(int i=0;i<(int)v.size();++i) log(v[i]);
	std::cout << "]" << std::endl;
}

void log(const Vec3* v, int N)
{
	std::cout << "array (len=" << N << ") [" << std::endl;
	for(int i=0;i<N;++i) log(v[i]);
	std::cout << "]" << std::endl;
}

// -----------------------------------------------------------------------------------------------------------------------
struct UniformBuffer
{
    std::map<int, int> _adr;
    std::vector<Vec3> _buf;
    
    void set(int id, Vec3& buf)
    {
		if(!contains(id)) alloc(id,1);
		// std::cout << "set id " << id << " at " << _adr[id] << " in " << _buf.size() << std::endl;
		_buf[_adr[id]] = buf;
    }
    
    bool contains(int id) { return _adr.find(id) != _adr.end(); }
    
    void alloc(int id, int size)
	{
		int index = _buf.size();
		_buf.resize(index+size);
		_adr[id] = index;
	}
    
    Vec3 get(int id) { return _buf[_adr[id]]; }
    
    void mix(const UniformBuffer& v1, const UniformBuffer& v2, float delta)
    {
        _adr = v1._adr; _buf.resize(v1._buf.size());
        for(int i=0; i<(int)_buf.size(); ++i) _buf[i] = imp::mix( v1._buf[i], v2._buf[i], delta );
    }
};

// -----------------------------------------------------------------------------------------------------------------------
 struct VertCallback
 {
    virtual void operator()(Vec3& vert_in, Vec3& vert_in2, UniformBuffer& out_uniforms)  = 0;
 };
// -----------------------------------------------------------------------------------------------------------------------
 struct FragCallback
 {
    virtual void operator()(int x, int y, UniformBuffer& uniforms, imp::ImageData* targets) = 0;
 };

// -----------------------------------------------------------------------------------------------------------------------
struct Rasterizer
{
	static void drawWire(Vec3* vertices, UniformBuffer* uniformBufs, imp::ImageData* targets, FragCallback& fragCallback)
	{
		UniformBuffer uniforms;
		Vec3 dirv = vertices[1] - vertices[0];
		float len = dirv.length();
		
		for(int t=0;t<std::floor(len)+1;++t)
		{
			float rel = clamp((float)t/(float)len);
			Vec3 position = imp::mix(vertices[0],vertices[1],rel);
			int x = std::floor(position.x());
			int y = std::floor(position.y());
			if(x < 0 || x>=(int)targets[0].getWidth() || y<0 || y>=(int)targets[0].getHeight()) continue;
			
			uniforms.mix(uniformBufs[0], uniformBufs[1], rel);
			fragCallback(x,y,uniforms,targets);
		}
	}
	
	static void drawHorizontalLine(Vec3* vertices, UniformBuffer* uniformBufs, imp::ImageData* targets, FragCallback& fragCallback)
	{
		if(vertices[0].y() < 0.0 || vertices[0].y() >= targets[0].getHeight()) return;

		int left=0,right=1;
		if(vertices[left].x() > vertices[right].x()){left=1;right=0;}
		
		UniformBuffer uniforms;
		for(int x=std::floor(vertices[left].x());x<std::floor(vertices[right].x())+1;++x)
		{
			if(x < 0) continue;
			if(x >= (int)targets[0].getWidth()) return;
			
			float rel = clamp(linearstep(vertices[left].x(),vertices[right].x(), (float)x));
			uniforms.mix(uniformBufs[left], uniformBufs[right], rel);
			fragCallback(x,(int)vertices[0].y(),uniforms,targets);
		}
	}
	
	static void drawTriangle(Vec3* vertices, UniformBuffer* uniformBufs, imp::ImageData* targets, FragCallback& fragCallback)
	{
		int bottom=0,center=1,top=2;
        if(vertices[bottom].y()>vertices[center].y()) swap(bottom,center);
		if(vertices[bottom].y()>vertices[top].y()) swap(bottom,top);
		if(vertices[center].y()>vertices[top].y()) swap(center,top);
		
		// for(int y=std::floor(vertices[bottom].y());y<std::floor(vertices[top].y())+1;y+=2)
		for(int y=std::floor(vertices[bottom].y());y<std::floor(vertices[top].y())+1;++y)
		{
			int a=center,b=top;
			if(y < vertices[center].y()) {a=bottom; b=center;}
			
			// float rel = 0.5;
			float rel0 = clamp(linearstep(vertices[a].y(), vertices[b].y(), (float)y));
			float rel1 = clamp(linearstep(vertices[bottom].y(), vertices[top].y(), (float)y));
			
			UniformBuffer uniforms[2];
			uniforms[0].mix(uniformBufs[a],uniformBufs[b],rel0);
			uniforms[1].mix(uniformBufs[bottom],uniformBufs[top],rel1);
			
			Vec3 line[2];
			line[0] = imp::mix(vertices[a],vertices[b],rel0);
			line[1] = imp::mix(vertices[bottom],vertices[top],rel1);

			drawHorizontalLine(line,uniforms,targets,fragCallback);
			// drawWire(line,uniforms,targets,fragCallback);
		}
	}
	
	static void drawTriangleWF(Vec3* vertices, UniformBuffer* uniformBufs, imp::ImageData* targets, FragCallback& fragCallback)
	{
		Vec3 line1[2], line2[2], line3[2];
		UniformBuffer uniforms1[2], uniforms2[2], uniforms3[2];
		
		line1[0] = vertices[0]; line1[1] = vertices[1];
		line2[0] = vertices[0]; line2[1] = vertices[2];
		line3[0] = vertices[1]; line3[1] = vertices[2];
		
		uniforms1[0] = uniformBufs[0]; uniforms1[1] = uniformBufs[1];
		uniforms2[0] = uniformBufs[0]; uniforms2[1] = uniformBufs[2];
		uniforms3[0] = uniformBufs[1]; uniforms3[1] = uniformBufs[2];
		
		drawWire(line1,uniforms1,targets,fragCallback);
		drawWire(line2,uniforms2,targets,fragCallback);
		drawWire(line3,uniforms3,targets,fragCallback);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
void vertexOperations(std::vector<Vec3>& buf, std::vector<Vec3>& col, std::vector<UniformBuffer>& buf_out, VertCallback& vertCallback)
{
	for(int i=0;i<(int)buf.size();++i) vertCallback(buf[i],col[i],buf_out[i]);
}

// -----------------------------------------------------------------------------------------------------------------------
void fragOperations(std::vector<UniformBuffer>& buf, imp::ImageData* targets, FragCallback& fragCallback)
{
	for(int i=0;i<(int)buf.size();i+=3)
	{
		UniformBuffer* uniforms = &buf[i];
		
		Vec3 vertices[3];
		vertices[0] = uniforms[0].get(Varying_MVPVert);
		vertices[1] = uniforms[1].get(Varying_MVPVert);
		vertices[2] = uniforms[2].get(Varying_MVPVert);
		
		Rasterizer::drawTriangle(vertices, uniforms, targets, fragCallback);
	}
}

// -----------------------------------------------------------------------------------------------------------------------
void renderVertex(std::vector<Vec3>& buf, std::vector<Vec3>& col, imp::ImageData* targets, VertCallback& vertCallback, FragCallback& fragCallback)
{       
    // std::cout << "render mesh : 0%";  
    // std::cout << "start" << std::endl;
	
	std::vector<UniformBuffer> uniforms; uniforms.resize(3);
	
	
	// int i = 0;
	for(int i=0;i<(int)buf.size();i+=3)
	{
		std::vector<Vec3> vertices = {buf[i+0], buf[i+1], buf[i+2]};
		std::vector<Vec3> colors = {col[i+0], col[i+1], col[i+2]};
		
		// log(vertices);
		// log(colors);
		
		vertexOperations(vertices,colors, uniforms, vertCallback);
		fragOperations(uniforms,targets,fragCallback);
		// std::cout << "\rrender mesh : " << (i+3)*100.0/buf.size() << "% ";
	}
	// std::cout << "\rrender mesh : 100%";  
	
    // std::cout << "done" << std::endl;
	s_counter++;
}
