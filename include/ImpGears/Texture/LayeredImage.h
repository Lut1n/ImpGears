#ifndef IMP_LAYEREDIMAGE_H
#define IMP_LAYEREDIMAGE_H

#include <Core/Object.h>
#include <Core/Vec4.h>
#include <Core/Grid.h>

IMPGEARS_BEGIN

class IMP_API LayeredImage : public Object
{
    public:

	// ----------------------------------------------------------
	using b8_t = std::uint8_t;
	using b16_t = std::uint16_t;
	using b32_t = std::uint32_t;
	using b64_t = std::uint64_t;
	
	static int b8;
	static int b16;
	static int b32;
	static int b64;
	
	// ----------------------------------------------------------
	class Channel
	{
	public:
		Meta_Class(Channel)
		Channel(int depth) : _depth(depth) {}
		int size() const {return _depth;}
		
	protected:
		int _depth;
	};

	// ----------------------------------------------------------
	template <typename Ty>
	class ChannelLayer : public Channel, public Layer<Ty>
	{
	public:
		
		Meta_Class(ChannelLayer)
		
		ChannelLayer(const Dimension<2>& dim)
			: Channel(sizeof(Ty))
			, Layer<Ty>(dim)
		{
		}
		
		virtual ~ChannelLayer()
		{
		}
	};

	// ----------------------------------------------------------
	Meta_Class(LayeredImage)

	LayeredImage();
	virtual ~LayeredImage();

	void build(int w, int h, int chnlCount = 4, int chnlSize = 8);
	void build(int w, int h, const std::vector<int>& chnls);
	void destroy();

	Channel::Ptr getLayer(int chnl) const{return _layers[chnl]; }
	
	template<typename chnlSize>
	ChannelLayer<chnlSize>* getLayer(int chnl) const
	{
		if(_layers[chnl]->size() == sizeof(chnlSize))
		{
			return dynamic_cast< ChannelLayer<chnlSize>* >( _layers[chnl].get() ); 
		}
		
		return nullptr;
	}

	void clone(const LayeredImage& other);
	void resize(int w, int h, int xrel=0, int yrel=0);
	void draw(const LayeredImage& srcData, const IntRect& dstRect);

	void fill(const Vec4& px);
	
	std::uint64_t get(int x, int y, int ch) const;
	void set(int x, int y, int ch, std::uint64_t val);

	Vec4 get(int x, int y) const;
	void set(int x, int y, const Vec4& px);

	std::uint32_t width() const{return _width;}
	std::uint32_t height() const{return _height;}
	std::uint32_t chnlCount() const{return _layers.size();}
	std::vector<int> chnlSize() const
	{
		std::vector<int> sizes; sizes.resize(_layers.size());
		for(unsigned int i=0; i<_layers.size();++i) sizes[i] = _layers[i]->size();
		return sizes;
	}

    protected:
		
	std::vector< Channel::Ptr >	_layers;
	std::uint32_t _width;
	std::uint32_t _height;
};

IMPGEARS_END

#endif // IMP_LAYEREDIMAGE_H
