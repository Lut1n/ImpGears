#include "Data/OBJMeshLoader.h"
#include <System/Parser.h>
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

imp::Uint32 parseIndices(const char* buff, std::vector<imp::Uint32>& vArr, std::vector<imp::Uint32>& vtArr,std::vector<imp::Uint32>& vnArr)
{
    imp::Uint32 nbIndices = 0;

    std::string strBuff(buff), sub;
    strBuff = strBuff.substr(strBuff.find(" ")+1); // remove "f "

    while(strBuff.size() > 0)
    {
        imp::Int32 spaceIt = strBuff.find(" ");

		if(spaceIt == -1)
		{
			sub = strBuff.c_str();
			strBuff = "";
		}
        else
		{
			sub = strBuff.substr(0, spaceIt);
			strBuff = strBuff.substr(spaceIt+1);
		}

        normalizeIndicesString(sub);

        imp::Uint32 v=0, vt=0, vn=0;
        std::sscanf(sub.c_str(), "%u/%u/%u", &v, &vt, &vn);

        vArr.push_back(v); vtArr.push_back(vt); vnArr.push_back(vn);
        ++nbIndices;
    }

    return nbIndices;
}

void getIndices(std::string& buff, std::vector<float>& vertices, std::vector<float>& texCoords, std::vector<float>& normals,
                std::vector<float>& vertexBuffer, std::vector<float>& texCoordBuffer, std::vector<float>& normalBuffer, bool& isQuad)
{
    isQuad = false;
    int nbInd = occurrenceOf(buff.c_str(), ' ');
    if(nbInd == 4)
        isQuad = true;

    std::vector<imp::Uint32> vIndices;
    std::vector<imp::Uint32> vtIndices;
    std::vector<imp::Uint32> vnIndices;
    imp::Uint32 nbIndices = parseIndices(buff.c_str(), vIndices, vtIndices, vnIndices);
    for(imp::Uint32 i=0; i<nbIndices; ++i)
    {
        imp::Uint32 v = vIndices[i] - 1;
        imp::Uint32 vt = vtIndices[i] - 1;
        imp::Uint32 vn = vnIndices[i] - 1;

        vertexBuffer.push_back(vertices[v*3]);
        vertexBuffer.push_back(vertices[v*3+1]);
        vertexBuffer.push_back(vertices[v*3+2]);

        texCoordBuffer.push_back(texCoords[vt*2]);
        texCoordBuffer.push_back(texCoords[vt*2+1]);

        normalBuffer.push_back(normals[vn*3]);
        normalBuffer.push_back(normals[vn*3+1]);
        normalBuffer.push_back(normals[vn*3+2]);
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

bool OBJMeshLoader::saveToFile(const MeshModel* meshModel, const char* filename)
{
    //imp::Parser file(filename, imp::Parser::FileType_Text, imp::Parser::AccessMode_Write);

    //const char* head="test save model file";
    //file.Write(head, 19);

    std::ofstream f(filename, std::ofstream::out);
    f << "test save model file";
    f.close();

    return true;
}


IMPGEARS_END
