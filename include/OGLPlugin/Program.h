#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include <Core/Object.h>

#include <map>

IMPGEARS_BEGIN

#define PROGRAM_DEFAULT_NAME "unnamed_program"
#define IMP_GLSL_SRC( src ) #src

class IMP_API Program : public Object
{
public:

    Meta_Class(Program)

    Program(const std::string& name = PROGRAM_DEFAULT_NAME);
    Program(const std::string& vertCode, const std::string& fragCode, const std::string& name = PROGRAM_DEFAULT_NAME);
    virtual ~Program();

    void load(const std::string& vertCode, const std::string& fragCode);
    void compile(std::uint32_t srcId);
    void use();

    std::int32_t locate(const std::string& id) const;
    std::uint32_t id() const {return _programID;}

    void rename(const std::string& name);

protected:

    std::string _name;
    std::uint32_t _vertID;
    std::uint32_t _fragID;
    std::uint32_t _programID;

    static std::string s_compilation_log;

    mutable std::map<std::string,int> _locationCache;
};

IMPGEARS_END

#endif // IMP_SHADER_H
