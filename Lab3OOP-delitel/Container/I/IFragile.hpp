#ifndef IFRAGILE_HPP
#define IFRAGILE_HPP

#include "IContainer.hpp"

class IFragileContainer: virtual public IContainer{
    public:
        virtual double getMaxPressure() const = 0;
};

#endif