#ifndef CONTAINER_HPP
#define CONTAINER_HPP


#include <iostream>
#include <string>
#include <memory>
#include "./I/IContainer.hpp"



class Container: public IContainer{
    protected:
        std::string number;                      
        std::string client;              
        int length, width, height;       
        double cost;                     
        double mass;
    public:

        Container() 
        : number(0), client(""), length(0), width(0), height(0), cost(0), mass(0) {}
        Container(std::string number, std::string client, int length, int width, int height, double cost, double mass){
            this->number = number;
            this->client = client;
            this->length = length;
            this->width = width;
            this->height = height;
            this->cost = cost;
            this->mass = mass;
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

        double getMass() const override{
            return mass;
        }

        void setId(int X, int Y, int Z) override{
            number = std::to_string(X) + "_" + std::to_string(Y) + "_" + std::to_string(Z);
        }

    void getInfo(std::ostream& output) const override{
        output << "Container ID: " << number << std::endl;
        output << "Client: " << client << std::endl;
        output << "Dimensions: " << length << "x" << width << "x" << height << std::endl;
        output << "Cost: $" << cost << std::endl;
        output << "Mass: " << mass << " kg" << std::endl;
    };
    std::shared_ptr<IContainer> Clone(size_t i = 0, size_t method = 0) override{
        switch (method)
        {
        case 0:
            return std::make_shared<Container>(number, client, length, width, height, cost, mass);
        case 1:
            return std::make_shared<Container>(number, client, width, length, height, cost, mass);
        case 2:
            return std::make_shared<Container>(number, client, length, height, width, cost, mass);
        case 3:
            return std::make_shared<Container>(number, client, height, length, width, cost, mass);
        case 4:
            return std::make_shared<Container>(number, client, width, height, length, cost, mass);
        case 5:
            return std::make_shared<Container>(number, client, height, width, length, cost, mass);
        default:
            return std::make_shared<Container>(number, client, length, width, height, cost, mass);
        }
    }

    std::string isType() const override{
        return "Default Container";
    }
    ~Container() override {}

};


#endif