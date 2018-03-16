#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include <vector>
#include <memory>

#include <Core/Object.h>
#include <Core/Matrix4.h>
#include <SceneGraph/Texture.h>

IMPGEARS_BEGIN

#define IMP_GLSL_SRC( src ) #src

/// \brief Defines a shader (vertex and fragment shaders).
class IMP_API Shader : public Object
{
    public:
	
		/// \brief Defines a shader parameter (uniform).
		class Parameter : public Object
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
				Type_Mat4v,
				Type_Sampler
			};
			
			union Value
			{
				float	 value_1f;
				const Vector3* value_3f;
				int		 value_1i;
				const float*	 value_1fv;
				const Vector3* value_3fv;
				const int*	 value_1iv;
				// const Matrix3* value_mat3v;
				const Matrix4* value_mat4v;
			};
			
			Meta_Class(Parameter)
			
			Parameter(const std::string& id, Type type);
			
			~Parameter();
			
			void set(float float1);
			
			void set(const Vector3* vec3);
			
			void set(int int1);
			
			void set(const float* float1Array, int count = 1);
			
			void set(const Vector3* vec3Array, int count);
			
			void set(const int* int1Array, int count = 1);
			
			// void set(const Matrix3* mat3Array, int count);
			
			void set(const Matrix4* mat4Array, int count = 1);
			
			void set(const Texture* texture, std::int32_t textureUnit = 0);
			
			const std::string& getID() const {return id;}
			
			void updateUniform(const Shader& program) const;
			
			private:
			
			std::string id;
			std::uint32_t count;
			Type type;
			Value value;
			const Texture* sampler;
			
		};
		
		Meta_Class(Shader)
	
        Shader(const char* vertexShader, const char* fragmentShader);
        virtual ~Shader();

        void setTextureParameter(const char* name, const Texture* texture, std::int32_t textureUnit = 0);
        void setFloatParameter(const char* name, float value);
        void setMatrix4Parameter(const char* name, const Matrix4& Matrix4);
        void setVector3ArrayParameter(const char* name, float* vector3Array, std::uint32_t count);
        void setVector3Parameter(const char* name, const Vector3& vec3);
		
		void setParameter(const Parameter& param);

        void setProjection(const Matrix4& projection);
        void setView(const Matrix4& view);
        void setModel(const Matrix4& model);

        void enable();
        void disable();
		
		std::int32_t getParameterLocation(const std::string& id) const;
		std::uint32_t getID() const {return m_programID;}

        static const Shader* getActiveShader() {return m_instance;}
		
		void addParameter(const std::shared_ptr<Parameter>& param);
		void removeParameter(Parameter* param);
		void clearParameter();
		Parameter* getParameter(const std::string& name);
		void updateAllParameters();
		void updateParameter(const std::string& name);
		
		void addTextureParameter(const std::string& name, const Texture* texture, std::int32_t textureUnit = 0);
        void addFloatParameter(const std::string& name, float value);
        void addMatrix4Parameter(const std::string& name, const Matrix4* Matrix4);
        void addVector3ArrayParameter(const std::string& name,  float* vector3Array, std::uint32_t count);
        void addVector3Parameter(const std::string& name, const Vector3* vec3);
		
        void addProjection(const Matrix4* projection);
        void addView(const Matrix4* view);
        void addModel(const Matrix4* model);

    protected:
    private:

		bool				_valid;
        std::uint32_t m_vertexID;
        std::uint32_t m_fragmentID;
        std::uint32_t m_programID;

        static Shader* m_instance;
		
		std::vector< std::shared_ptr<Parameter> > _parameters;
};

IMPGEARS_END

#endif // IMP_SHADER_H
