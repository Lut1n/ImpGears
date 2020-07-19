

//--------------------------------------------------------------
// Data structure
template<class Ty>
class TreeBase
{
public:
  TreeBase* _parent;
  Ty _data;
  std::vector<TreeBase*> _children;

public:
  TreeBase()
    : _parent(nullptr)
  {
  }

  virtual ~TreeBase()
  {
    for(auto it : _children) delete it;
    _children.clear();
  }

  void add(TreeBase* c)
  {
    if(c==nullptr) return;
    c->_parent=this;
    _children.push_back(c);
  }

  void rem(TreeBase* c)
  {
    if(c==nullptr) return;
    c->_parent = nullptr;
    auto it = std::find(_children.begin(),_children.end(),c);
    _children.erase(it);
  }

  void rem(int index)
  {
    TreeBase* c = at(index);
    if(c==nullptr) return;
    rem(c);
  }

  TreeBase* at(int index)
  {
    if(index<0 || index>=(int)_children.size()) return nullptr;
    return _children[index];
  }

  int childrenCount() const
  {
    return _children.size();
  }

  TreeBase* parent()
  {
    return _parent;
  }

  Ty& data()
  {
      return _data;
  }

  void setData(const Ty& data)
  {
      _data = data;
  }

  bool visible()
  {
      return _data.visible();
  }

  void setVisible(bool v)
  {
      _data.setVisible(v);
/*
      if(v && _node==nullptr)
      {

      }
      else if(!v && _node!=nullptr)
      {

      }*/
  }

};


//--------------------------------------------------------------
/*
enum DataType
{
    Undefined,
    GeoPrimitiveSet,
    ConstructiveTree
};
*/

//--------------------------------------------------------------
enum ConstructiveOp
{
    NONE,
    MERGE,
    INTER,
    DIFF
};

//--------------------------------------------------------------
class ConstructiveData
{
public:
    ConstructiveOp _operation;
    bool _dirty;
    bool _visible;
    Node::Ptr _node;
    int _gui_primIndex;

    std::vector<GeoNode::Ptr> _geoset;
    std::vector<JsonSceneNode::GeometricPrimitive> _prims;
    std::vector<bool> _visibilities;

    GeoNode::Ptr _localGeode;
    Geometry::Ptr _localResult;



    ConstructiveData()
      : _operation(NONE)
      , _dirty(false)
      , _visible(false)
      , _gui_primIndex(0)
    {}

    void setOp(ConstructiveOp op)
    {
        if(op==_operation) return;

        _operation=op;
        _dirty=true;
    }

    void addPrim(GeoNode::Ptr geo, JsonSceneNode::GeometricPrimitive prim)
    {
        _geoset.push_back(geo);
        _prims.push_back(prim);
        _visibilities.push_back(true);
    }

    void remPrim(int index)
    {
      auto it_geo = _geoset.begin() + index;
      auto it_prim = _prims.begin() + index;
      auto it_visible = _visibilities.begin() + index;

      _geoset.erase(it_geo);
      _prims.erase(it_prim);
      _visibilities.erase(it_visible);
    }

    void remPrim(GeoNode::Ptr geo)
    {
      auto it = std::find(_geoset.begin(),_geoset.end(),geo);
      if(it==_geoset.end()) return;

      int index = std::distance(_geoset.begin(),it);
      remPrim(index);
    }

    int primCount()
    {
      return _geoset.size();
    }

    GeoNode::Ptr geoAt(int index)
    {
      return _geoset[index];
    }

    JsonSceneNode::GeometricPrimitive primAt(int index)
    {
      return _prims[index];
    }

    bool isPrimVisible(int index)
    {
        return _visibilities[index];
    }

    void setPrimVisible(int index, bool v)
    {
        _visibilities[index] = v;
    }

    bool isVisible()
    {
        return _visible;
    }

    void setVisible(bool v)
    {
        _visible = v;
    }
};

using Tree = TreeBase<ConstructiveData>;
