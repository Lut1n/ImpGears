#ifndef IMP_KEYBINDINGCONFIG_H
#define IMP_KEYBINDINGCONFIG_H

#include "base/impBase.h"
#include "io/Streamable.h"

#include <map>

IMPGEARS_BEGIN

class IMP_API KeyBindingConfig : public Streamable
{
	public:

		KeyBindingConfig();
		virtual ~KeyBindingConfig();

		virtual void Load(Parser* _parser);
		virtual void Save(Parser* _parser);

		Uint32 getKey(const char* name) const;

	protected:
	private:
		std::map<std::string, Uint32> m_keysMap;
};

IMPGEARS_END

#endif // IMP_KEYBINDINGCONFIG_H
