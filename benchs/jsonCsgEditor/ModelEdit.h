

void recChild(std::vector< std::pair<Node::Ptr,Node::Ptr> >& arr, const Node::Ptr& c, const Node::Ptr& p)
{
  arr.push_back( std::make_pair(c, p) );
}

struct GuiControler
{
      bool requestSave = false;
      bool requestLoad = false;
      bool requestReset = false;
      bool requestDuplicate = false;
      bool requestAdd = false;
      bool requestRem = false;
      int selected = 0;

      Node::Ptr inEdition;

      Tree* selectedTree = nullptr;
      std::vector< std::pair<Node::Ptr,Node::Ptr> > toRem;
      std::vector< std::pair<Node::Ptr,Node::Ptr> > toAdd;

      std::string filename;
      JsonSceneNode::GeometricPrimitive selectedPrimitive;

      GuiControler()
      {
        filename.reserve(1024);
        selectedPrimitive = JsonSceneNode::Cube;
      }

      void endFrame()
      {
          requestReset = false;
          requestSave = false;
          requestLoad = false;
          requestDuplicate = false;
          requestAdd = false;
          requestRem = false;
          toRem.clear();
          toAdd.clear();
      }

      void showNodeEdition()
      {
          Node::Ptr& node = inEdition;
          if(!node) return;

          ImGui::Begin("NodeEdit");

          ImGui::SliderFloat3("position", node->getPosition().data(),-10.0,10.0);
          ImGui::SliderFloat3("rotation", node->getRotation().data(),-3.14,3.14);
          ImGui::SliderFloat3("scale", node->getScale().data(),0.01,30.0);

          GeoNode::Ptr geode = GeoNode::dMorph(node);
          if(geode)
          {
            ImGui::ColorEdit3("color", geode->_material->_color.data());
          }

          // force update transform
          node->setPosition(node->getPosition());
          node->setRotation(node->getRotation());
          node->setScale(node->getScale());

          ImGui::End();
      }

      void remFromScene(Node::Ptr sceneRoot)
      {
        for(auto node : toRem)
        {
          if(node.second == nullptr) sceneRoot->remNode(node.first);
          else node.second->remNode(node.first);
        }
      }

      void addToScene(Node::Ptr sceneRoot)
      {
        for(auto node : toAdd)
        {
          if(node.second == nullptr) sceneRoot->addNode(node.first);
          else node.second->addNode(node.first);
        }
      }

      void recRem(const Node::Ptr& child, const Node::Ptr& parent)
      {
        recChild(toRem, child, parent);
      }

      void recAdd(const Node::Ptr& child, const Node::Ptr& parent)
      {
        recChild(toAdd, child, parent);
      }
};

void findAllGeoNode(Tree* tree, std::vector<std::pair<Node::Ptr,Node::Ptr> >& found)
{
  for(auto it : tree->data()._geoset) found.push_back( std::make_pair(it, tree->data()._node) );
  for(auto it : tree->_children) findAllGeoNode(it, found);
}

void findAllNode(Tree* tree, std::vector<std::pair<Node::Ptr,Node::Ptr> >& found)
{
  Node::Ptr pnode;
  if(tree->parent()) pnode = tree->parent()->data()._node;
  if(tree->data()._node) found.push_back( std::make_pair(tree->data()._node, pnode) );
  for(auto it : tree->_children) findAllNode(it, found);
}

void addDirectChildren(Tree& tree, std::vector< std::pair<Node::Ptr,Node::Ptr> >& arr)
{
  for(auto g : tree.data()._geoset) recChild(arr,g,tree.data()._node);
  for(auto n : tree._children) recChild(arr,n->data()._node,tree.data()._node);
}

void showGraph(Tree& tree, GuiControler& ctrl, const std::string& text = "")
{
      ConstructiveData& geoset = tree._data;

      const char* items[] = { "SET", "MERGE", "INTER", "DIFF"};

      // const char* prim_items[] = { "CUBE", "SPHERE", "TORUS", "CYLINDER", "CAPSULE", "CRYSTAL"};

      int item_op = geoset._operation;
      ImGui::Combo("Operation", &item_op, items, IM_ARRAYSIZE(items));

      if(item_op == 0) geoset.setOp(NONE);
      else if(item_op == 1) geoset.setOp(MERGE);
      else if(item_op == 2) geoset.setOp(INTER);
      else if(item_op == 3) geoset.setOp(DIFF);

      if(ImGui::Button("Apply"))
      {
        // prepare
        std::vector<GeoNode::Ptr> geodes;
        if(geoset.primCount() >= 1) geodes.push_back(geoset.geoAt(0));
        if(geoset.primCount() >= 2) geodes.push_back(geoset.geoAt(1));
        if(tree.childrenCount() >= 1) geodes.push_back(tree.at(0)->data()._localGeode);
        if(tree.childrenCount() >= 2) geodes.push_back(tree.at(1)->data()._localGeode);

        // reset visible
        addDirectChildren(tree, ctrl.toRem);
        if(tree.data()._localGeode != nullptr)
          ctrl.recRem(tree.data()._localGeode, tree.data()._node);

        // compute new
        if(geodes.size() > 1)
        {
          if(geoset._operation == MERGE)
            tree.data()._localGeode = mergeGeodes(geodes[0], geodes[1]);
          else if(geoset._operation == DIFF)
            tree.data()._localGeode = diffGeodes(geodes[0], geodes[1]);
          else if(geoset._operation == INTER)
            tree.data()._localGeode = interGeodes(geodes[0], geodes[1]);
          else if(geoset._operation == NONE)
            tree.data()._localGeode = nullptr;
        }
        else
        {
          tree.data()._localGeode = nullptr;
        }

        if(tree.data()._localGeode != nullptr)
        {
          ctrl.recAdd(tree.data()._localGeode, tree.data()._node);
          // recChild(toRem, geodes[0], tree.data()._node);
          // recChild(toRem, geodes[1], tree.data()._node);
        }
        else
        {
          // add all sub node and geode
          addDirectChildren(tree, ctrl.toAdd);
        }
      }


      const char* primitives[] = { "CUBE", "SPHERE", "TORUS", "CYLINDER", "CAPSULE", "CRYSTAL", "SUB_OP"};
      const JsonSceneNode::GeometricPrimitive ordered_enum[] =
      { JsonSceneNode::Cube, JsonSceneNode::Sphere, JsonSceneNode::Torus, JsonSceneNode::Cylinder, JsonSceneNode::Capsule,
        JsonSceneNode::Crystal, JsonSceneNode::Undefined};
      int prim_index = geoset._gui_primIndex;
      ImGui::Combo("Primitive", &prim_index, primitives, IM_ARRAYSIZE(primitives));
      geoset._gui_primIndex = prim_index;
      if(ImGui::Button("Add"))
      {
          if(prim_index < 6)
          {
            ctrl.selectedPrimitive = ordered_enum[prim_index];
            ctrl.selectedTree = &tree;
            ctrl.requestAdd = true;
          }
          else
          {
            Tree* sub = new Tree();
            sub->data()._node = Node::create();
            tree.add(sub);
            ctrl.recAdd(sub->data()._node, geoset._node);
          }
      }

      if(ImGui::Button("Edit"))
      {
        ctrl.inEdition = tree.data()._node;
      }

      // nodeEdit(tree.data()._node);

      // int n=0;
      for(int i=0;i<geoset.primCount();++i)
      {
          std::stringstream ss;
          ss << "primitive #" << i;
          if(geoset.primAt(i) == JsonSceneNode::Cube) ss << " (cube)";
          if(geoset.primAt(i) == JsonSceneNode::Sphere) ss << " (Sphere)";
          if(geoset.primAt(i) == JsonSceneNode::Torus) ss << " (torus)";
          if(geoset.primAt(i) == JsonSceneNode::Cylinder) ss << " (cylinder)";
          if(geoset.primAt(i) == JsonSceneNode::Capsule) ss << " (capsule)";
          if(geoset.primAt(i) == JsonSceneNode::Crystal) ss << " (crytal)";

          std::string str = ss.str();
          if (ImGui::TreeNode(str.c_str()))
          {
              // nodeEdit(geoset.geoAt(i));

              if(ImGui::Button("Edit"))
              {
                ctrl.inEdition = geoset.geoAt(i);
              }

              if(ImGui::Button("Erase"))
              {
                ctrl.recRem(geoset.geoAt(i),tree.data()._node);
                ctrl.requestRem = true;
                geoset._geoset[i] = nullptr;
              }

              ImGui::TreePop();
          }
      }

      for(int i=geoset.primCount()-1;i>=0;--i)
      {
          if(geoset.geoAt(i) == nullptr)
          {
              // model->remItem(i);
              geoset.remPrim(i);
          }
      }

      std::vector<int> toRem;

      for(int i=0;i<tree.childrenCount();++i)
      {
          std::stringstream ss; ss << items[tree.at(i)->data()._operation] << " " << std::hex << tree.at(i);
          std::string str = ss.str();

          if (ImGui::TreeNode(str.c_str()))
          {

            showGraph(*tree.at(i),ctrl, str);

            if(ImGui::Button("Erase")) toRem.push_back(i);

            ImGui::TreePop();
          }
      }

      for(int i=(int)toRem.size()-1;i>=0;--i)
      {
        findAllGeoNode(tree.at(i), ctrl.toRem);
        findAllNode(tree.at(i), ctrl.toRem);

        delete tree.at(i);
        tree.rem(i);
      }
}
