

// -----------------------------------------------------------------------------------------------------------------------
struct Varying
{
    std::map<int, int> _adr;
    std::vector<float> _buf;
    
    void set(int id, float* buf, int size)
    {
		if(_adr.find(id) == _adr.end())
        {
            _adr[id] = _buf.size();
            _buf.resize(_buf.size()+size);
        }
        int adr = _adr[id];
        for(int i=0;i<size;++i)_buf[adr+i] = buf[i];
    }
    
    float* get(int id)
    {
        return &(_buf[_adr[id]]);
    }
    
    void lerp(const Varying& v1, const Varying& v2, float delta)
    {
        if(_buf.size() != v1._buf.size()) { _adr = v1._adr; _buf.resize(v1._buf.size()); }
        
        for(std::uint32_t i=0; i<v1._buf.size(); ++i) _buf[i] = mix(v1._buf[i],v2._buf[i],delta);
    }
};

struct Interpolator
{
    SortMap sortmap, sortmap2;
    Varying var[3], xvar[2], varX;
    Triangle triangleBuf[10], tupleBuf[10], srcVertex, iLine;
    Vec3 singleBuf[10];
    
    int lastX, lastY;
    bool lastAdv;
    
    void setVarying(Triangle* varArr)
    {
        for(int id=0;id<5;++id)
        {
            triangleBuf[id] = varArr[id];
            sortmap.apply(triangleBuf[id]);
            foreach(3, var[i].set(id, triangleBuf[id][i].data(),3) );
        }
    }
    void setVarying2(Triangle* varArr)
    {
        for(int id=0;id<5;++id)
        {
            tupleBuf[id] = varArr[id];
            sortmap2.apply(tupleBuf[id]);
            foreach(2, xvar[i].set(id, tupleBuf[id][i].data(),3) );
        }
    }
    
    void init(const Triangle& npc, Triangle* varArr)
    {
        lastX = -1;
        lastY = -1;
        lastAdv = false;
        srcVertex = npc;
        sortmap.init(srcVertex[0].y(),srcVertex[1].y(),srcVertex[2].y());
        setVarying(varArr);
        sortmap.apply(srcVertex);
    }
	
	void computeIntersect(int side, int p1, int p2, float x)
	{
        float rel = clamp(step(srcVertex[p1].y(), srcVertex[p2].y(), x));
        varX.lerp(var[p1], var[p2], rel);
        iLine[side] = mix(srcVertex[p1],srcVertex[p2],rel);
        foreach((int)varX._adr.size(), tupleBuf[i][side] = varX.get(i) );
	}
    
    void advanceY(int x)
    {        
		int a = 1; int b = 2;
		if(x < srcVertex[1].y()) {a=0; b=1;}
		
		computeIntersect(0, 0, 2, x);
		computeIntersect(1, a, b, x);
        
        sortmap2.init(iLine[0].x(),iLine[1].x());
        
        setVarying2(tupleBuf);
        sortmap2.apply(iLine);
    }
    
    bool advanceX(int x)
    {
        float rel = step(iLine[0].x(),iLine[1].x(), x);
        
        //check if rel > 0 and rel < 1
        if(rel >= 0.0 && rel <= 1.0)
        {
            Varying varX; varX.lerp(xvar[0], xvar[1], rel);
            
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
    Vec4 bounds = tri.getBounds();
    bounds[0] = std::max(bounds[0], state.viewport[0]);
    bounds[1] = std::min(bounds[1], state.viewport[2]);
    bounds[2] = std::max(bounds[2], state.viewport[1]);
    bounds[3] = std::min(bounds[3], state.viewport[3]);
     
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
