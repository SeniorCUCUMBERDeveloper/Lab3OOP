#include "Storage.hpp"
#include "../Container/I/IFragile.hpp"
#include "../Octree/Octree.hpp"
#include "../Container/I/IRefragedContainer.hpp"
#include <iostream>
#include <algorithm>
#include <exception>
#include <numeric>
#include <cmath>
#include <vector>


int Storage::calculateDepth(){
    auto countDivisionsByTwo = [](int num) {
        int count = 0;
        while (num > 0 && num % 2 == 0) {
            num /= 2;
            count++;
        }
        return count;
    };
    return std::min({countDivisionsByTwo(length), countDivisionsByTwo(width), countDivisionsByTwo(height)});
    
}


  void Storage::addExternalCheckFunction(const std::function<void(Storage&, IContainer*, ContainerPosition<int>)>& externalFunc) {
        checker.addCheckFunction(externalFunc);
    }


Storage::Storage(int number, int length, int width, int height, double temperature) : checker(){
    this->number = number;
    this->length = length;
    this->width = width;
    this->height = height;
    this->temperature = temperature;
    BoundingBox<int> bound(Point<int>(0, 0, 0), Point<int>(length, width, height));
    this->containers = new Octree<int, IContainer*, ContainerPosition<int>>(bound, calculateDepth());
    this->checker.addCheckFunction([this](Storage& storage, IContainer* container, ContainerPosition<int> position) {
        checkTemperature(storage, container, position);
    });
    this->checker.addCheckFunction([this](Storage& storage, IContainer* container, ContainerPosition<int> position) {
        checkPressure(storage, container, position);
    });
}


bool Storage::comparePosition(std::pair<ContainerPosition<int>, IContainer*>& pos1, std::pair<ContainerPosition<int>, IContainer*>& pos2){
    return pos1.first.LLDown.z < pos2.first.LLDown.z;
}


bool Storage::comparePositionReverse(std::pair<ContainerPosition<int>, IContainer*>& pos1, std::pair<ContainerPosition<int>, IContainer*>& pos2){
    return pos1.first.LLDown.z > pos2.first.LLDown.z;
}


Storage& Storage::operator=(const Storage& other) {
        if (this != &other) { 
            number = other.number;
            length = other.length;
            width = other.width;
            height = other.height;
            temperature = other.temperature;
            containers = other.containers->Clone();
            auto i = other.containers->searchDepth();
            std::sort(i.begin(), i.end(), comparePosition);
            for(auto& it : i){
                addContainer(it.second->Clone(), it.first.LLDown.x, it.first.LLDown.y, it.first.LLDown.z);
            }
            Checker checker = other.checker;
        }
        return *this;
    }


Storage::Storage(const Storage& other)
        : number(other.number), length(other.length), width(other.width), height(other.height), temperature(other.temperature) {
        containers = other.containers->Clone(); // Используем интерфейс Clone для копирования
        auto i = other.containers->searchDepth();
        std::sort(i.begin(), i.end(), comparePosition);
        for(auto& it : i){
            addContainer(it.second->Clone(), it.first.LLDown.x, it.first.LLDown.y, it.first.LLDown.z);
        }
        Checker checker = other.checker;
    }


void Storage::getSize(int l, int w, int h){
    if(l < length || w < width || h < height){
        throw std::runtime_error("Storage l|w|h must be greater than or equal to length|width|height");
    }
    Storage newStorage(this->number, l, w, h, this->temperature);
    auto i = this->containers->searchDepth();
    std::sort(i.begin(), i.end(), comparePosition);
    for(auto& it : i){
        std::cout << it.second->getId() << std::endl;
    }
    for(auto& it : i){
        newStorage.addContainer(it.second->Clone(), it.first.LLDown.x, it.first.LLDown.y, it.first.LLDown.z);
    }
    delete containers;
    *this = newStorage;
}


//Вставка

ContainerPosition<int> Storage::calculateContainerPosition(int x, int y, int z, int l, int w, int h) {
    ContainerPosition<int> pos;
    //LLDown
    pos.LLDown.x = x;
    pos.LLDown.y = y;
    pos.LLDown.z = z;

    //LLUp
    pos.LLUp.x = x;
    pos.LLUp.y = y;
    pos.LLUp.z = z + h;

    //LRDown
    pos.LRDown.x = x + l;
    pos.LRDown.y = y;
    pos.LRDown.z = z;

    //LRUp
    pos.LRUp.x = x + l;
    pos.LRUp.y = y;
    pos.LRUp.z = z + h;

    //RRDown
    pos.RRDown.x = x + l;
    pos.RRDown.y = y + w;
    pos.RRDown.z = z;

    //RRUp
    pos.RRUp.x = x + l;
    pos.RRUp.y = y + w;
    pos.RRUp.z = z + h;

    //RLDown
    pos.RLDown.x = x;
    pos.RLDown.y = y + w;
    pos.RLDown.z = z;

    //RLUp
    pos.RLUp.x = x;
    pos.RLUp.y = y + w;
    pos.RLUp.z = z + h;
    return pos;
}


std::vector<std::pair<ContainerPosition<int>,IContainer*>> Storage::searchUnderContainer(ContainerPosition<int>& position){
    std::vector<std::pair<ContainerPosition<int>,IContainer*>> result;
    int minX = Octree<int, IContainer*, ContainerPosition<int>>::getMinX(position);
    int maxX = Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(position);
    int minY = Octree<int, IContainer*, ContainerPosition<int>>::getMinY(position);
    int maxY = Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(position);
    int minZ = Octree<int, IContainer*, ContainerPosition<int>>::getMinZ(position);
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    lock.lock();
    auto current = containers->searchDepth();
    lock.unlock();
    for (const auto& container : current) {
        const ContainerPosition<int>& containerPos = container.first;

        if (Octree<int, IContainer*, ContainerPosition<int>>::getMinX(containerPos) <= maxX && Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(containerPos) >= minX &&
            Octree<int, IContainer*, ContainerPosition<int>>::getMinY(containerPos) <= maxY && Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(containerPos) >= minY &&
            Octree<int, IContainer*, ContainerPosition<int>>::getMaxZ(containerPos) < minZ) {
                result.push_back(container);
                continue;
        }
    }
    std::sort(result.begin(), result.end(), comparePosition);
    return result;
}


std::vector<std::pair<ContainerPosition<int>,IContainer*>> Storage::searchUpperContainer(ContainerPosition<int>& position){
    std::vector<std::pair<ContainerPosition<int>,IContainer*>> result;
    int minX = Octree<int, IContainer*, ContainerPosition<int>>::getMinX(position);
    int maxX = Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(position);
    int minY = Octree<int, IContainer*, ContainerPosition<int>>::getMinY(position);
    int maxY = Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(position);
    int maxZ = Octree<int, IContainer*, ContainerPosition<int>>::getMaxZ(position);
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    lock.lock();
    auto current = containers->searchDepth();
    lock.unlock();
    for (const auto& container : current) {
        const ContainerPosition<int>& containerPos = container.first;
        
        if (Octree<int, IContainer*, ContainerPosition<int>>::getMinX(containerPos) <= maxX && Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(containerPos) >= minX &&
            Octree<int, IContainer*, ContainerPosition<int>>::getMinY(containerPos) <= maxY && Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(containerPos) >= minY &&
            Octree<int, IContainer*, ContainerPosition<int>>::getMinZ(containerPos) > maxZ) {
                result.push_back(container);
                continue;
        }
    }
    std::sort(result.begin(), result.end(), comparePositionReverse);
    return result;
}



double Storage::calculatemass(std::vector<std::pair<ContainerPosition<int>, IContainer*>> con, size_t it){
    double mass = 0;
    for(size_t i = it + 1; i < con.size(); i++){
        if((con[i].second)!= nullptr){
            mass += (*con[i].second).getMass();
        }
    }
    return mass;
}


bool Storage::checkSupport(ContainerPosition<int>& position, std::vector<std::pair<ContainerPosition<int>,IContainer*>> con){
    bool firstSupport = false;
    bool secondSupport = false;
    Point<int> first = Point<int>(position.LLDown.x, position.LLDown.y + (Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(position) - Octree<int, IContainer*, ContainerPosition<int>>::getMinY(position)) / 2, position.LLDown.z - 1);
    Point<int> second = Point<int>(position.LRDown.x, position.LRDown.y + (Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(position) - Octree<int, IContainer*, ContainerPosition<int>>::getMinY(position)) / 2, position.LLDown.z - 1);
    Point<int> middle = Point<int>(position.LLDown.x + (Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(position) - Octree<int, IContainer*, ContainerPosition<int>>::getMinX(position)) / 2, position.LLDown.y + (Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(position) - Octree<int, IContainer*, ContainerPosition<int>>::getMinY(position)) / 2, position.LLDown.z - 1);

    for(auto container = con.rbegin(); container != con.rend(); ++container){
        if(firstSupport == true && secondSupport == true){
            return true;
        }
        if((*container).first.LLUp.z + 1 != position.LLDown.z){
            return false;
        }
        if(Octree<int, IContainer*, ContainerPosition<int>>::pointincontainer(first, (*container).first)){
            firstSupport = true;
        }
        if(Octree<int, IContainer*, ContainerPosition<int>>::pointincontainer(second, (*container).first)){
            secondSupport = true;
        }
        if(Octree<int, IContainer*, ContainerPosition<int>>::pointincontainer(middle, (*container).first)){
            return true;
        }
    }
    return false;
}

void Storage::addContainer(IContainer* container, int X, int Y, int Z){
    if(X < 0 || Y < 0 || Z < 0){
        throw std::invalid_argument("Coordinates should be positive");
    }
    ContainerPosition<int> pos = calculateContainerPosition(X, Y, Z, container->getLength(), container->getWidth(), container->getHeight());
    auto cache = containers->SearchInsert(container, pos);
    if(cache == nullptr){
        throw std::invalid_argument("Valid place for container doesn t exist");
    }
    checker.applyChecks(*this ,container, pos);
    (*containers).insert(container, pos, cache);
    container->setId(X, Y, Z);
}



//Информация о складе
std::string Storage::getInfoAboutStorage() const{
    std::string result = "Length: " + std::to_string(length) + ", Width: " + std::to_string(width) + ", Height: "
    + std::to_string(height) + ", Temperature: " + std::to_string(temperature);
    return result;
}



bool Storage::isNoTop(const ContainerPosition<int>& position){
    int minX = Octree<int, IContainer*, ContainerPosition<int>>::getMinX(position);
    int maxX = Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(position);
    int minY = Octree<int, IContainer*, ContainerPosition<int>>::getMinY(position);
    int maxY = Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(position);
    int maxZ = Octree<int, IContainer*, ContainerPosition<int>>::getMaxZ(position);
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    lock.lock();
    auto copy = containers->searchDepth();
    lock.unlock();
    
    for(auto i : copy){
        const ContainerPosition<int>& containerPos = i.first;
        if (Octree<int, IContainer*, ContainerPosition<int>>::getMinX(containerPos) <= maxX && Octree<int, IContainer*, ContainerPosition<int>>::getMaxX(containerPos) >= minX &&
            Octree<int, IContainer*, ContainerPosition<int>>::getMinY(containerPos) <= maxY && Octree<int, IContainer*, ContainerPosition<int>>::getMaxY(containerPos) >= minY &&
            Octree<int, IContainer*, ContainerPosition<int>>::getMinZ(containerPos) == maxZ + 1) {
                return false;
        }
    }

    return true;
}


//перемещения
void Storage::moveContainer(std::string id, int X, int Y, int Z){
    if(X < 0 || Y < 0 || Z < 0){
        throw std::invalid_argument("Invalid coordinate");
    }
    auto cache = containers->search(id);
    if(cache == nullptr){
        throw std::invalid_argument("Container does not exist");
    }
    std::pair<ContainerPosition<int>, IContainer*> item;
    for(auto& i : cache->con){//con
        if(i.second->getId() == id){
            item.first = i.first;
            item.second = i.second;
            cache->con.erase(std::remove(cache->con.begin(), cache->con.end(), i), cache->con.end());
            break;
        }
    }
    if(!isNoTop(item.first)){
        containers->insert(item.second, item.first, cache);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z); 
        throw std::invalid_argument("Not a top containerMove");
    }
    try{
        addContainer(item.second, X, Y, Z);
    }catch(std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        containers->insert(item.second, item.first, cache);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        throw std::invalid_argument("Can't move container "); 
    }
}


//Повороты 
void Storage::rotateContainer(std::string id, int method) {
    auto cache = containers->search(id);
    if(cache == nullptr){
        throw std::invalid_argument("Container does not exist1" + id);
    }
    std::pair<ContainerPosition<int>, IContainer*> item;
    for(auto& i : cache->con){
        if(i.second->getId() == id){
            item.first = i.first;
            item.second = i.second;
            cache->con.erase(std::remove(cache->con.begin(), cache->con.end(), i), cache->con.end());
            break;
        }
    }
    IContainer* container = item.second;
    if(container->isType() == "Fragile" || container->isType() == "Fragile and Refraged Container"){
        containers->insert(item.second, item.first, cache);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        throw std::invalid_argument("Fragile container cannot be rotated");
    }
    ContainerPosition<int> pos = item.first;
    if(!isNoTop(pos)){
        containers->insert(item.second, item.first, cache);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        throw std::invalid_argument("No top container");
    }
    int X = pos.LLDown.x;
    int Y = pos.LLDown.y;
    int Z = pos.LLDown.z;
    IContainer *newContainer = container->Clone(0, method);
    try{
        addContainer(newContainer, X, Y, Z);
        delete item.second;
    }catch(std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        containers->insert(item.second, item.first, cache);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        delete newContainer;
        throw std::invalid_argument("Can't rotate container ");
    }
}


void Storage::multitread(IContainer* container, int X, int Y, int Z){
    std::unique_lock<std::mutex> ul(mtx, std::defer_lock);
    ContainerPosition<int> pos = calculateContainerPosition(X, Y, Z, container->getLength(), container->getWidth(), container->getHeight());
    ul.lock();
    auto cache = containers->SearchInsert(container, pos); //Иначе будет data race shared_mutex юзать нет смысла много записи 
    ul.unlock();
    if(cache == nullptr){
        throw std::invalid_argument("Valid place for container doesn t exist");
    }
    checker.applyChecks(*this, container, pos);
    ul.lock();
    if(!containerAdded.load()){
        containerAdded.store(true);
        containers->insert(container, pos, cache);
        container->setId(X, Y, Z);
        return;
    }
    ul.unlock();
    if(containerAdded.load()){
        throw std::invalid_argument("Container already added");
    }
}

bool Storage::addContainerR(IContainer* container, int yStart, int yEnd){
    std::unique_lock<std::mutex> ul(mtx, std::defer_lock);
    for(int y = yStart; y <= yEnd; y++){
        for(int x = 0; x <= length; x++){
            for(int z = 0; z <= height; z++){
                try{
                    if(containerAdded.load()){
                        return true;
                    }
                    multitread(container, x, y, z);
                    return true;
                }catch(std::invalid_argument &e){
                }
            }
        }
    }
    return false;
}

std::string Storage::addContainer(IContainer* container){
    std::vector<std::thread>  threads;
    for (int y = 0; y <= width; y += 20) {
        int yEnd = std::min(y + 20, width);
        threads.emplace_back(&Storage::addContainerR, this, container, y, yEnd);
    }
    for (auto& t : threads) {
        t.join();
    }
    if (!containerAdded.load()) {
        return "_";
    } else {
        containerAdded.store(false);
        return container->getId();
    }
}


void Storage::removeContainer(std::string id){
    auto cache2 = containers->search(id);
    auto cache = containers->findI(id);
    if(cache2 == nullptr){
        throw std::invalid_argument("No container on storage with id " + id);
    }
    std::vector<std::pair<ContainerPosition<int>, IContainer*>> con = searchUpperContainer(cache.first);
    if(con.empty()){
        //Простой случай, если на верху нет 
        containers->remove(id);
    }else{
        //Сложный случай, если на верху есть контейнеры
        std::pair<ContainerPosition<int>, IContainer *> copy_delete = std::make_pair(cache.first, cache.second->Clone());
        std::vector<std::pair<ContainerPosition<int>, IContainer*>> con_copy;
        
        
        //Удаляем контейнеры, делаем их копии, т.к. они в разных узлах дерева


        for(auto& i : con){
            if(i.second != nullptr){
                con_copy.insert(con_copy.begin(), std::make_pair(i.first, i.second->Clone()));
                //con_copy = con;
                containers->remove(i.second->getId());
            }
        }

        containers->remove(id);
        //Пытаемся раскидать контейнеры по новым позициям

        std::vector<std::string> newPlacement;
        std::sort(con_copy.begin(), con_copy.end(), comparePosition);
        for(auto& container : con_copy){
            auto last = container.second->Clone();
            std::string newId = addContainer(last);

            //Возврат, если не получилось раскидать контейнеры
            if(newId == "_"){
                delete last;
                for(auto& return_containerId : newPlacement){
                    containers->remove(return_containerId);
                }
                try{
                    addContainer(copy_delete.second, copy_delete.first.LLDown.x, copy_delete.first.LLDown.y, copy_delete.first.LLDown.z);
                    for(auto& ret : con_copy){
                        addContainer(ret.second, ret.first.LLDown.x, ret.first.LLDown.y, ret.first.LLDown.z);
                    }
                }catch(std::exception& e){//Худший случай
                    std::cerr << "Critical error: " << e.what() << std::endl;
                    bool flag = false;
                    for(auto& ret : con_copy){
                        flag = containers->remove(ret.second->getId());
                        if(flag == false){
                            delete ret.second;
                        }
                    }
                }
                throw std::invalid_argument("No space found to move container with id " + id);
            }
            //Страховка         
            
            else{
                newPlacement.push_back(newId);
            }


        }
        //Удаляем копии контейнеров
        for(auto& i : con_copy){
            delete i.second;
        }
        delete copy_delete.second;
    }
}


std::string Storage::getInfo() const{
    std::string result;
    auto container = containers->searchDepth();
    if(container.empty()){
        return "No containers on storage.";
    }
    for(auto& con : container){
        if(con.second != nullptr){
            result += con.second->getId() + " " + std::to_string(con.second->getLength()) + " x " +
        std::to_string(con.second->getWidth()) + " x " + std::to_string(con.second->getHeight()) + " " +
        con.second->isType() + "\n"; 
        }
    }
    return result;
}


void Storage::howContai(IContainer* container, std::vector<size_t>& result, size_t method){
    size_t count = 0;
    IContainer* currentContainer = container->Clone(0, method);
    Storage st(*this);
    while (true) 
    {
        if (st.addContainer(currentContainer) != "_") {
            result[method]++;
            currentContainer = currentContainer->Clone(result[method], method);
            std::cout << "Added " << method << " " << result[method] << std::endl;
        } else {
            std::cout << "Cannot add more containers for method " << method << ", total added: " << result[method] << "\n";
            break;
        }
    }
    delete currentContainer;
}


 size_t Storage::howContainer(IContainer* container){
    std::vector<size_t> result(6, 0);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 6; ++i) {
        threads.emplace_back(&Storage::howContai, this, container, std::ref(result), i);
    }
    for (auto& t : threads) {
        t.join();
    }
    return std::accumulate(result.begin(), result.end(), result[0], [](int a, int b){return std::max(a,b);});
}


std::pair<ContainerPosition<int>, IContainer*> Storage::find(std::string id){
    auto it = containers->findI(id);
    if(it.second == nullptr){
        throw std::runtime_error("Container not found");
    }
    return it;
}


std::vector<std::string> Storage::getListContainers() const{
    std::vector<std::string> con;
    for(auto it = containers->begin(); it != containers->end(); ++it){
        if (*it == nullptr) {
            std::cout << "Invalid container found.\n";
            continue;
        }
        if(!(*it)->con.empty()){
            for(auto it2 = (*it)->con.begin(); it2 != (*it)->con.end(); ++it2){
                con.push_back((*it2).second->getId());
            }
        }
    }
    return con;
}


void Storage::checkTemperature(Storage& storage, IContainer* container, ContainerPosition<int> pos){
    IRefragedContainer* ref = dynamic_cast<IRefragedContainer*>(container);
    if(container == nullptr){
        throw std::invalid_argument("Container is not refraged");
    }
    if(((*container).isType() == "Refraged" || (*container).isType() == "Fragile and Refraged Container") &&
    storage.temperature > (*ref).getMaxTemperature())
    {
        throw std::invalid_argument("Container is too hot");
    }
}


void Storage::checkPressure(Storage& storage, IContainer* container, ContainerPosition<int> pos){
    if(container == nullptr){
        throw std::invalid_argument("Container is not fragile");
    }
    if(pos.LLDown.z != 0){
        std::vector<std::pair<ContainerPosition<int>,IContainer*>> con = storage.searchUnderContainer(pos);
        if(con.empty() || con[0].first.LLDown.z != 0){
            throw std::invalid_argument("Container can t fly 1");
        }
        if(!checkSupport(pos, con)){
            throw std::invalid_argument("Support doesn t exist");
        }
        for(size_t i = 0; i < con.size(); i++){
            ContainerPosition<int> check = con[i].first;
            if(con[i].second == nullptr){
                continue;
            }
            if(((*con[i].second).isType() == "Fragile" || (*con[i].second).isType() == "Fragile and Refraged Container") && calculatemass(con, i) + (*container).getMass() > (*(dynamic_cast<IFragileContainer*>(con[i].second))).getMaxPressure()){
                throw std::invalid_argument("Container would be too heavy");
            }
        }
    }
}