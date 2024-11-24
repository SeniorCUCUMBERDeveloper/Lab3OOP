#ifndef FRAGILECONTAINER_HPP
#define FRAGILECONTAINER_HPP

#include "./I/IFragile.hpp"


class FragileContainer : public IFragileContainer{
    protected:
        std::string number;                      
        std::string client;              
        int length, width, height;       
        double cost;                     
        double mass;
        double maxPressure;
    public:
        FragileContainer(std::string number, std::string client, int length, int width, int height, double cost, double mass,  double maxM){
            this->number = number;
            this->client = client;
            this->length = length;
            this->width = width;
            this->height = height;
            this->cost = cost;
            this->mass = mass;
            this->maxPressure = maxM;
        }

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
        output << "MaxPressure: " << maxPressure << std::endl;
    }

        std::string isType() const override { return "Fragile"; }

        double getMaxPressure() const override{ return maxPressure; }

        IContainer* Clone(size_t i = 0, size_t method = 0) override { 
            switch (method)
            {
            case 0:
                return new FragileContainer(number, client, length, width, height, cost, mass, maxPressure);
            case 1:
                return new FragileContainer(number, client, width, length, height, cost, mass, maxPressure);
            case 2:
                return new FragileContainer(number, client, length, height, width, cost, mass, maxPressure);
            case 3:
                return new FragileContainer(number, client, height, length, width, cost, mass, maxPressure);
            case 4:
                return new FragileContainer(number, client, width, height, length, cost, mass, maxPressure);
            case 5:
                return new FragileContainer(number, client, height, width, length, cost, mass, maxPressure);
            default:
                return new FragileContainer(number, client, length, width, height, cost, mass, maxPressure);
            }
        }
};

#endif