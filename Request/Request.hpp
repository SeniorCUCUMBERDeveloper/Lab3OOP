#ifndef REQUEST_HPP
#define REQUEST_HPP


#include "../Container/Container.hpp"
#include "../Storage/Storage.hpp"
#include <random>
#include <set>
#include <iterator>



class Request{
    private:
        Storage* storage;
        size_t operation;
    public:
        Request(Storage* st, size_t oper) : storage(st), operation(oper){}
        void RequestQ(){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> actionDist(0, 3);
            std::uniform_int_distribution<int> DimensionsDist(1, 10);
            std::uniform_int_distribution<int> ZDist(1, 2);
            std::uniform_int_distribution<int> RotateMethodDist(0, 5);
            std::uniform_int_distribution<int> indexDist(0, 100);
            std::vector<std::string> id;
            std::vector<std::string> op(operation, " ");
            for(size_t i = 0; i < operation; ++i){
                try{
                    int action = actionDist(gen);
                    if(action == 0){
                        IContainer* c = new Container("0", "Andre", DimensionsDist(gen), DimensionsDist(gen), 1,(double)(DimensionsDist(gen)), (double)(DimensionsDist(gen)));
                        std::string flag = storage->addContainer(c);
                        if(flag == "_"){
                            std::cerr << "Error in RequestQ: Can't add container with this index" << std::endl;
                            --i;
                            delete c;
                        }else{
                            std::cout << "Container added successfully" << std::endl;
                        }
                        storage->getInfo();
                       op[i] = "add " + std::to_string(c->getLength()) + " X " + std::to_string(c->getWidth()) + " X " + std::to_string(c->getHeight()) + "\n";
                       std::cout << "\n-------------------------------\n" << std::endl;
                       std::cout << op[i] << std::endl;
                       std::cout << storage->getInfo() << std::endl;
                    } 
                    if(action == 1){
                        id = storage->getListContainers();
                        if(id.empty()){
                            throw std::invalid_argument("Empty");
                        }
                        auto it = id.begin() + DimensionsDist(gen) % (id.size() ); 
                        std::string s = *it;     
                        storage->removeContainer(*it);
                        storage->getInfo();
                        std::cout << "Container removed successfully" << std::endl;
                        op[i] = "remove " + s + "\n";
                        std::cout << "\n-------------------------------\n" << std::endl;
                       std::cout << op[i] << std::endl;
                       std::cout << storage->getInfo() << std::endl;
                    }
                    if(action == 2){
                        id = storage->getListContainers();
                        if(id.empty()){
                            throw std::invalid_argument("Empty");
                        }
                        auto it = id.begin() + DimensionsDist(gen) % (id.size());  
                        int method = RotateMethodDist(gen);
                        storage->rotateContainer(*it, method);
                        std::cout << "Complete random Rotate Method" << std::endl;
                        op[i] = "rotate " + (*it) + " " + std::to_string(method) + "\n";
                        std::cout << "\n-------------------------------\n" << std::endl;
                       std::cout << op[i] << std::endl;
                       std::cout << storage->getInfo() << std::endl;
                    }
                    if(action == 3){
                        id = storage->getListContainers();
                        if(id.empty()){
                            throw std::invalid_argument("Empty");
                        }
                        auto it = id.begin() + DimensionsDist(gen) % (id.size());  
                        int X = DimensionsDist(gen);
                        int Y = DimensionsDist(gen);
                        int Z = DimensionsDist(gen);
                        storage->moveContainer(*it, X, Y, Z);
                        std::cout << "Container moved successfully" << std::endl;
                        op[i] = "move " + (*it) + " to " + std::to_string(X) + " " + std::to_string(Y) + " " + std::to_string(Z) + "\n";
                        std::cout << "\n-------------------------------\n" << std::endl;
                       std::cout << op[i] << std::endl;
                       std::cout << storage->getInfo() << std::endl;
                        
                    }
                }catch(std::exception& e){
                    std::cerr << "Error in RequestQ: " << e.what() << std::endl;
                    --i;
                }
                //std::this_thread::sleep_for(std::chrono::milliseconds(500)); не надо здесь
            }
            std::cout << "\n-------------------------------\n" << std::endl;
            std::cout << storage->getInfo() << std::endl;
            for(size_t i = 0; i < operation; ++i){
                std::cout << "Operation " << i + 1 << ": " << op[i];
            }
        }
};


#endif