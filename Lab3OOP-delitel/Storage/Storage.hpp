#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "../Container/I/IContainer.hpp"
#include <thread> 
#include <vector>
#include <list>
#include <mutex> 
#include <atomic>
#include <shared_mutex>
#include <iostream>
#include "../Octree/Octree.hpp"
#include "../Checker/Checker.hpp"
#include <condition_variable>


class Storage{
    private:
        int number;
        int length, width, height;
        double temperature;
        Octree<Point<int>, std::shared_ptr<IContainer>> containers;
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
          std::vector<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> getALLcontainers(){
            auto i = containers.searchDepth();
            return i;
          }
          void addExternalCheckFunction(const std::function<void(Storage&, std::shared_ptr<IContainer>, ContainerPosition<Point<int>>)>& externalFunc);
          Storage(int number, int length, int width, int height, double temperature);
          Storage(const Storage& other);
          std::string addContainer(std::shared_ptr<IContainer> container);
          void moveContainer(std::string id, int X, int Y, int Z);
          void rotateContainer(std::string id, int method);
          void removeContainer(std::string id);
          std::string getInfo() const;
          int getTemperature() const{
            return temperature;
          }
          size_t howContainer(std::shared_ptr<IContainer> container);
          void addContainer(std::shared_ptr<IContainer>, int X, int Y, int Z);
          void getSize(int l, int w, int h);
          std::string getInfoAboutStorage() const;
          std::vector<std::string> getListContainers() const;
          ~Storage(){
            }

          std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>> find(std::string id);

          Storage& operator=(const Storage& other);


        private:
          std::mutex mtx;
          mutable std::shared_mutex smtx;
          std::atomic<bool> containerAdded{false}; 
          int calculateDepth();
          static bool comparePosition(std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos1, std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos2);
          static bool comparePositionReverse(std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos1, std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos2);
          bool isNoTop(const ContainerPosition<Point<int>>& position);
          std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> searchUnderContainer(ContainerPosition<Point<int>>& position);
          static double calculatemass(std::vector<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> con, size_t it);
          bool addContainerR(std::shared_ptr<IContainer> container,  int yStart, int yEnd, Point<int>& point);
          bool moveContainer(std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>> it);
          std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>> isTop(const ContainerPosition<Point<int>>& position);
           Point<int> multitread(std::shared_ptr<IContainer> container, int X, int Y, int Z);
           std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> searchUpperContainer(ContainerPosition<Point<int>>& position);
           void howContai(std::shared_ptr<IContainer> container, std::vector<size_t>& result, size_t method);
           static bool checkSupport(ContainerPosition<Point<int>>& position, std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> con);
           static void checkTemperature(Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> position);
           static void checkPressure(Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> position);

           std::list<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> searchAllContainersUpper(std::list<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> dec);

           Point<int> parsePoint(const std::string& str);
           std::string numeric(const Point<int>& p);

          

};
#endif