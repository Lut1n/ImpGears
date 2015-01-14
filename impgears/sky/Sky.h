#ifndef SKY_H
#define SKY_H

#include <iostream>
#include "../base/Vector3.h"

class Sky
{
    public:
        Sky();
        Sky(const std::string& filename);
        virtual ~Sky();

        virtual void load(const std::string& filename);
        virtual void update(const imp::Vector3& position, const imp::Vector3& orientation) = 0;
        virtual void render() = 0;
    protected:
    private:
};

#endif // SKY_H
