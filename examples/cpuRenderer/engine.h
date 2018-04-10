

float modulation(float b1, float b2, float p)
{
    float res  = (p-b1)/(b2-b1);
    res = res<0.0?0.0:res;
    res = res>1.0?1.0:res;
    return res;
}

struct Interpolator2
{
    struct Tuple
    {
        Vec3 p1;
        Vec3 p2;
    };
    
    Vec4 sortmap, sortmap2;
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
            applySort(triangleBuf[id], sortmap);
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
            applySort(tupleBuf[id], sortmap2);
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
        initSort(srcVertex.p1.y(),srcVertex.p2.y(),srcVertex.p3.y(), sortmap);
        setVarying(varArr);
        applySort(srcVertex,sortmap);
    }
    
    void advanceY(int x)
    {
        float a = srcVertex.p1.y();
        float b = srcVertex.p2.y();
        float c = srcVertex.p3.y();
        
        float rel = modulation(a, c, x); Varying varX;
        varX.lerp(var1, var3, rel);
        
        iLine.p1 = lerp(srcVertex.p1,srcVertex.p3,rel);
        for(int id=0; id<5; ++id) tupleBuf[id].p1 = varX.get(id);
        
        if(x < b)
        {
            float rel = modulation(a, b, x);
            varX.lerp(var1, var2, rel);
            iLine.p2 = lerp(srcVertex.p1,srcVertex.p2,rel);
        }
        else
        {
            float rel = modulation(b, c, x);
            varX.lerp(var2, var3, rel);
            iLine.p2 = lerp(srcVertex.p2,srcVertex.p3,rel);
        }
        
        for(int id=0; id<5; ++id) tupleBuf[id].p2 = varX.get(id);
        
        // initSort(yNpc.p1.x(),yNpc.p2.x(), sortmap2);
        initSort(iLine[0].x(),iLine[1].x(),sortmap2);
        
        setVarying2(tupleBuf);
        applySort(iLine, sortmap2);
    }
    
    bool advanceX(int x)
    {
        float rel = modulation(iLine.p1.x(),iLine.p2.x(), x);
        
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
        if(y != lastY)
        {
            advanceY(y);
            lastY = y;
        }
        if(x!= lastX)
        {
            lastAdv = advanceX(x);
            lastX = x;
        }
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
    virtual Vec4 operator()(Interpolator2& interpo)  = 0;
 };
 
// -----------------------------------------------------------------------------------------------------------------------
Vec4 computeBounds(Triangle& tri)
{
    float boundA_x = tri.p1.x();
    float boundA_y = tri.p1.y();
    float boundB_x = tri.p1.x();
    float boundB_y = tri.p1.y();
     
    if(tri.p2.x() < boundA_x ) { boundA_x = tri.p2.x();}
    if(tri.p3.x() < boundA_x ) { boundA_x = tri.p3.x();}
                                                                                     
    if(tri.p2.y() < boundA_y ) { boundA_y = tri.p2.y();}
    if(tri.p3.y() < boundA_y ) { boundA_y = tri.p3.y();}
                                                                                     
    if(tri.p2.x() > boundB_x ) { boundB_x = tri.p2.x();}
    if(tri.p3.x() > boundB_x ) { boundB_x = tri.p3.x();}
                                                                                     
    if(tri.p2.y() > boundB_y ) { boundB_y = tri.p2.y();}
    if(tri.p3.y() > boundB_y ) { boundB_y = tri.p3.y();}
     
    float w = state.viewport[2]*0.5;
    float h = state.viewport[3]*0.5;
     
    if(boundA_x < -w) boundA_x = -w;
    if(boundA_y < -h) boundA_y= -h;
    if(boundB_x > w) boundB_x = w;
    if(boundB_y > h) boundB_y = h;
     
    return Vec4(boundA_x,boundB_x,boundA_y,boundB_y);
}

Interpolator2 interpo2;

// -----------------------------------------------------------------------------------------------------------------------
void renderFragment(Triangle& mvpVertex, imp::ImageData& target, imp::ImageData& back, FragCallback& fragCallback)
{
    Vec4 bounds = computeBounds(mvpVertex);
    
    float w = state.viewport[2]*0.5;
    float h = state.viewport[3]*0.5;
       
    // scanline algorithm
    for(int j=bounds[2]; j<bounds[3]; ++j)
    {
        for(int i=bounds[0]; i<bounds[1]; ++i)
        {
            if(interpo2.advance(i,j))
            {
                // depth test
                float new_depth = (std::min(1.0, abs(interpo2.get(Varying_MVVert).z()-0.1) / 20.0)) * 255.0;
                float curr_depth = back.getPixel(i+w, j+h)[0];
                if( new_depth < curr_depth)
                {
                     
                    Vec4 color = fragCallback(interpo2) * 255.0;
                     
                    target.setPixel(i+w, j+h, color);
                    Vec4 depthPx(new_depth,0.0,0.0,1.0);
                    back.setPixel(i+w, j+h, depthPx);
                }
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
