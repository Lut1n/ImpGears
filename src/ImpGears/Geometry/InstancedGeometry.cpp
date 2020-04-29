#include <ImpGears/Geometry/InstancedGeometry.h>

IMPGEARS_BEGIN

InstancedGeometry::InstancedGeometry()
{
    // _transforms;
    _instanceCount = 0;
    _transformsChanged = false;
}

void InstancedGeometry::clone(Geometry::Ptr geometry)
{
    _prim = geometry->_prim;
    _vertices = geometry->_vertices;

    _hasIndices = geometry->_hasIndices;
    _hasTexCoords = geometry->_hasTexCoords;
    _hasColors = geometry->_hasColors;
    _hasNormals = geometry->_hasNormals;

    _indices = geometry->_indices;
    _normals = geometry->_normals;
    _colors = geometry->_colors;
    _texCoords = geometry->_texCoords;
}

void InstancedGeometry::setTransforms(const std::vector<Matrix4>& buf)
{
    _transforms = buf;
    _transformsChanged = true;
}

void InstancedGeometry::setTransformAt(int index, const Matrix4& mat)
{
    if(index <0) return;
    if(index >= (int)_transforms.size()) _transforms.resize(index+1);
    
    _transforms[index] = mat;
    _transformsChanged = true;
}

const std::vector<Matrix4>& InstancedGeometry::getTransforms() const
{
    return _transforms;
}

bool InstancedGeometry::hasChanged() const
{
    return _transformsChanged;
}

void InstancedGeometry::changesDone()
{
    _transformsChanged = false;
}


IMPGEARS_END
