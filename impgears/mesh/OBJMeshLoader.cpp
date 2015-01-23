#include "OBJMeshLoader.h"
#include <vector>
#include <fstream>
#include <string>

int occurrenceOf(const std::string& buff, char c)
{
    int result = 0;

    for(unsigned int n=0; n<buff.size(); ++n)
    {
        if(buff[n] == c)
            ++result;
    }

    return result;
}

void normalizeIndicesString(std::string& buff)
{
    int occur = occurrenceOf(buff, '/');
    switch(occur)
    {
        case 0:
            buff.append("/1/1");
        break;
        case 1:
            buff.append("/1");
        break;
        case 2:
        {
            size_t pos = buff.find("//");
            if(pos != std::string::npos)
            {
                buff.replace(pos, 2, "/1/");
            }
        }
        break;
        default:
            fprintf(stderr, "[impError] bad indices format.\n");
        break;
    }
}

void getTexCoord(std::string& buff, std::vector<float>& texCoords)
{
    float u,v;
    std::sscanf(buff.c_str(), "vt %f %f", &u, &v);
    texCoords.push_back(u);
    texCoords.push_back(v);
}

void getNormal(std::string& buff, std::vector<float>& normals)
{
    float x,y,z;
    std::sscanf(buff.c_str(), "vn %f %f %f", &x, &y, &z);
    normals.push_back(x);
    normals.push_back(y);
    normals.push_back(z);
}

void getVertex(std::string& buff, std::vector<float>& vertices)
{
    float x,y,z;
    std::sscanf(buff.c_str(), "v %f %f %f", &x, &y, &z);
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void getIndices(std::string& buff, std::vector<float>& vertices, std::vector<float>& texCoords, std::vector<float>& normals,
                std::vector<float>& vertexBuffer, std::vector<float>& texCoordBuffer, std::vector<float>& normalBuffer, bool& isQuad)
{
    isQuad = false;

    char p1[64], p2[64], p3[64], p4[64];

    int nbInd = occurrenceOf(buff.c_str(), ' ');

    if(nbInd == 3)
    {
        std::sscanf(buff.c_str(), "f %s %s %s", p1, p2, p3);
        p4[0] = '\0';
    }
    else if(nbInd == 4)
    {
        std::sscanf(buff.c_str(), "f %s %s %s %s", p1, p2, p3, p4);
        isQuad = true;
    }

    std::string p1str(p1), p2str(p2), p3str(p3), p4str(p4);

    normalizeIndicesString(p1str);
    normalizeIndicesString(p2str);
    normalizeIndicesString(p3str);

    float v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3, v4, vt4, vn4;
    std::sscanf(p1str.c_str(), "%f/%f/%f", &v1, &vt1, &vn1);
    std::sscanf(p2str.c_str(), "%f/%f/%f", &v2, &vt2, &vn2);
    std::sscanf(p3str.c_str(), "%f/%f/%f", &v3, &vt3, &vn3);

    --v1;
    --vt1;
    --vn1;
    --v2;
    --vt2;
    --vn2;
    --v3;
    --vt3;
    --vn3;

    vertexBuffer.push_back(vertices[v1*3]);
    vertexBuffer.push_back(vertices[v1*3+1]);
    vertexBuffer.push_back(vertices[v1*3+2]);
    vertexBuffer.push_back(vertices[v2*3]);
    vertexBuffer.push_back(vertices[v2*3+1]);
    vertexBuffer.push_back(vertices[v2*3+2]);
    vertexBuffer.push_back(vertices[v3*3]);
    vertexBuffer.push_back(vertices[v3*3+1]);
    vertexBuffer.push_back(vertices[v3*3+2]);

    if(texCoords.size() > 0)
    {
        texCoordBuffer.push_back(texCoords[vt1*2]);
        texCoordBuffer.push_back(texCoords[vt1*2+1]);
        texCoordBuffer.push_back(texCoords[vt2*2]);
        texCoordBuffer.push_back(texCoords[vt2*2+1]);
        texCoordBuffer.push_back(texCoords[vt3*2]);
        texCoordBuffer.push_back(texCoords[vt3*2+1]);
    }

    if(normals.size() > 0)
    {
        normalBuffer.push_back(normals[vn1*3]);
        normalBuffer.push_back(normals[vn1*3+1]);
        normalBuffer.push_back(normals[vn1*3+2]);
        normalBuffer.push_back(normals[vn2*3]);
        normalBuffer.push_back(normals[vn2*3+1]);
        normalBuffer.push_back(normals[vn2*3+2]);
        normalBuffer.push_back(normals[vn3*3]);
        normalBuffer.push_back(normals[vn3*3+1]);
        normalBuffer.push_back(normals[vn3*3+2]);
    }

    if(isQuad)
    {
        normalizeIndicesString(p4str);
        std::sscanf(p4str.c_str(), "%f/%f/%f", &v4, &vt4, &vn4);

        --v4;
        --vt4;
        --vn4;

        vertexBuffer.push_back(vertices[v4*3]);
        vertexBuffer.push_back(vertices[v4*3+1]);
        vertexBuffer.push_back(vertices[v4*3+2]);

        if(texCoords.size() > 0)
        {
            texCoordBuffer.push_back(texCoords[vt4*2]);
            texCoordBuffer.push_back(texCoords[vt4*2+1]);
        }

        if(normals.size() > 0)
        {
            normalBuffer.push_back(normals[vn4*3]);
            normalBuffer.push_back(normals[vn4*3+1]);
            normalBuffer.push_back(normals[vn4*3+2]);
        }
    }
}


IMPGEARS_BEGIN

OBJMeshLoader::OBJMeshLoader()
{
}

OBJMeshLoader::~OBJMeshLoader()
{
}

MeshModel* OBJMeshLoader::loadFromFile(const char* filename)
{
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<float> normals;

    std::vector<float> vertexBuffer, texCoordBuffer, normalBuffer;

    std::ifstream f(filename, std::ifstream::in);
    std::string buff;

    bool isQuad = false;
    MeshModel::VertexMode vertexMode = MeshModel::VertexMode_Triangles;

    while(f.good())
    {
        getline(f, buff);

        if(buff[0] == 'v')
        {
            if(buff[1] == 't')
            {
                // texture coord
                getTexCoord(buff, texCoords);
            }
            else if(buff[1] == 'n')
            {
                // normal
                getNormal(buff, normals);
            }
            else if(buff[1] == 'p')
            {
                // param space vertex
                fprintf(stderr, "[impError] OBJ loading : Param space vertex not supported.\n");
            }
            else
            {
                // vertex
                getVertex(buff, vertices);
            }
        }
        else if(buff[0] == 'p')
        {
            // points
            getIndices(buff, vertices, texCoords, normals, vertexBuffer, texCoordBuffer, normalBuffer, isQuad);
            vertexMode = MeshModel::VertexMode_Points;
        }
        else if(buff[0] == 'l')
        {
            // lines
            getIndices(buff, vertices, texCoords, normals, vertexBuffer, texCoordBuffer, normalBuffer, isQuad);
            vertexMode = MeshModel::VertexMode_Lines;
        }
        else if(buff[0] == 'f')
        {
            // faces
            getIndices(buff, vertices, texCoords, normals, vertexBuffer, texCoordBuffer, normalBuffer, isQuad);
            if(isQuad)
                vertexMode = MeshModel::VertexMode_Quads;
        }
    }

    f.close();

    MeshModel* model = new MeshModel();

    model->setVertexBuffer(vertexBuffer.data(), vertexBuffer.size());
    model->setTexCoordBuffer(texCoordBuffer.data(), texCoordBuffer.size());
    model->setNormalBuffer(normalBuffer.data(), normalBuffer.size());

    model->setVertexMode(vertexMode);
    model->updateVBO(false);

    return model;
}

IMPGEARS_END
