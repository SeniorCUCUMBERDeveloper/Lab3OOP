#ifndef ICONTAINER_HPP
#define ICONTAINER_HPP

#include <tuple>
#include <string>
#include <vector>
#include <iostream>
#include <memory>



class IContainer{
public:
    virtual std::string isType() const = 0;
    virtual void getInfo(std::ostream& output) const = 0;
    virtual void setId(int X, int Y, int Z) = 0;
    virtual std::string getId() const = 0;
    virtual int getLength() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual double getMass() const = 0;
    virtual std::shared_ptr<IContainer> Clone(size_t i = 0, size_t method = 0) = 0;
    virtual ~IContainer() = default;
};

#endif