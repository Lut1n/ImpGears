#include <sstream>

#include "Graphics/BmpLoader.h"
#include "Graphics/DefaultShader.h"
#include "Graphics/GraphicRenderer.h"
#include "Graphics/RenderTarget.h"

#include "Core/Timer.h"
#include "Core/State.h"

#include "System/SFMLContextInterface.h"
#include "Graphics/ScreenVertex.h"

#include <System/FileInfo.h>

#include "Gui/GuiImage.h"
#include "Gui/GuiText.h"
#include "Gui/GuiButton.h"
#include "Gui/Line2DGeometry.h"
#include "Gui/GuiContainer.h"
#include "Gui/GuiSlider.h"
#include "Gui/GuiScrollbar.h"
#include "Gui/GuiBox.h"
#include "Gui/GuiCheckBox.h"
#include "Gui/GuiList.h"
#include "Gui/GuiLink.h"
#include "Gui/GuiEditText.h"
#include "Gui/GuiComboBox.h"
#include "Gui/GuiState.h"
#include "Gui/GuiShader.h"
#include "Gui/GuiEventHandler.h"

#include <Data/ImageUtils.h>
#include <Math/Perlin.h>
#include <Math/BasicFunctions.h>

using namespace imp;

//--------------------------------------------------------------
static GuiComponent* s_guiRoot = IMP_NULL;

//--------------------------------------------------------------
GuiLinkedBox* buildBox(const std::string& name, GuiImage** i, GuiButton** paramsButton, const std::vector<std::string>& iImg, const std::vector<std::string>& oImg);

class ValueHandler
{
	public:
	
	virtual void onNumericValueChanged(const std::string& key, double v) {}
	virtual void onStringValueChanged(const std::string& key, const std::string& v) {}
};

class Dirtable
{
	public:
	
	Dirtable()
	: _dirty(false)
	{}
	
	virtual ~Dirtable()
	{}
	
	virtual void makeDirty() {_dirty = true;}
	virtual bool isDirty() const {return _dirty;}
	
	protected:
		bool _dirty;
};

class EnumSelector : public GuiComponent
{
	public:
	EnumSelector(const std::string& name, const std::vector<std::string>& e, ValueHandler* handler)
	: GuiComponent(0.0,0.0,0.0,0.0)
	, _name(name)
	, _handler(handler)
	, _index(0.0)
	, _e(e)
	{
		GuiButton* down = new GuiButton();
		_text = new GuiText();
		GuiButton* up = new GuiButton();

		class ButtonBehaviour : public GuiEventHandler
		{
			public:
			ButtonBehaviour(EnumSelector* check, double dir)
				: _check(check)
				, _dir(dir)
				, _pressed(false)
			{}
			~ButtonBehaviour(){}
			
			bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
			{
				if(over)
				{
					_pressed = true;
					return true;
				}
				return false;
			}
			
			bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
			{
				if(over && _pressed)
				{
					_check->doAction( _dir );
				}
				_pressed = false;
				return false;
			}

			private:
			EnumSelector* _check;
			double _dir;
			bool _pressed;
		};

		down->addEventHandler(new ButtonBehaviour(this, -1.0));
		up->addEventHandler(new ButtonBehaviour(this, 1.0));
		
		Layout::Parameters params;
		
		params.setAlignementX(Layout::Alignement_Center);
		_text->setLayoutParameters(params);
		_text->setSize(50.0, 20.0);
		params.setResizingY(Layout::Resizing_Fill);
		params.setAlignementX(Layout::Alignement_Begin);
		down->setLayoutParameters(params);
		down->setSize(20.0,20.0);
		params.setAlignementX(Layout::Alignement_End);
		params.setResizingX(Layout::Resizing_None);
		up->setLayoutParameters(params);
		up->setSize(20.0,20.0);
		
		compose(_text);
		compose(down);
		compose(up);
	}

	//--------------------------------------------------------------
	~EnumSelector()
	{
	}
	
	void doAction(double dir)
	{
		_index += dir;
		if(_index < 0)
			_index = _e.size()-1.0;
		else if(_index >= _e.size())
			_index = 0.0;
		
		_text->setText( getStringValue().c_str() );
		_handler->onStringValueChanged(_name, getStringValue().c_str());
	}
			
	std::string getStringValue() const {return _e[_index];}
	void setStringValue(const std::string& v)
	{
		for(unsigned int i=0; i<_e.size(); ++i)
		{
			if(_e[i] == v)
			{
				_index = i;
				_text->setText( getStringValue().c_str() );
				break;
			}
		}
	}
	
	protected :
	
	std::string _name;
	ValueHandler* _handler;
	GuiText* _text;
	double _index;
	std::vector<std::string> _e;
};

class NumericSelector : public GuiComponent
{
	public:
	NumericSelector(const std::string& name, ValueHandler* handler)
	: GuiComponent(0.0,0.0,0.0,0.0)
	, _name(name)
	, _handler(handler)
	, _numericValue(0.0)
	{
		GuiButton* down = new GuiButton();
		_text = new GuiText();
		GuiButton* up = new GuiButton();

		class ButtonBehaviour : public GuiEventHandler
		{
			public:
			ButtonBehaviour(NumericSelector* check, double dir)
				: _check(check)
				, _dir(dir)
				, _pressed(false)
			{}
			~ButtonBehaviour(){}
			
			bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
			{
				if(over)
				{
					_pressed = true;
					return true;
				}
				return false;
			}
			
			bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
			{
				if(over && _pressed)
				{
					_check->doAction( _dir );
				}
				_pressed = false;
				return false;
			}

			private:
			NumericSelector* _check;
			double _dir;
			bool _pressed;
		};

		down->addEventHandler(new ButtonBehaviour(this, -0.1));
		up->addEventHandler(new ButtonBehaviour(this, 0.1));
		
		Layout::Parameters params;
		
		params.setAlignementX(Layout::Alignement_Center);
		_text->setLayoutParameters(params);
		_text->setSize(50.0, 20.0);
		params.setResizingY(Layout::Resizing_Fill);
		params.setAlignementX(Layout::Alignement_Begin);
		down->setLayoutParameters(params);
		down->setSize(20.0,20.0);
		params.setAlignementX(Layout::Alignement_End);
		params.setResizingX(Layout::Resizing_None);
		up->setLayoutParameters(params);
		up->setSize(20.0,20.0);
		
		compose(_text);
		compose(down);
		compose(up);
	}

	//--------------------------------------------------------------
	~NumericSelector()
	{
	}
	
	void doAction(double dir)
	{
		_numericValue += dir;
		std::ostringstream ss;
		ss << _numericValue;
		_text->setText( ss.str().c_str() );
		
		_handler->onNumericValueChanged(_name, _numericValue);
	}
			
	double getNumericValue() const {return _numericValue;}
	void setNumericValue(double v)
	{
		_numericValue=v;
		
		std::ostringstream ss;
		ss << _numericValue;
		_text->setText( ss.str().c_str() );
	}
	
	protected :
	
	std::string _name;
	ValueHandler* _handler;
	GuiText* _text;
	double _numericValue;
};

//--------------------------------------------------------------
class FunctionNodeParams : public ValueHandler
{
	public:
	
	enum Type
	{
		Type_Numeric = 0,
		Type_String
	};
	
	virtual void onNumericValueChanged(const std::string& key, double v)
	{
		_numericMap[key] = v;
		_dirtable->makeDirty();
	}
	
	virtual void onStringValueChanged(const std::string& key, const std::string& v)
	{
		_stringMap[key] = v;
		_dirtable->makeDirty();
	}
	
	FunctionNodeParams(Dirtable* dirtable)
		: _dirtable(dirtable)
	{
		_gui = new GuiBox();
		
		Layout::Parameters param;
		param.setAlignementX(Layout::Alignement_Center);
		param.setPositionningY(Layout::Positionning_Auto);
		param.setResizingX(Layout::Resizing_Fill);
		
		GuiButton* button = new GuiButton();
		button->setText("ok");
		button->setSize(0.0,20.0);
		button->setLayoutParameters(param);
		_gui->addComponent(button);
		
		class ButtonBehaviour : public GuiEventHandler
		{
			public:
			ButtonBehaviour(GuiBox* check)
				: _check(check)
			{}
			~ButtonBehaviour(){}
			
			bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
			{
				if(over)
				{
					_check->setVisible( false );
				}
				return false;
			}

			private:
			GuiBox* _check;
		};
		
		button->addEventHandler(new ButtonBehaviour(_gui));
		
		_gui->setTitle("parameters");
		_gui->setSize(110.0, 150.0);
		_gui->setPosition(100.0,150.0);
		_gui->enableScrollbar(true);
	}
	
	void buildView()
	{
		
		initializeParams();
		
		Layout::Parameters param;
		param.setAlignementX(Layout::Alignement_Center);
		param.setPositionningY(Layout::Positionning_Auto);
		
		std::map<std::string, std::vector<std::string>* >::iterator sit;
		for(sit=_stringEnum.begin(); sit!=_stringEnum.end(); sit++)
		{
			std::vector<std::string>* e = sit->second;
			
			EnumSelector* cc = new EnumSelector(sit->first, *e, this);
			cc->setSize(0.0,20.0);
			param.setAlignementX(Layout::Alignement_Center);
			param.setResizingX(Layout::Resizing_Fill);
			cc->setLayoutParameters(param);
			cc->setStringValue(_stringMap[sit->first]);
			
			GuiText* text = new GuiText();
			text->setSize(0.0,20.0);
			param.setAlignementX(Layout::Alignement_Begin);
			param.setResizingX(Layout::Resizing_None);
			text->setLayoutParameters(param);
			text->setText(sit->first);
			
			_gui->addComponent(text);
			_gui->addComponent(cc);
		}
		
		std::map<std::string, double >::iterator dit;
		for(dit=_numericMap.begin(); dit!=_numericMap.end(); dit++)
		{
			NumericSelector* cc = new NumericSelector(dit->first, this);
			cc->setSize(0.0,20.0);
			param.setAlignementX(Layout::Alignement_Center);
			param.setResizingX(Layout::Resizing_Fill);
			cc->setLayoutParameters(param);
			cc->setNumericValue(_numericMap[dit->first]);
			
			GuiText* text = new GuiText();
			text->setSize(0.0,20.0);
			param.setAlignementX(Layout::Alignement_Begin);
			param.setResizingX(Layout::Resizing_None);
			text->setLayoutParameters(param);
			text->setText(dit->first);
			
			_gui->addComponent(text);
			_gui->addComponent(cc);
		}
	}
	
	virtual ~FunctionNodeParams()
	{
		delete _gui;
	}
	
	virtual void initializeParams(){};
	
	double getNumeric(const std::string& id) {return _numericMap.at(id);}
	std::string getString(const std::string& id) {return _stringMap[id];}
	
	void addNumeric(std::string id, double dft = 0.0){_numericMap[id] = dft;}
	void addEnum(std::string id, std::vector<std::string>& e, std::string dft)
	{
		std::vector<std::string>* v = new std::vector<std::string>();
		
		for(unsigned int i=0; i<e.size(); ++i)
		{
			v->push_back(e[i]);
		}
		
		_stringEnum[id] = v;
		_stringMap[id] = dft;
	}
	
	Dirtable* _dirtable;
	std::map<std::string, std::vector<std::string>* > _stringEnum;
	std::map<std::string, std::string> _stringMap;
	std::map<std::string, double> _numericMap;
	
	GuiBox* _gui;
};

//--------------------------------------------------------------
class FunctionNode : public Dirtable
{
	public:
	
	FunctionNode();
	
	virtual ~FunctionNode();
	
	virtual void setup() = 0;
	virtual void apply() = 0;
	virtual FunctionNode* clone() = 0;
	
	
	void buildView(const std::string& title, const std::vector<std::string>& i, const std::vector<std::string>& o);
	
	void addImage(const std::string& name, int id)
	{
			_nameImg[name] = id;
	}
	
	ImageData* getImage(int id)
	{
		ImageData* img = IMP_NULL;
		
		try 
		{ 
			img = _imgIn.at(id);
		} 
		catch ( const std::out_of_range & ) 
		{ 
			img = IMP_NULL;
		}
		return img;
	}
	
	
	ImageData* getImage(const std::string& name)
	{
		ImageData* img = IMP_NULL;
		
		try 
		{ 
			int id = _nameImg.at(name);
			if(id<0)id = -id;
			return getImage(id);
		} 
		catch ( const std::out_of_range & ) 
		{ 
			img = IMP_NULL;
		}
		return img;
	}
	
	GuiButton* _paramsButton;
	GuiLinkedBox* _guiNode;
	GuiImage* _guiImage;
	ImageData* _image;
	std::map<int, FunctionNode*> _dependances;
	
	std::map<int, ImageData*> _imgIn;
	std::map<std::string, int> _nameImg;
	
	double getNumericParameter(const std::string& key);
	std::string getStringParameter(const std::string& key);
	
	void addDependance(int id, FunctionNode* node);
	void removeDependance(FunctionNode* node);
	void removeDependance(int id);
	
	FunctionNodeParams* _paramsView;
};


//--------------------------------------------------------------
class EditorGraph : public GuiLinkHandler, public Dirtable
{
	public:
	
	EditorGraph();
	
	virtual ~EditorGraph();
	
	void addFunctionNode(const std::string& funcName);
	
	void removeFunctionNode(FunctionNode* node);
	
	void updateAllNode();
	
	virtual bool onLinkCreate(GuiEventSource* c1, int id1, GuiEventSource* c2, int id2);
	
	virtual bool onLinkDelete(GuiEventSource* c1, int id1);
	
	GuiComponent* _viewer;
	GuiComponent* _selector;
	GuiImage* _imageView;
	std::map<std::string, FunctionNode*> _dict;
	
	GuiBox* _guiGraph;
	std::vector<FunctionNode*> _nodes;
	
	static EditorGraph* getInstance() {if(_instance == IMP_NULL)_instance = new EditorGraph(); return _instance;}
	
	static EditorGraph* _instance;
};

//--------------------------------------------------------------
class SelectorAction : public GuiEventHandler
{
	public:
	
	SelectorAction(const std::string& functorName)
		: GuiEventHandler()
		, _functorName(functorName)
	{}
	
	virtual bool onMouseReleased(GuiEventSource* evnSrc, bool over, int buttonID, float x, float y, bool action)
	{
		if(over)
		{
			EditorGraph::_instance->addFunctionNode(_functorName);
			EditorGraph::_instance->_selector->setVisible(false);
			return true;
		}
		return false;
	}
	
	std::string _functorName;
};

//--------------------------------------------------------------
GuiComponent* buildSelector()
{
	Layout::Parameters param;
	param.setAlignementX(Layout::Alignement_Center);
	param.setPositionningY(Layout::Positionning_Auto);
	param.setResizingX(Layout::Resizing_Fill);
	
	GuiBox* root = new GuiBox();
	
	/*for(unsigned int i=0; i<10; ++i)
	{
		GuiButton* button = new GuiButton();
		button->setSize(0.0,30.0);
		button->setLayoutParameters(param);
		root->addComponent(button);
	}*/
	GuiButton* button = new GuiButton();
	button->setSize(0.0,30.0);
	button->setLayoutParameters(param);
	button->addEventHandler(new SelectorAction("sin") );
	button->setText("sin");
	root->addComponent(button);
	
	button = new GuiButton();
	button->setSize(0.0,30.0);
	button->setLayoutParameters(param);
	button->addEventHandler(new SelectorAction("noise") );
	button->setText("noise");
	root->addComponent(button);
	
	root->setTitle("Node list");
	root->setPosition((800-200)/2.0, (600-300)/2.0);
	root->setSize(200.0, 300.0);
	root->enableScrollbar(true);
	
	return root;
}

//--------------------------------------------------------------
GuiComponent* buildViewer(GuiImage** i)
{
	Layout::Parameters param;
	param.setAlignementX(Layout::Alignement_Center);
	param.setAlignementY(Layout::Alignement_Center);
	
	GuiBox* viewer = new GuiBox();
	viewer->setPosition(150.0,50.0);
	viewer->setSize(500.0,500.0);
	
	class ImgViewerAction : public GuiEventHandler
	{
		public:
		
		ImgViewerAction(GuiBox* v)
			: _v(v)
		{}
		
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over)
				_v->setVisible(false);
			return false;
		}
		
		protected:
		
		GuiBox* _v;
	};

	(*i) = new GuiImage();
	(*i)->setImage("test.bmp");
	(*i)->setSize(500.0,500.0);
	(*i)->setLayoutParameters(param);
	viewer->addComponent(*i);
	viewer->addEventHandler(new ImgViewerAction(viewer));
	
	return viewer;
}

//--------------------------------------------------------------
GuiBox* buildGui()
{
	GuiBox* root = new GuiBox();
	root->setSize(800.0-10.0, 600.0-10.0);
	root->enableScrollbar(true);
	
	return root;
}

//--------------------------------------------------------------
class SinNode : public FunctionNode
{
	public:
	
	class SinParams : public FunctionNodeParams
	{
		public :
		
		SinParams(Dirtable* dirtable)
		: FunctionNodeParams(dirtable)
		{}
		virtual void initializeParams()
		{
			// "type":"sinus", "dirX":8, "dirY":3, "freq":8.0, "ampl":255, "ratio":0.8, "perturbMapID":"noise1", "perturb":1
			addNumeric("dirX", 8.0);
			addNumeric("dirY", 3.0);
			addNumeric("freq", 8.0);
			addNumeric("ampl", 255.0);
			addNumeric("ratio", 0.8);
			addNumeric("perturb", 1.0);
		}
	};
	
	SinNode()
		: FunctionNode()
	{		
		std::ostringstream oss;
		oss << "Sin " << s_count++;
		
		addImage("perturbMap", -1);
		
		std::vector<std::string> iImg;
		std::vector<std::string> oImg;
		
		std::map<std::string, int>::iterator it;
		for(it=_nameImg.begin(); it!=_nameImg.end();it++)
		{
			std::string name = it->first;
			int id = it->second;
			
			if(id<0)
			{
				iImg.push_back(name);
			}
			else
			{
				oImg.push_back(name);
			}
		}
		
		buildView(oss.str(), iImg, oImg);
		_paramsView = new SinParams((Dirtable*)this);
		_paramsView->buildView();
		_paramsView->_gui->setVisible(false);
		// _guiNode->compose( _paramsView->_gui );
		
		setup();
	}
	
	virtual ~SinNode()
	{}
	
	virtual void setup()
	{
		dirX = 8.0;
		dirY = 3.0;
		freq = 8.0;
		ampl = 255.0;
		ratio = 0.8;
		perturbMap = IMP_NULL;
		perturbIntensity = 1.0;
	}
	
	virtual void apply()
	{
		if(isDirty())
		{
			dirX = getNumericParameter("dirX");
			dirY = getNumericParameter("dirY");
			freq = getNumericParameter("freq");
			ampl = getNumericParameter("ampl");
			ratio = getNumericParameter("ratio");
			perturbIntensity = getNumericParameter("perturb");
			perturbMap = getImage("perturbMap");
			drawDirectionnalSinus(*_image, dirX, dirY, freq, ampl, ratio, perturbMap, perturbIntensity);
			_dirty = false;
		}
	}
	
	virtual FunctionNode* clone()
	{
		return new SinNode();
	}
	
	float dirX;
	float dirY;
	float freq;
	float ampl;
	float ratio;
	ImageData* perturbMap;
	float perturbIntensity;
	
	static int s_count;
};

int SinNode::s_count = 0;

class NoiseFunctor : public FunctionNode
{
	public:
	
	class NoiseParams : public FunctionNodeParams
	{
		public :
		
		NoiseParams(Dirtable* dirtable)
		: FunctionNodeParams(dirtable)
		{}
		
		virtual void initializeParams()
		{
			// "type":"sinus", "dirX":8, "dirY":3, "freq":8.0, "ampl":255, "ratio":0.8, "perturbMapID":"noise1", "perturb":1
			std::vector<std::string> e;
			e.push_back("perlin");
			e.push_back("simplex");
			
			addEnum("method", e, "perlin");
			addNumeric("octave", 6.0);
			addNumeric("persistence", 0.7);
			addNumeric("freq", 4.0);
			addNumeric("ampl", 255.0);
			addNumeric("tiles", 1.0);
		}
	};
	
	NoiseFunctor()
		: FunctionNode()
	{
		std::ostringstream oss;
		oss << "Noise " << s_count++;
		
		
		std::vector<std::string> iImg;
		std::vector<std::string> oImg;
		
		std::map<std::string, int>::iterator it;
		for(it=_nameImg.begin(); it!=_nameImg.end();it++)
		{
			std::string name = it->first;
			int id = it->second;
			
			if(id<0)
			{
				iImg.push_back(name);
			}
			else
			{
				oImg.push_back(name);
			}
		}
		
		buildView(oss.str(), iImg, oImg);
		_paramsView = new NoiseParams((Dirtable*)this);
		_paramsView->buildView();
		_paramsView->_gui->setVisible(false);
		// _guiNode->compose( _paramsView->_gui );
		
		setup();
	}
	
	virtual ~NoiseFunctor()
	{}
	
	virtual void setup()
	{
		_method = "simplex";
		_tiles = 1.0;
		_freq = 4.0;
		_ampl = 255.0;
		_octaveCount = 6;
		_persistence = 0.7;
	}
	
	virtual void apply()
	{
		if(isDirty())
		{
			_method = getStringParameter("method");
			_tiles = getNumericParameter("tiles");
			_freq = getNumericParameter("freq");
			_ampl = getNumericParameter("ampl");
			_octaveCount = getNumericParameter("octave");
			_persistence = getNumericParameter("persistence");
			
			for(unsigned int i=0; i<_image->getWidth(); ++i)
			{
				for(unsigned int j=0; j<_image->getHeight(); ++j)
				{
					double nX = ((double)i)/_image->getWidth();
					double nY = ((double)j)/_image->getHeight();
					double v = 0.0;
					if(_method == "perlin")
						v = imp::perlinOctave(nX, nY, 0, _octaveCount, _persistence, _freq, 1.0/_tiles);
					else if(_method == "simplex")
						v = imp::simplexOctave(nX, nY, 0, _octaveCount, _persistence, _freq, 1.0/_tiles);
					
					imp::Uint8 comp = (v+1.0)/2.0 * _ampl;
					imp::Pixel px = {comp,comp,comp,255};
					_image->setPixel(i,j,px);
				}
			}
			_dirty = false;
		}
	}
	
	virtual FunctionNode* clone()
	{
		return new NoiseFunctor();
	}
	
	std::string _method;
	double _tiles;
	double _freq;
	double _ampl;
	double _octaveCount;
	double _persistence;
	
	static int s_count;
};

int NoiseFunctor::s_count = 0;

//--------------------------------------------------------------
EditorGraph::EditorGraph()
{
	
	_instance = this;
	_dict["sin"] = new SinNode();
	_dict["noise"] = new NoiseFunctor();
	_guiGraph = buildGui();
	
	
	_viewer = buildViewer(&_imageView);
	_selector = buildSelector();
	
	_guiGraph->addComponent(_viewer);
	_guiGraph->addComponent(_selector);
	_viewer->setVisible(false);
	_selector->setVisible(false);
}

//--------------------------------------------------------------
EditorGraph::~EditorGraph()
{}

//--------------------------------------------------------------
void EditorGraph::addFunctionNode(const std::string& funcName)
{
	FunctionNode* f = _dict[funcName];
	
	if(f)
	{
		f = f->clone();
		_nodes.push_back( f );
		_guiGraph->addComponent( f->_guiNode );
		_guiGraph->addComponent( f->_paramsView->_gui );
	}
}

//--------------------------------------------------------------
void EditorGraph::removeFunctionNode(FunctionNode* node)
{
	bool found = false;
	for(unsigned int i=0; i<_nodes.size(); ++i)
	{
		if(_nodes[i] == node)
		{
			delete _nodes[i];
			found = true;
		}
		
		if(found && i<_nodes.size()-1)
		{
			_nodes[i] = _nodes[i+1];
		}
	}
	if(found )
	{
		_nodes.resize(_nodes.size()-1);
	}
}

//--------------------------------------------------------------
void EditorGraph::updateAllNode()
{
	// updatedNode on this iteration
	std::vector<int> updatednodes;
	
	// iterate
	for(unsigned int i=0; i<_nodes.size(); ++i)
	{
		if(_nodes[i]->isDirty())
		{
			_nodes[i]->apply();
			_nodes[i]->_guiImage->setImage( _nodes[i]->_image );
			updatednodes.push_back(i);
		}
	}
	
	// make dirty depending nodes
	// looking for dependances (make them dirty too)
	
	// for all nodes
	for(unsigned int i=0; i<_nodes.size(); ++i)
	{
		bool dpDirty = false;
		// check dependances
		std::map<int, FunctionNode*>::iterator it;
		for(it=_nodes[i]->_dependances.begin(); it!=_nodes[i]->_dependances.end(); it++)
		{
			// for each updated node
			for(unsigned int u=0; u<updatednodes.size(); ++u)
			{
				if(it->second == _nodes[updatednodes[u]])
				{
					dpDirty = true;
					break;
				}
			}
			
			if(dpDirty)	break;
		}
		
		if(dpDirty)
		{
			_nodes[i]->makeDirty();
		}
	}
}

//--------------------------------------------------------------
bool EditorGraph::onLinkCreate(GuiEventSource* c1, int id1, GuiEventSource* c2, int id2)
{
	FunctionNode* node1 = IMP_NULL;
	FunctionNode* node2 = IMP_NULL;
	
	for(unsigned int i=0; i<_nodes.size(); ++i)
	{
		if(_nodes[i]->_guiNode == c1)
		{
			node1 = _nodes[i];
		}
		if(_nodes[i]->_guiNode == c2)
		{
			node2 = _nodes[i];
		}
	}
	
	if(node1 && node2)
	{
		if(id1 < 0 && id2 > 0)
		{
			node1->_imgIn[-id1] = node2->_image;
			node1->addDependance(-id1, node2);
			node1->makeDirty();
		}
		else if(id1 > 0 && id2 < 0)
		{
			node2->_imgIn[-id2] = node1->_image;
			node2->addDependance(-id2, node1);
			node2->makeDirty();
		}
	}
	
	return true;
}

//--------------------------------------------------------------
bool EditorGraph::onLinkDelete(GuiEventSource* c1, int id1)
{
	if(id1 < 0)
	{
		for(unsigned int i=0; i<_nodes.size(); ++i)
		{
			if(_nodes[i]->_guiNode == c1)
			{
				_nodes[i]->_imgIn[-id1] = IMP_NULL;
				_nodes[i]->removeDependance(-id1);
				_nodes[i]->makeDirty();
				break;
			}
		}
	}
	return true;
}

//--------------------------------------------------------------
EditorGraph* EditorGraph::_instance = IMP_NULL;

//--------------------------------------------------------------
void GuiOnEvent()
{
	imp::State* state = imp::State::getInstance();
	if(s_guiRoot != IMP_NULL)
	{
		s_guiRoot->event(state);
		
		if(state->m_pressedKeys[imp::Event::Space] == State::ActionState_Pressed)
		{
			EditorGraph::getInstance()->_selector->setVisible(true);
		}
	}
}

//--------------------------------------------------------------
void GuiUpdate()
{
	
}

//Texture* tex = IMP_NULL;

//--------------------------------------------------------------
void GuiRender(imp::RenderTarget& target, imp::GraphicRenderer& renderer, imp::RenderParameters& screenParameters)
{
//	GuiStateSingleton::getInstance()->defaultShader = &defaultShader;
	if(s_guiRoot == IMP_NULL)
	{
		GuiState::getInstance()->setResolution(800.0,600.0);
		s_guiRoot = EditorGraph::getInstance()->_guiGraph;
		GuiComponent::_guiComponentShader = new imp::GuiShader();
//		GuiComponent::_guiComponentShader->setFloatParameter("u_resolutionX", 800.0);
//		GuiComponent::_guiComponentShader->setFloatParameter("u_resolutionY", 600.0);
		GuiComponent::_guiComponentShader->setFloatParameter("u_outlineWidth", 1.0);
		GuiComponent::_guiComponentShader->setFloatParameter("u_cornerRadius", 5.0);
		GuiComponent::_guiComponentShader->setVector3Parameter("u_outlineColor", imp::Vector3(1.0,1.0,1.0));
	}
	
	renderer.setCamera(IMP_NULL);
	renderer.setRenderParameters(screenParameters);
	target.bind();
	GuiComponent::_guiComponentShader->enable();
	GuiComponent::_guiComponentShader->setMatrix4Parameter("u_projection", screenParameters.getProjectionMatrix());
	GuiComponent::_guiComponentShader->setMatrix4Parameter("u_view", imp::Matrix4::getIdentityMat());
	GuiComponent::_guiComponentShader->setMatrix4Parameter("u_model", imp::Matrix4::getIdentityMat());
	GuiComponent::_guiComponentShader->setFloatParameter("u_cornerRadius", 3.0);
	renderer.renderScene(-1);
	
	s_guiRoot->render(0);
	
	GuiLinkRenderer::instance()->updateBuffer();
	GuiComponent::_guiComponentShader->setVector3Parameter( "u_color", Vector3(1.0,1.0,1.0) );
	GuiComponent::_guiComponentShader->setFloatParameter( "u_type", 1.0 );
	GuiComponent::_guiComponentShader->setFloatParameter("u_cornerRadius", 0.0);
	GuiLinkRenderer::instance()->draw();
	GuiComponent::_guiComponentShader->disable();
}

//--------------------------------------------------------------
void onEvent(imp::EvnContextInterface& evnContext)
{
	imp::State* state = imp::State::getInstance();
	
	for(unsigned int i=0; i<Event::Mouse_ButtonCount ; ++i)
	{
		if(state->m_pressedMouseButtons[i] == State::ActionState_Pressed)
			state->m_pressedMouseButtons[i] = State::ActionState_True;
		else if(state->m_pressedMouseButtons[i] == State::ActionState_Released)
			state->m_pressedMouseButtons[i] = State::ActionState_False;
	}
	for(unsigned int i=0; i<Event::KeyCount ; ++i)
	{
		if(state->m_pressedKeys[i] == State::ActionState_Pressed)
			state->m_pressedKeys[i] = State::ActionState_True;
		else if(state->m_pressedKeys[i] == State::ActionState_Released)
			state->m_pressedKeys[i] = State::ActionState_False;
	}
	evnContext.getEvents(0);

	imp::Event event;
	while (evnContext.nextEvent(event))
		imp::State::getInstance()->onEvent(event);

	if(state->m_pressedKeys[imp::Event::Escape])
		exit(0);
}

//--------------------------------------------------------------
int main(int argc, char* argv[])
{
	imp::Timer fpsTimer;
	imp::State state;

	int winw = 800;
	int winh = 600;

	imp::EvnContextInterface* evnContext = new imp::SFMLContextInterface();
	evnContext->createWindow(winw,winh);
	evnContext->setCursorVisible(0, true);

	imp::GraphicRenderer renderer(0, NULL);
	renderer.setCenterCursor(false);

	/*if(argc<2)
		return 0;

	imp::Texture* texture = BmpLoader::loadFromFile(argv[1]);
	if(texture == IMP_NULL)
		return 0;*/
	
	time_t accessLast, modifLast, statusLast;
	imp::getTimeInfo(argv[1], accessLast, modifLast, statusLast);

	state.setWindowDim(winw, winh);

	 evnContext->resizeWindow(0, winw, winh);
	evnContext->setWindowTitle(0, argv[1]);

	// screen render parameters
	imp::RenderParameters screenParameters;
	screenParameters.setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
	screenParameters.setClearColor(imp::Vector3(0.f, 0.f, 1.f));

	// screen render target
	imp::RenderTarget screenTarget;
	screenTarget.createScreenTarget(0);

	imp::Matrix4 i4 = imp::Matrix4::getIdentityMat();

	imp::DefaultShader defaultShader;
	imp::ScreenVertex screen;
	
		// gui render parameters
    imp::RenderParameters guiRenderParameters;
    guiRenderParameters.setOrthographicProjection(0.f, winw, 0.f, winh, 0.f, 1.f);
	imp::RenderTarget guiTarget;
	guiTarget.createBufferTarget(winw, winh, 1, false);
	imp::Texture* guiTexture = guiTarget.getTexture(0);

	// int c = 0;
	
	while (evnContext->isOpen(0))
	{
		onEvent(*imp::EvnContextInterface::getInstance());
		GuiOnEvent();
		
		EditorGraph::getInstance()->updateAllNode();
		
		GuiRender(guiTarget, renderer, guiRenderParameters);

		renderer.setCamera(IMP_NULL);
		renderer.setRenderParameters(screenParameters);
		screenTarget.bind();
		defaultShader.enable();
		defaultShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
		defaultShader.setMatrix4Parameter("u_view", i4);
		defaultShader.setMatrix4Parameter("u_model", i4);
		defaultShader.setFloatParameter("u_type", 0.0);
		defaultShader.setVector3Parameter("u_color", imp::Vector3(1.0, 1.0, 1.0));
		defaultShader.setTextureParameter("u_colorTexture", guiTexture, 0);
		renderer.renderScene(-1);
		screen.render(0);
		defaultShader.disable();
		screenTarget.unbind();

		evnContext->display(0);
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}

//--------------------------------------------------------------
FunctionNode::FunctionNode()
{
	_image = new ImageData();
	_image->create(500.0, 500.0, imp::PixelFormat_RGB8);
}

//--------------------------------------------------------------
FunctionNode::~FunctionNode()
{
	delete _guiNode;
	delete _image;
}

//--------------------------------------------------------------
double FunctionNode::getNumericParameter(const std::string& key)
{
	return _paramsView->_numericMap[key];
}

//--------------------------------------------------------------
std::string FunctionNode::getStringParameter(const std::string& key)
{
	return _paramsView->_stringMap[key];
}

//--------------------------------------------------------------
void FunctionNode::addDependance(int id, FunctionNode* node)
{
	std::map<int,FunctionNode*>::iterator it;
	for(it = _dependances.begin(); it != _dependances.end(); it++)
	{
		if(it->second == node)
			break;
	}
	/*for(unsigned int i=0; i<_dependances.size(); ++i)
	{
		if(node == _dependances[i])
			break;
	}
	*/
	node->removeDependance(this);
	_dependances[id] = node;
}

//--------------------------------------------------------------
void FunctionNode::removeDependance(FunctionNode* node)
{
	bool found = false;
	int id = 0;
	std::map<int,FunctionNode*>::iterator it;
	for(it=_dependances.begin(); it!=_dependances.end(); it++)
	{
		if(node == it->second)
		{
			id = it->first;
			found = true;
			break;
		}
	}
	
	if(found)
	{
		_dependances[id] = IMP_NULL;
	}
}

//--------------------------------------------------------------
void FunctionNode::removeDependance(int id)
{
	_dependances[id] = IMP_NULL;
}

//--------------------------------------------------------------
void FunctionNode::buildView(const std::string& title, const std::vector<std::string>& iImg, const std::vector<std::string>& oImg)
{
	_guiNode = buildBox(title, &_guiImage, &_paramsButton, iImg, oImg);
	makeDirty();
	
	class ViewerAction : public GuiEventHandler
	{
		public:

		ViewerAction(ImageData* img)
			: _img(img)
		{
		}
		//--------------------------------------------------------------
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over)
			{
				EditorGraph::getInstance()->_imageView->setImage(_img);
				EditorGraph::getInstance()->_viewer->setVisible(true);
			}
			return false;
		}
		
		protected:
		
		ImageData* _img;
	};
	
	class ParameterAction : public GuiEventHandler
	{
		public:

		ParameterAction(FunctionNode* img)
			: _img(img)
		{
		}
		//--------------------------------------------------------------
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over)
			{
				_img->_paramsView->_gui->setPosition(100.0, 100.0);
				_img->_paramsView->_gui->setVisible(true);
			}
			return false;
		}
		
		protected:
		
		FunctionNode* _img;
	};
	
	_guiImage->addEventHandler(new ViewerAction(_image) );
	_paramsButton->addEventHandler(new ParameterAction(this));
}

//--------------------------------------------------------------
GuiLinkedBox* buildBox(const std::string& name, GuiImage** i, GuiButton** paramsButton, const std::vector<std::string>& iImg, const std::vector<std::string>& oImg)
{
	Layout::Parameters param;
	param.setAlignementX(Layout::Alignement_Center);
	param.setPositionningY(Layout::Positionning_Auto);
	
	GuiLinkedBox* root = new GuiLinkedBox();
	 root->enableScrollbar(false);
	 
	 int inputCount = iImg.size();
	 int outputCount = 1;
	 
	 if(inputCount > 0)
	 {
		 root->setupAnchorCount(GuiLinkAnchor::Side_Left, inputCount);
		 for(int i=0; i<inputCount; ++i)
		 {
			 root->setAnchorText(GuiLinkAnchor::Side_Left, i+1, iImg[i]);
		 }
	 }
	 
	if(outputCount > 0)
	{
		root->setupAnchorCount(GuiLinkAnchor::Side_Right, outputCount);
		 /*for(int i=0; i<outputCount; ++i)
		 {
			 root->setAnchorText(GuiLinkAnchor::Side_Right, i, oImg[i]);
		 }*/
		 root->setAnchorText(GuiLinkAnchor::Side_Right, 1, "output");
	}
	
	GuiContainer* content = new GuiContainer();
	content->setPosition(0.0,10.0);
	content->setSize(100.0,140.0);
	content->setLayoutParameters(param);
	root->addComponent(content);

	(*i) = new GuiImage();
	(*i)->setSize(100.0,100.0);
	(*i)->setLayoutParameters(param);
	content->addComponent((*i));
	
	root->setPosition(100.0, 50.0);
	root->setSize(110.0, 150.0);
	 root->setTitle(name);
	 root->setLinkHandler( EditorGraph::getInstance() );

	Layout::Parameters listparam;
	listparam.setAlignementX(Layout::Alignement_Begin);
	listparam.setPositionningY(Layout::Positionning_Auto);
	(*paramsButton) = new GuiButton();
	(*paramsButton)->setText("parameters");
	(*paramsButton)->setPosition(10.0,120.0);
	(*paramsButton)->setSize(90.0,20.0);
	(*paramsButton)->setLayoutParameters(param);
	content->addComponent(*paramsButton);
	
	return root;
}