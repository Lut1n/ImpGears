#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include <vector>

#include "Core/impBase.h"
#include "Math/Matrix4.h"
#include "Graphics/Texture.h"

IMPGEARS_BEGIN

#define IMP_GLSL_SRC( src ) #src

/// \brief Defines a shader (vertex and fragment shaders).
class IMP_API Shader
{
    public:
	
		/// \brief Defines a shader parameter (uniform).
		class Parameter
		{
			public:
			
			enum Type
			{
				Type_Undefined,
				Type_1f,
				//Type_2f,
				Type_3f,
				//Type_4f,
				
				Type_1i,
				//Type_2i,
				//Type_3i,
				//Type_4i,
				
				//Type_1ui,
				//Type_2ui,
				//Type_3ui,
				//Type_4ui,
				
				Type_1fv,
				//Type_2fv,
				Type_3fv,
				//Type_4fv,
				
				Type_1iv,
				//Type_2iv,
				//Type_3iv,
				//Type_4iv,
				
				//Type_1uiv,
				//Type_2uiv,
				//Type_3uiv,
				//Type_4uiv,
				
				//Type_Mat2v,
				// Type_Mat3v,
				Type_Mat4v
			};
			
			union Value
			{
				float	 value_1f;
				Vector3* value_3f;
				int		 value_1i;
				float*	 value_1fv;
				Vector3* value_3fv;
				int*	 value_1iv;
				// Matrix3* value_mat3v;
				Matrix4* value_mat4v;
			};
			
			Parameter(const std::string& id, Type type);
			
			~Parameter();
			
			void set(float float1);
			
			void set(Vector3* vec3);
			
			void set(int int1);
			
			void set(float* float1Array, int count);
			
			void set(Vector3* vec3Array, int count);
			
			void set(int* int1Array, int count);
			
			// void set(Matrix3* mat3Array, int count);
			
			void set(Matrix4* mat4Array, int count);
			
			const std::string& getID() const {return id;}
			
			void updateUniform(const Shader& program) const;
			
			private:
			
			std::string id;
			Uint32 count;
			Type type;
			Value value;
			
		};
	
        Shader(const char* vertexShader, const char* fragmentShader);
        virtual ~Shader();

        void setTextureParameter(const char* name, const Texture* texture, Int32 textureUnit = 0);
        void setFloatParameter(const char* name, float value);
        void setMatrix4Parameter(const char* name, const Matrix4& Matrix4);
        void setVector3ArrayParameter(const char* name, float* vector3Array, Uint32 count);
        void setVector3Parameter(const char* name, const Vector3& vec3);
		
		void setParameter(const Parameter& param);

        void setProjection(const Matrix4& projection);
        void setView(const Matrix4& view);
        void setModel(const Matrix4& model);

        void enable();
        void disable();
		
		Int32 getParameterLocation(const std::string& id) const;
		Uint32 getID() const {return m_programID;}

        static const Shader* getActiveShader() {return m_instance;}
		
		void addParameter(Parameter* param);
		void removeParameter(Parameter* param);
		void clearParameter();
		Parameter* getParameter(const std::string& name);
		void updateAllParameters();
		void updateParameter(const std::string& name);

    protected:
    private:

        Uint32 m_vertexID;
        Uint32 m_fragmentID;
        Uint32 m_programID;

        static Shader* m_instance;
		
		std::vector<Parameter*> _parameters;
};

IMPGEARS_END

#endif // IMP_SHADER_H
