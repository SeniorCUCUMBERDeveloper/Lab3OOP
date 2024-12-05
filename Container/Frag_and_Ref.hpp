#ifndef FRAG_AND_REF_HPP
#define FRAG_AND_REF_HPP


#include "FragileContainer.hpp"
#include "RefragedContainer.hpp"


class FragileRefragedContainer : public IFragileContainer , public IRefragedContainer{
    protected:
        std::string number;                      
        std::string client;              
        int length, width, height;       
        double cost;                     
        double mass;
        double maxPressure;
        double maxTemperature;
    public:
        FragileRefragedContainer(std::string number, std::string client, int length, int width, int height, double cost, double mass, double maxM, double maxT){
            this->number = number;
            this->client = client;
            this->length = length;
            this->width = width;
            this->height = height;
            this->cost = cost;
            this->mass = mass;
            this->maxPressure = maxM;
            this->maxTemperature = maxT;
        }
        std::string isType() const override { return "Fragile and Refraged Container";}

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
        output << "Max Temperature: " << maxTemperature << std::endl;
    }
        double getMaxPressure() const override{ return maxPressure; }
        double getMaxTemperature() const override{ return maxTemperature; }
        std::shared_ptr<IContainer> Clone(size_t i = 0, size_t method = 0) override{ 
            switch (method)
            {
            case 0:
                return std::make_shared<FragileRefragedContainer>(number, client, length, width, height, cost, mass,  maxPressure, maxTemperature);
            case 1:
                return std::make_shared<FragileRefragedContainer>(number, client, width, length, height, cost, mass,  maxPressure, maxTemperature);
            case 2:
                return std::make_shared<FragileRefragedContainer>(number, client, length, height, width, cost, mass,  maxPressure, maxTemperature);
            case 3:
                return std::make_shared<FragileRefragedContainer>(number, client, height, length, width, cost, mass,  maxPressure, maxTemperature);
            case 4:
                return std::make_shared<FragileRefragedContainer>(number, client, width, height, length, cost, mass,  maxPressure, maxTemperature);
            case 5:
                return std::make_shared<FragileRefragedContainer>(number, client, height, width, length, cost, mass,  maxPressure, maxTemperature);
            default:
                return std::make_shared<FragileRefragedContainer>(number, client, length, width, height, cost, mass,  maxPressure, maxTemperature);
            }
        }
        ~FragileRefragedContainer() override{}
};


#endif