#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "../Container/I/IContainer.hpp"
#include <thread> 
#include <vector>
#include <mutex> 
#include <atomic>
#include <shared_mutex>
#include <iostream>
#include "../Octree/Octree.hpp"
#include "../Checker/Checker.hpp"


class Storage{
    private:
        int number;
        int length, width, height;
        double temperature;
        Octree<int, IContainer*, ContainerPosition<int>>* containers;
        Checker<int> checker;

        public:
          int getLength(){
            return length;
          }
          int getWidth(){
            return width;
          }
          int getHeight(){
            return height;
          }     
          Storage(){}
          void addExternalCheckFunction(const std::function<void(Storage&, IContainer*, ContainerPosition<int>)>& externalFunc);
          Storage(int number, int length, int width, int height, double temperature);
          Storage(const Storage& other);
          std::string addContainer(IContainer* container);
          void moveContainer(std::string id, int X, int Y, int Z);
          void rotateContainer(std::string id, int method);
          void removeContainer(std::string id);
          std::string getInfo() const;
          int getTemperature() const{
            return temperature;
          }
          size_t howContainer(IContainer* container);
          void addContainer(IContainer* container, int X, int Y, int Z);
          void getSize(int l, int w, int h);
          std::string getInfoAboutStorage() const;
          std::vector<std::string> getListContainers() const;
          ~Storage(){
            std::cout << "Storage destructor" << std::endl;
            delete containers;
            }

          std::pair<ContainerPosition<int>, IContainer*> find(std::string id);

          Storage& operator=(const Storage& other);


        private:
          std::mutex mtx;
          mutable std::shared_mutex shared_mtx;
          std::atomic<bool> containerAdded{false}; 
          int calculateDepth();
          static bool comparePosition(std::pair<ContainerPosition<int>, IContainer*>& pos1, std::pair<ContainerPosition<int>, IContainer*>& pos2);
          static bool comparePositionReverse(std::pair<ContainerPosition<int>, IContainer*>& pos1, std::pair<ContainerPosition<int>, IContainer*>& pos2);
          bool isNoTop(const ContainerPosition<int>& position);
          std::vector<std::pair<ContainerPosition<int>,IContainer*>> searchUnderContainer(ContainerPosition<int>& position);
          static double calculatemass(std::vector<std::pair<ContainerPosition<int>, IContainer*>> con, size_t it);
          bool addContainerR(IContainer* container,  int yStart, int yEnd);
          ContainerPosition<int> calculateContainerPosition(int x, int y, int z, int l, int w, int h);
          bool moveContainer(std::pair<ContainerPosition<int>, IContainer*> it);
          std::pair<ContainerPosition<int>, IContainer*> isTop(const ContainerPosition<int>& position);
           void multitread(IContainer* container, int X, int Y, int Z);
           std::vector<std::pair<ContainerPosition<int>,IContainer*>> searchUpperContainer(ContainerPosition<int>& position);
           void howContai(IContainer* container, std::vector<size_t>& result, size_t method);
           static bool checkSupport(ContainerPosition<int>& position, std::vector<std::pair<ContainerPosition<int>,IContainer*>> con);
           static void checkTemperature(Storage& storage, IContainer* container, ContainerPosition<int> position);
           static void checkPressure(Storage& storage, IContainer* container, ContainerPosition<int> position);
          

};
#endif