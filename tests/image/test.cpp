#include <Graphics/Image.h>
#include <Graphics/Rasterizer.h>
#include <Descriptors/ImageIO.h>

void hsym(imp::Image& image, const imp::Vec4& col)
{
    for(int i=0;i<image.width();++i)
    {
        for(int j=0;j<image.height();++j)
        {
            imp::Vec4 cc = image.getPixel(i,j); cc[3] = 255.0;
            if(cc == col)image.setPixel(image.width()-1-i,j,col);
        }
    }
}

int main(int argc, char* argv[])
{
    imp::Image::Ptr image = imp::Image::create(11,8,3);
    
    imp::Vec4 bgcol(50,128,200,255);
    imp::Vec4 fgcol(255,128,200,255);
        
    /*Image image2(4,4,4);
    image2.fill(imp::Vec4(54,234,75,255));
    image.draw(image2, imp::IntRect(2,3,4,4));*/
    
    imp::Rasterizer rast;
    rast.setTarget(image);
    rast.setColor(fgcol);
    
    float offset = 5.0;
    image->fill(bgcol);
    
    rast.square(imp::Vec3(offset+2.0,4.0,0.0),4.0);
    rast.grid(imp::Vec3(offset+2.0,0.0,0.0),imp::Vec3(offset+3.0,7.0,0.0),1);
    rast.rectangle(imp::Vec3(offset+5.0,1.0,0.0),imp::Vec3(offset+5.0,3.0,0.0));
    rast.rectangle(imp::Vec3(offset+4.0,3.0,0.0),imp::Vec3(offset+4.0,4.0,0.0));
    rast.dot(imp::Vec3(offset+1.0,0.0,0.0));
    
    rast.setColor(bgcol);
    rast.dot(imp::Vec3(offset+2.0,4.0,0.0));
    
    hsym(*image.get(),fgcol);
    imp::ImageIO::save(image,"out.tga");
    
    return 0;
}
