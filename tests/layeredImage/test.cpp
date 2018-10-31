#include <Texture/LayeredImage.h>
#include <fstream>
// ----------------------------------------------------------
template<typename Ty>
void printLayer(const imp::Layer<Ty>& layer)
{
    std::cout << "layer size = " << layer.size()[0] << ";" << layer.size()[1] << " (" << layer.size()[0]*layer.size()[1] << ")" << std::endl;
    
	imp::Dimension<2> pos;
    for(int x=0; x<layer.size()[0];++x)
    {
        pos[0] = x;
        for(int y=0; y<layer.size()[1];++y)
        {
            pos[1] = y;
            std::cout << (int)layer.get(pos) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "end layer" << std::endl;
}


template<typename Ty>
void interlace(const imp::LayeredImage& src, std::vector<Ty>& out, const std::vector<int>& mask)
{
	imp::Dimension<2> S((int)src.width(),(int)src.height());
	unsigned int chnls = src.chnlCount();
	chnls = std::min(chnls,(unsigned int)mask.size());
	out.resize(S[0] * S[1] * chnls);
	
	unsigned int outIndex = 0;
	
	for(int i=0; i<S[0]; ++i)
	{
		for(int j=0; j<S[1]; ++j)
		{
			imp::Vec4 val = src.get(i,j);
			for(unsigned int k=0;k<chnls;++k)out[outIndex + mask[k] ] = val[k];
			outIndex += chnls;
		}
	}
}


int main(int argc, char* argv[])
{
	imp::LayeredImage image;
	image.build( 8,11,{1,1,1,1} );	// refacto chnl size
	
	std::cout << "channel count = " << image.chnlCount() << std::endl;
	
	imp::Vec4 fillv(50,128,200,255);
	image.fill(fillv);
	
	 imp::Vec4 val = image.get(7,3);
	 std::cout << "val = " << val[0] << ";" << val[1] << ";" << val[2] << ";" << val[3] << std::endl;
	 std::cout << "should be (50,128,200,250)" << std::endl;
	 val = imp::Vec4(70,80,90,100);
	 image.set(7,3,val);
	 val = imp::Vec4(50,128,200,255);
	 val = image.get(7,3);
	 std::cout << "val = " << val[0] << ";" << val[1] << ";" << val[2] << ";" << val[3] << std::endl;
	 std::cout << "should be (70,80,90,100)" << std::endl;
	 
	 
	 imp::LayeredImage image2;
	 image2.build(4,4,{1,1,1,1} );
	 image2.fill(imp::Vec4(54,234,75,255));
	 
	 std::cout << "before draw :" << std::endl;
	 printLayer(*image.getLayer<std::uint8_t>(0));
	 std::cout << "drawing :" << std::endl;
	 printLayer(*image2.getLayer<std::uint8_t>(0));
	 image.draw(image2, imp::IntRect(2,3,4,4));
	 std::cout << "after draw :" << std::endl;
	 printLayer(*image.getLayer<std::uint8_t>(0));
	 
	 imp::Vec4 monsterColor(255,128,200,255);
	 image.set(0,3,monsterColor);
	 image.set(0,4,monsterColor);
	 image.set(0,6,monsterColor);
	 image.set(0,7,monsterColor);
	 image.set(1,0,monsterColor);
	 image.set(1,2,monsterColor);
	 image.set(1,8,monsterColor);
	 image.set(1,10,monsterColor);
	 image.set(2,0,monsterColor);
	 image.set(2,2,monsterColor);
	 image.set(2,3,monsterColor);
	 image.set(2,4,monsterColor);
	 image.set(2,5,monsterColor);
	 image.set(2,6,monsterColor);
	 image.set(2,7,monsterColor);
	 image.set(2,8,monsterColor);
	 image.set(2,10,monsterColor);
	 image.set(3,0,monsterColor);
	 image.set(3,1,monsterColor);
	 image.set(3,2,monsterColor);
	 image.set(3,3,monsterColor);
	 image.set(3,4,monsterColor);
	 image.set(3,5,monsterColor);
	 image.set(3,6,monsterColor);
	 image.set(3,7,monsterColor);
	 image.set(3,8,monsterColor);
	 image.set(3,9,monsterColor);
	 image.set(3,10,monsterColor);
	 image.set(4,1,monsterColor);
	 image.set(4,2,monsterColor);
	 image.set(4,4,monsterColor);
	 image.set(4,5,monsterColor);
	 image.set(4,6,monsterColor);
	 image.set(4,8,monsterColor);
	 image.set(4,9,monsterColor);
	 image.set(5,2,monsterColor);
	 image.set(5,3,monsterColor);
	 image.set(5,4,monsterColor);
	 image.set(5,5,monsterColor);
	 image.set(5,6,monsterColor);
	 image.set(5,7,monsterColor);
	 image.set(5,8,monsterColor);
	 image.set(6,3,monsterColor);
	 image.set(6,7,monsterColor);
	 image.set(7,2,monsterColor);
	 image.set(7,8,monsterColor);
	 
	 
	 std::vector<unsigned char> out;
	 std::vector<int> outChnls = {0,1,2};
	 interlace<unsigned char>(image,out, outChnls);
	 
	 // save value as tga
	 {
		std::ofstream ostrm("out.tga",std::ios::binary);
        unsigned char v = 0;
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); //ID length (1o)
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Color map type (1o)
        if(outChnls.size() == 1) v = 3; // black and white
        if(outChnls.size() == 3 || outChnls.size() == 4) v = 2;	// TrueColor
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Image type (1o)
        v = 0;	// no color map
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
        unsigned short d = 0;
        ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // X-origin (2o)
        ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // Y-origin (2o)
        d = image.height();
        ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // width (2o)
        d = image.width();
        ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // height (2o)
        // v = 8;	// default for black and white
        v = outChnls.size() * 8;	// 24 for RGB; 32 for RGBA
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // px depth (1o)
        v = 0;
        ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // description (1o)
        ostrm.write(reinterpret_cast<char*>(out.data()), (image.width()*image.height()*outChnls.size())); // image data
	 }
	 
	
	return 0;
}
