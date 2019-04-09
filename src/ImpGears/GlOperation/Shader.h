#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include <Core/Object.h>

IMPGEARS_BEGIN

#define IMP_GLSL_SRC( src ) #src

class IMP_API Shader : public Object
{
public:
	
	Meta_Class(Shader)

	Shader();
	Shader(const std::string& vertCode, const std::string& fragCode);
	virtual ~Shader();
	
	void load(const std::string& vertCode, const std::string& fragCode);
	void compile(std::uint32_t srcId);
	void use();
	
	std::int32_t locate(const std::string& id) const;
	std::uint32_t id() const {return _programID;}

protected:

	std::uint32_t _vertID;
	std::uint32_t _fragID;
	std::uint32_t _programID;
	
	static std::string s_compilation_log;
};

IMPGEARS_END

#endif // IMP_SHADER_H
