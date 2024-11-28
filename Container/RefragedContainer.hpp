#ifndef REF_HPP
#define REF_HPP


#include "./I/IRefragedContainer.hpp"
#include <iostream>

class RefragedContainer : public IRefragedContainer{
    protected:
        std::string number;                      
        std::string client;              
        int length, width, height;       
        double cost;                     
        double mass;
        double maxTemperature;
    public:
    RefragedContainer(std::string number, std::string client, int length, int width, int height, double cost, double mass, double maxT){
        this->number = number;
        this->client = client;
        this->length = length;
        this->width = width;
        this->height = height;
        this->cost = cost;
        this->mass = mass;
        this->maxTemperature = maxT;
    }

    ~RefragedContainer(){}

    int getLength() const override{
            return length;
        }
        int getWidth() const override{
            return width;
        }
        int getHeight() const override{
            return height;
        }

        std::string getId() const override{
            return number;
        }

        void setId(int X, int Y, int Z) override{
            number = std::to_string(X) + "_" + std::to_string(Y) + "_" + std::to_string(Z);
        }

        double getMass() const override{
            return mass;
        }

    void getInfo(std::ostream& output) const override{
        output << "Container ID: " << number << std::endl;
        output << "Client: " << client << std::endl;
        output << "Dimensions: " << length << "x" << width << "x" << height << std::endl;
        output << "Cost: $" << cost << std::endl;
        output << "Mass: " << mass << " kg" << std::endl;
        output << "MaxTemperature: " << maxTemperature << std::endl;
    }

    std::string isType() const override { return "Refraged";}

    double getMaxTemperature() const override{
        return maxTemperature;
    }
    IContainer* Clone(size_t i = 0, size_t method = 0) override {
        switch (method)
        {
        case 0:
            return new RefragedContainer(number, client, length, width, height, cost, mass, maxTemperature);
        case 1:
            return new RefragedContainer(number, client, width, length, height, cost, mass, maxTemperature);
        case 2:
            return new RefragedContainer(number, client, length, height, width, cost, mass, maxTemperature);
        case 3:
            return new RefragedContainer(number, client, height, length, width, cost, mass, maxTemperature);
        case 4:
            return new RefragedContainer(number, client, width, height, length, cost, mass, maxTemperature);
        case 5:
            return new RefragedContainer(number, client, height, width, length, cost, mass, maxTemperature);
        default:
            return new RefragedContainer(number, client, length, width, height, cost, mass, maxTemperature);
        }
    }
};

#endif