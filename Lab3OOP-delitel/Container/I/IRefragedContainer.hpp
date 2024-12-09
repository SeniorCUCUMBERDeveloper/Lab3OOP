#ifndef IREFRAGEDCONTAINER_HPP
#define IREFRAGEDCONTAINER_HPP


#include "IContainer.hpp"

class IRefragedContainer : virtual public IContainer{
    public:
        virtual double getMaxTemperature() const = 0;
};

#endif