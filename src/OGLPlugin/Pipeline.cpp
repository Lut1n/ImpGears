#include <OGLPlugin/Pipeline.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_copy = GLSL_CODE(

uniform sampler2D u_input_sampler;
varying vec2 v_texCoord;

vec4 i_col(vec2 uv){return texture2D(u_input_sampler, uv).rgbw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = i_col(v_texCoord);
}

);


//--------------------------------------------------------------
static std::string glsl_fill = GLSL_CODE(

uniform vec4 u_fill_color;

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = u_fill_color;
}

);

IMPGEARS_BEGIN

std::string FrameOperation::s_glsl_copy;
std::string FrameOperation::s_glsl_fill;

//--------------------------------------------------------------
//--------------------------------------------------------------
FrameOperation::FrameOperation()
{
    if(s_glsl_copy.empty()) s_glsl_copy = glsl_copy;
    if(s_glsl_fill.empty()) s_glsl_fill = glsl_fill;
}

//--------------------------------------------------------------
FrameOperation::~FrameOperation()
{

}

//--------------------------------------------------------------
void FrameOperation::setInput(ImageSampler::Ptr sampler, int id)
{
    if(id >= (int)_input.size()) _input.resize(id+1);
    _input[id] = sampler;
}

//--------------------------------------------------------------
void FrameOperation::setOutput(ImageSampler::Ptr sampler, int id)
{
    if(id >= (int)_output.size()) _output.resize(id+1);
    _output[id] = sampler;
}

//--------------------------------------------------------------
ImageSampler::Ptr FrameOperation::getInput(int id)
{
    if( id<0 || id>=(int)_input.size() ) return nullptr;
    return _input[id];
}

//--------------------------------------------------------------
ImageSampler::Ptr FrameOperation::getOutput(int id)
{
    if( id<0 || id>=(int)_output.size() ) return nullptr;
    return _output[id];
}

//--------------------------------------------------------------
Graph::Ptr FrameOperation::buildQuadGraph(const std::string& debug_name,
                                          const std::string& glsl_code,
                                          Vec4 viewport) const
{
    ReflexionModel::Ptr shader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi,
                debug_name);
    shader->_fragCode_lighting = glsl_code;

    QuadNode::Ptr quad = QuadNode::create();
    quad->setReflexion(shader);
    Node::Ptr root = quad;

    Graph::Ptr graph = Graph::create();
    graph->setRoot(root);
    graph->getInitState()->setViewport( viewport );
    graph->getInitState()->setOrthographicProjection(-1.0,1.0,-1.0,1.0,0.0,1.0);

    return graph;
}

//--------------------------------------------------------------
void FrameOperation::setEnvCubeMap(CubeMapSampler::Ptr& cubemap)
{
    _environment = cubemap;
}

//--------------------------------------------------------------
void FrameOperation::setShaCubeMap(CubeMapSampler::Ptr& cubemap)
{
    _shadows = cubemap;
}

//--------------------------------------------------------------
void FrameOperation::setCamera(const Camera* camera)
{
    _camera = camera;
}

//--------------------------------------------------------------
void FrameOperation::setLight(const LightNode* light)
{
    _light = light;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
Pipeline::Binding::Binding()
{
    _opA_output = 0;
    _opB_input = 0;
}

//--------------------------------------------------------------
Pipeline::Binding::Binding(int opA, int opB, int opA_output, int opB_input)
{
    _opA = opA;
    _opB = opB;
    _opA_output = opA_output;
    _opB_input = opB_input;
}

//--------------------------------------------------------------
bool Pipeline::Binding::operator==(const Binding& that) const
{
    return _opB==that._opB && _opB_input==that._opB_input;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
Pipeline::Pipeline(GlRenderer* renderer)
    : _renderer(renderer)
    , _dirty(true)
{

}

//--------------------------------------------------------------
Pipeline::~Pipeline()
{

}

//--------------------------------------------------------------
void Pipeline::setOperation(const FrameOperation::Ptr& op, int index)
{
    if(index < 0) return;
    if(index >= (int)_operations.size()) _operations.resize(index+1);

    auto found = std::find(_operations.begin(),_operations.end(),op);
    if(found == _operations.end())
    {
        _operations[index] = op;
    }
    _dirty = true;
}

//--------------------------------------------------------------
void Pipeline::setActive(int opIndex, bool activate)
{
    if(opIndex < 0) return;
    if(opIndex >= (int)_activated.size()) _activated.resize(opIndex+1, true);
    _activated[opIndex] = activate;
}

//--------------------------------------------------------------
void Pipeline::bind(int dstOpId, int srcOpId, int dstInputId, int srcOutputId)
{
    Binding newBinding(srcOpId, dstOpId, srcOutputId, dstInputId);

    auto found = std::find(_bindings.begin(),_bindings.end(),newBinding);
    if(found == _bindings.end())
        _bindings.push_back(newBinding);
    else
        *found = newBinding;

    _dirty = true;
}

//--------------------------------------------------------------
void Pipeline::unbind(int dstOpId, int dstInputId)
{
    Binding targetBinding(0, dstOpId, 0, dstInputId);

    auto found = std::find(_bindings.begin(),_bindings.end(),targetBinding);
    if(found != _bindings.end()) _bindings.erase(found);

    _dirty = true;
}

//--------------------------------------------------------------
ImageSampler::Ptr Pipeline::getOutputFrame( int opIndex, int outputIndex )
{
    if(opIndex < 0 || opIndex >= (int)_operations.size()) return nullptr;

    FrameOperation::Ptr op = _operations[opIndex];
    if(op) return op->getOutput( outputIndex );
    return nullptr;
}

//--------------------------------------------------------------
void Pipeline::setupOperations()
{
    int opSize = (int)_operations.size();
    int bdSize = (int)_bindings.size();

    for(int k=0;k<opSize;++k)
    {
        std::map< int, std::vector<int> > output_bindings;

        // map all output bindings for the current operation
        for(int i=0; i<bdSize;++i)
        {
            int src_op = _bindings[i]._opA;
            int output_index = _bindings[i]._opA_output;
            if(src_op == k)
                output_bindings[output_index].push_back( i );
        }

        if( output_bindings.size() == 0 ) continue;

        // instantiate output frames and bind them
        for(auto key_bind : output_bindings)
        {
            int output_index = key_bind.first;
            const std::vector<int>& binds = key_bind.second;

            ImageSampler::Ptr frame = _operations[k]->getOutput(output_index);
            if(frame == nullptr)
            {
                frame = ImageSampler::create(1024,1024,4,Vec4(0.0));
                _operations[k]->setOutput(frame, output_index);
            }

            for(auto next : binds)
            {
                int target_op = _bindings[next]._opB;
                int input_index = _bindings[next]._opB_input;
                _operations[target_op]->setInput(frame, input_index);
            }

        }
    }

}

//--------------------------------------------------------------
void Pipeline::buildDependencies()
{
    int opSize = (int)_operations.size();
    int bdSize = (int)_bindings.size();
    int depsSize = (int)_dependencies.size();

    // resize dependencies matrix if needed
    if(depsSize != opSize)
    {
        _dependencies.resize(opSize);
        for(auto& d : _dependencies) d.resize(opSize);
    }

    // update
    for(int i=0;i<opSize;++i) for(int j=0; j<opSize;++j)
    {
        bool isDep = false;
        for(int k=0; k<bdSize;++k)
            isDep |= (_bindings[k]._opA == j) && (_bindings[k]._opB == i);
        _dependencies[i][j] = isDep;
    }
}

//--------------------------------------------------------------
void Pipeline::deduceOrder()
{
    int opSize = (int)_operations.size();
    std::vector<bool> remainings(opSize, true);
    _orderedOp.resize(opSize);

    for(int k=0; k<opSize;++k)
    {
        int taken_id = -1;

        // take a remaining
        for(int i=0; i<opSize;++i)
        {
            if(remainings[i])
            {
                bool all_deps_takens = true;

                // check dependencies
                for(int j=0; j<opSize;++j)
                    all_deps_takens &= !_dependencies[i][j] || !remainings[j];

                if(all_deps_takens) { taken_id=i; break;}
            }
        }

        if(taken_id >= 0)
        {
            remainings[taken_id] = false;
            _orderedOp[k] = taken_id;
        }
        else
        {
            std::cout << "impError : Pipeline is not consistent" << std::endl;
            return;
        }
    }
}

//--------------------------------------------------------------
void Pipeline::prepare(const Camera* camera,
                       const LightNode* light,
                       CubeMapSampler::Ptr& shadowCubemap,
                       CubeMapSampler::Ptr& envCubemap)
{
    if(_dirty)
    {
        buildDependencies();
        deduceOrder();

        setupOperations();
        for(auto& op : _operations) op->setup();

        _dirty = false;
    }

    for(auto& op : _operations)
    {
        op->setCamera(camera);
        op->setLight(light);
        op->setEnvCubeMap(envCubemap);
        op->setShaCubeMap(shadowCubemap);
    }
}

//--------------------------------------------------------------
void Pipeline::run(int targetOpIndex)
{
    if(targetOpIndex < 0 || targetOpIndex>=(int)_operations.size()) return;

    for(int i=0;i<(int)_operations.size();++i)
    {
        int op_index = _orderedOp[i];
        if(op_index == targetOpIndex) continue;
        bool skipOp = op_index<(int)_activated.size() && !_activated[op_index];
        _operations[op_index]->apply( _renderer, skipOp );
    }

    // be sure that the target operation is the last one
    _operations[targetOpIndex]->apply( _renderer, false );
}


IMPGEARS_END
