
// -----------------------------------------------------------------------------------------------------------------------
struct Varying
{
    std::map<int, int> _adr;
    std::vector<Vec3> _buf;
    
    void push(int id, const Vec3& v3)
    {
        _adr[id] = _buf.size();
        _buf.resize(_buf.size()+1);
        set(id,v3);
    }
    void set(int id, const Vec3& v3)
    {
		if(_adr.find(id) == _adr.end()) push(id, v3);
		else
		{
			int adr = _adr[id];
			_buf[adr] = v3;
		}
    }
    Vec3& get(int id)
    {
        int adr = _adr[id];
        return _buf[adr];
    }
    void copyAdr(const Varying& other)
    {
        std::map<int, int>::const_iterator it;
        for(it = other._adr.begin(); it != other._adr.end(); it++)
        {
            _adr[ it->first ] = it->second;
        }
        _buf.resize(other._buf.size());
    }
    
    void lerp(const Varying& varying_1, const Varying& varying_2, float delta)
    {
        const std::vector<Vec3>& _buf1 = varying_1._buf;
        const std::vector<Vec3>& _buf2 = varying_2._buf;
        
        if(_buf.size() != _buf1.size()) copyAdr(varying_1);
        
        for(unsigned int i=0; i<_buf1.size(); ++i)
        {
            _buf[i] = _buf1[i]*(1.0-delta) + _buf2[i]*delta;
        }
    }
};

struct Interpolator
{
    SortMap sortmap, sortmap2;
    Varying var1,var2,var3;
    Varying xvar1,xvar2;
    Triangle triangleBuf[10];
    Triangle tupleBuf[10];
    Vec3    singleBuf[10];
    Triangle srcVertex;
    Triangle iLine;
    
    int lastX, lastY;
    bool lastAdv;
    
    void setVarying(Triangle* varArr)
    {
        for(int id=0;id<5;++id)
        {
            triangleBuf[id] = varArr[id];
            sortmap.apply(triangleBuf[id]);
            var1.set(id, triangleBuf[id].p1);
            var2.set(id, triangleBuf[id].p2);
            var3.set(id, triangleBuf[id].p3);
        }
    }
    void setVarying2(Triangle* varArr)
    {
        for(int id=0;id<5;++id)
        {
            tupleBuf[id] = varArr[id];
            sortmap2.apply(tupleBuf[id]);
            xvar1.set(id, tupleBuf[id].p1);
            xvar2.set(id, tupleBuf[id].p2);
        }
    }
    
    void init(const Triangle& npc, Triangle* varArr)
    {
        lastX = -1;
        lastY = -1;
        lastAdv = false;
        srcVertex = npc;
        sortmap.init(srcVertex.p1.y(),srcVertex.p2.y(),srcVertex.p3.y());
        setVarying(varArr);
        sortmap.apply(srcVertex);
    }
    
    void advanceY(int x)
    {
        float a = srcVertex.p1.y();
        float b = srcVertex.p2.y();
        float c = srcVertex.p3.y();
        
        float rel = clamp(step(a, c, x)); Varying varX;
        varX.lerp(var1, var3, rel);
        
        iLine.p1 = mix(srcVertex.p1,srcVertex.p3,rel);
        for(int id=0; id<5; ++id) tupleBuf[id].p1 = varX.get(id);
        
        if(x < b)
        {
            float rel = clamp(step(a, b, x));
            varX.lerp(var1, var2, rel);
            iLine.p2 = mix(srcVertex.p1,srcVertex.p2,rel);
        }
        else
        {
            float rel = clamp(step(b, c, x));
            varX.lerp(var2, var3, rel);
            iLine.p2 = mix(srcVertex.p2,srcVertex.p3,rel);
        }
        
        for(int id=0; id<5; ++id) tupleBuf[id].p2 = varX.get(id);
        
        sortmap2.init(iLine[0].x(),iLine[1].x());
        
        setVarying2(tupleBuf);
        sortmap2.apply(iLine);
    }
    
    bool advanceX(int x)
    {
        float rel = step(iLine.p1.x(),iLine.p2.x(), x);
        
        //check if rel > 0 and rel < 1
        if(rel > 0.0 && rel < 1.0)
        {
            Varying varX; varX.lerp(xvar1, xvar2, rel);
            
            for(int id=0; id<5; ++id) singleBuf[id] = varX.get(id);
            return true;
        }
        return false;
    }
    
    bool advance(int x, int y)
    {
        if(y != lastY) { advanceY(y); lastY = y; }
        if(x!= lastX) { lastAdv = advanceX(x); lastX = x; }
        return lastAdv;
    }
    
    Vec3& get(int id)
    {
        return singleBuf[id];
    }
};

// -----------------------------------------------------------------------------------------------------------------------
 struct VertCallback
 {
    virtual Triangle operator()(float* vert_in, float* vert_in2, Triangle* vert_out)  = 0;
 };
// -----------------------------------------------------------------------------------------------------------------------
 struct FragCallback
 {
    virtual void operator()(Interpolator& interpo,int x, int y, imp::ImageData** targetArr)  = 0;
 };
 
// -----------------------------------------------------------------------------------------------------------------------
Vec4 getDrawClipping(Triangle& tri)
{
    float w = state.viewport[2]*0.5;
    float h = state.viewport[3]*0.5;

    Vec4 bounds = tri.getBounds();
    bounds[0] = std::max(bounds[0],-w);
    bounds[1] = std::min(bounds[1],w);
    bounds[2] = std::max(bounds[2],-h);
    bounds[3] = std::min(bounds[3],h);
     
    return bounds;
}

Interpolator interpo2;

// -----------------------------------------------------------------------------------------------------------------------
void renderFragment(Triangle& mvpVertex, imp::ImageData& target, imp::ImageData& back, FragCallback& fragCallback)
{
    Vec4 clipping = getDrawClipping(mvpVertex);
       
    // scanline algorithm
    for(int j=clipping[2]; j<clipping[3]; ++j)
    {
        for(int i=clipping[0]; i<clipping[1]; ++i)
        {
            if(interpo2.advance(i,j))
            {
				 imp::ImageData* targetArr[2]; targetArr[0] = &target; targetArr[1] = &back;
				 fragCallback(interpo2,i,j,targetArr);
            }
        }
    }
}
   
// -----------------------------------------------------------------------------------------------------------------------
void renderVertex(std::vector<float>& buf, std::vector<float>& col, imp::ImageData& target, imp::ImageData& back, VertCallback& vertCallback, FragCallback& fragCallback)
{       
    std::cout << "render mesh : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 9)
    {
        
        Triangle vert_out[10];
        Triangle ndc = vertCallback(&buf[i],&col[i], vert_out); // varArr[Varying_MVPVert];// * win;
        interpo2.init(vert_out[Varying_MVPVert],vert_out);
        renderFragment(ndc, target, back, fragCallback);
          
        std::cout << "\rrender mesh : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
}
