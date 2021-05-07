#include <ImpGears/Core/Grid.h>

using namespace imp;


// ----------------------------------------------------------
void test_chunk(Chunk<float>& chunk, Dimension<3>& p3)
{
    std::cout << "p3 size = " << p3.size() << std::endl;
    std::cout << "chunk size = " << chunk.size()[0] << ";" << chunk.size()[1] << ";" << chunk.size()[2] << " ()" << std::endl;
    
    Dimension<3> pos;
    float val = 0.0;
    for(int x=0; x<p3[0];++x)
    {
        pos[0] = x;
        for(int y=0; y<p3[1];++y)
        {
            pos[1] = y;
            for(int z=0; z<p3[2];++z)
            {
                pos[2] = z;
                chunk.set(pos,(val++)/200.0);
            }            
        }
    }
    
    pos[0] = 2; pos[1] = 1; pos[2] = 4;
    std::cout << "pull test = " << chunk.get(pos) << " from " << pos[0] << ";" << pos[1] << ";" << pos[2] << "(" << chunk.index(pos) << ")" << std::endl;
    Dimension<3> check_pos = chunk.vector(chunk.index(pos));
    std::cout << "check pos <=> index : " << check_pos[0] << ";" << check_pos[1] << ";" << check_pos[2] << std::endl;
    
    for(int x=0; x<p3[0];++x)
    {
        pos[0] = x;
        for(int y=0; y<p3[1];++y)
        {
            pos[1] = y;
            for(int z=0; z<p3[2];++z)
            {
                pos[2] = z;
                std::cout << chunk.get(pos) << " ";
            }
            std::cout << std::endl;
        }
    }
}

// ----------------------------------------------------------
void test_layer(Layer<char>& layer, Dimension<2>& p2)
{
    std::cout << "p2 size = " << p2.size() << std::endl;
    std::cout << "layer size = " << layer.size()[0] << ";" << layer.size()[1] << " ()" << std::endl;
    
    Dimension<2> pos;
    float val = 0.0;
    for(int x=0; x<p2[0];++x)
    {
        pos[0] = x;
        for(int y=0; y<p2[1];++y)
        {
            pos[1] = y;
            layer.set(pos,val+=1);         
        }
    }
    
    pos[0] = 2; pos[1] = 7;
	unsigned int index = layer.index(pos) ;
    std::cout << "pull test ===	 " << (int)layer.get(pos) << " from " << pos[0] << ";" << pos[1] << "(" << index << ")" << std::endl;
    imp::Dimension<2> check_pos = layer.vector(index);
    std::cout << "check pos <=> index : " << check_pos[0] << ";" << check_pos[1] << std::endl;
    
    for(int x=0; x<p2[0];++x)
    {
        pos[0] = x;
        for(int y=0; y<p2[1];++y)
        {
            pos[1] = y;
            std::cout << (int)layer.get(pos) << " ";
        }
        std::cout << std::endl;
    }
    
    {
		Dimension<2> pos00(0,0);
		Dimension<2> pos01(0,1);
		Dimension<2> pos10(1,0);
		
		std::cout << "pos00 = " << (int)layer.get(pos00) << std::endl;
		std::cout << "pos01 = " << (int)layer.get(pos01) << std::endl;
		std::cout << "pos10 = " << (int)layer.get(pos10) << std::endl;
	}
}


int main(int argc,char* argv[])
{
    Dimension<2> p2; p2[0] = 5; p2[1] = 10;
    Dimension<3> p3; p3[0] = 3; p3[1] = 4; p3[2] = 6;
    
    Layer<char> layer(p2);
    Chunk<float> chunk(p3);
    
    test_layer(layer,p2);
	test_chunk(chunk,p3);
}

