#ifndef REQUEST_HPP
#define REQUEST_HPP


#include "../Container/Container.hpp"
#include "../Storage/Storage.hpp"
#include <random>
#include <set>
#include <iterator>



class Request{
    private:
        Storage& storage;
        size_t operation;
    public:
        Request(Storage& st, size_t oper) : storage(st), operation(oper){}
        void RequestQ(){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> actionDist(0, 1);
            std::uniform_int_distribution<int> DimensionsDist(1, 10);
            std::uniform_int_distribution<int> ZDist(1, 2);
            std::uniform_int_distribution<int> RotateMethodDist(0, 5);
            std::uniform_int_distribution<int> indexDist(0, 100);
            std::vector<std::string> id;
            for(size_t i = 0; i < 300; ++i){
                try{
                    int action = actionDist(gen);
                    if(action == 0){
                        IContainer* c = new Container("0", "Andre", DimensionsDist(gen), DimensionsDist(gen), 1,(double)(DimensionsDist(gen)), (double)(DimensionsDist(gen)));
                        std::string flag = storage.addContainer(c);
                        if(flag == "_"){
                            //std::cerr << "Error in RequestQ: Can't add container with this index" << std::endl;
                            --i;
                            delete c;
                        }else{
                            std::cout << "Container added successfully" << std::endl;
                        }
                        storage.getInfo();
                       std::cout << "\n-------------------------------\n" << std::endl;
                       std::cout << storage.getInfo() << std::endl;
                    } 
                }catch(std::exception& e){
                    std::cerr << "Error in RequestQ: " << e.what() << std::endl;
                    --i;
                }
                //std::this_thread::sleep_for(std::chrono::milliseconds(500)); не надо здесь
            }
            bool flag = false;
            for(int i = 0; i < 150; ++i){
                try{
                id = storage.getListContainers();
                        if(id.empty()){
                            throw std::invalid_argument("Empty");
                        }
                        auto it = id.begin() + DimensionsDist(gen) % (id.size() ); 
                        std::string s = *it;     
                        std::cout << s << std::endl;
                        storage.removeContainer(*it);
                        storage.getInfo();
                        std::cout << "Container removed successfully" << std::endl;
                        std::cout << "\n-------------------------------\n" << std::endl;
                       std::cout << storage.getInfo() << std::endl;
                }catch(std::exception& e){
                    std::cerr << "Error in RequestQ: " << e.what() << std::endl;
                    if(flag == true){
                        //return;
                    }
                    flag = true;
                }
            std::cout << "\n-------------------------------\n" << std::endl;
                std::cout << storage.getInfo() << std::endl;
        //     // for(size_t i = 0; i < operation; ++i){
        //     //     std::cout << "Operation " << i + 1 << ": " << op[i];
        //     // }
         }
        }

};


#endif