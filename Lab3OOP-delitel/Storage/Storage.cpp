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


  void Storage::addExternalCheckFunction(const std::function<void(Storage&, std::shared_ptr<IContainer>, ContainerPosition<Point<int>>)>& externalFunc) {
        checker.addCheckFunction(externalFunc);
    }


Storage::Storage(int number, int length, int width, int height, double temperature) : checker(){
    this->number = number;
    this->length = length;
    this->width = width;
    this->height = height;
    this->temperature = temperature;
    BoundingBox<Point<int>> bound(Point<int>(0, 0, 0), Point<int>(length, width, height));
    this->containers = Octree<Point<int>, std::shared_ptr<IContainer>>(bound);
    this->checker.addCheckFunction([this](Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> position) {
        checkTemperature(storage, container, position);
    });
    this->checker.addCheckFunction([this](Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> position) {
        checkPressure(storage, container, position);
    });
}


bool Storage::comparePosition(std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos1, std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos2){
    return pos1.first.LLDown.z < pos2.first.LLDown.z;
}


bool Storage::comparePositionReverse(std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos1, std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>& pos2){
    return pos1.first.LLDown.z > pos2.first.LLDown.z;
}


Storage& Storage::operator=(const Storage& other) {
        if (this != &other) { 
            number = other.number;
            length = other.length;
            width = other.width;
            height = other.height;
            temperature = other.temperature;
            containers = (other).containers.Clone();
            auto i = other.containers.searchDepth();
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
        containers = other.containers.Clone(); // Используем интерфейс Clone для копирования
        auto i = other.containers.searchDepth();
        std::sort(i.begin(), i.end(), comparePosition);
        for(auto it : i){
            std::cout << it.second->getId() << std::endl;
        }
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
    auto i = this->containers.searchDepth();
    std::sort(i.begin(), i.end(), comparePosition);
    for(auto& it : i){
        std::cout << it.second->getId() << std::endl;
    }
    for(auto& it : i){
        newStorage.addContainer(it.second->Clone(), it.first.LLDown.x, it.first.LLDown.y, it.first.LLDown.z);
    }
    *this = newStorage;
}


//Вставка


std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> Storage::searchUnderContainer(ContainerPosition<Point<int>>& position){
    std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> result;
    int minX = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(position);
    int maxX = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(position);
    int minY = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(position);
    int maxY = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(position);
    int minZ = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinZ(position);
    std::vector<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> current;
    {
    std::shared_lock<std::shared_mutex> lock(smtx);
    current = containers.searchDepth();
    }
    for (const auto& container : current) {
        const ContainerPosition<Point<int>>& containerPos = container.first;

        if (Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(containerPos) <= maxX && Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(containerPos) >= minX &&
            Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(containerPos) <= maxY && Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(containerPos) >= minY &&
            Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxZ(containerPos) < minZ) {
                result.push_back(container);
                continue;
        }
    }
    std::sort(result.begin(), result.end(), comparePosition);
    return result;
}


std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> Storage::searchUpperContainer(ContainerPosition<Point<int>>& position){
    std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> result;
    int minX = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(position);
    int maxX = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(position);
    int minY = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(position);
    int maxY = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(position);
    int maxZ = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxZ(position);
    auto current = containers.searchDepth();
    for (const auto& container : current) {
        const ContainerPosition<Point<int>>& containerPos = container.first;
        
        if (Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(containerPos) <= maxX && Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(containerPos) >= minX &&
            Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(containerPos) <= maxY && Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(containerPos) >= minY &&
            Octree<Point<int>, std::shared_ptr<IContainer>>::getMinZ(containerPos) > maxZ) {
                result.push_back(container);
                continue;
        }
    }
    std::sort(result.begin(), result.end(), comparePositionReverse);
    return result;
}



double Storage::calculatemass(std::vector<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> con, size_t it){
    double mass = 0;
    for(size_t i = it + 1; i < con.size(); i++){
        if((con[i].second)!= nullptr){
            mass += (*con[i].second).getMass();
        }
    }
    return mass;
}


bool Storage::checkSupport(ContainerPosition<Point<int>>& position, std::vector<std::pair<ContainerPosition<Point<int>>,std::shared_ptr<IContainer>>> con){
    bool firstSupport = false;
    bool secondSupport = false;
    Point<int> first = Point<int>(position.LLDown.x, position.LLDown.y + (Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(position) - Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(position)) / 2, position.LLDown.z - 1);
    Point<int> second = Point<int>(position.LRDown.x, position.LRDown.y + (Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(position) - Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(position)) / 2, position.LLDown.z - 1);
    Point<int> middle = Point<int>(position.LLDown.x + (Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(position) - Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(position)) / 2, position.LLDown.y + (Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(position) - Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(position)) / 2, position.LLDown.z - 1);

    for(auto container = con.rbegin(); container != con.rend(); ++container){
        if(firstSupport == true && secondSupport == true){
            return true;
        }
        if((*container).first.LLUp.z + 1 != position.LLDown.z){
            return false;
        }
        if(Octree<Point<int>, std::shared_ptr<IContainer>>::pointincontainer(first, (*container).first)){
            firstSupport = true;
        }
        if(Octree<Point<int>, std::shared_ptr<IContainer>>::pointincontainer(second, (*container).first)){
            secondSupport = true;
        }
        if(Octree<Point<int>, std::shared_ptr<IContainer>>::pointincontainer(middle, (*container).first)){
            return true;
        }
    }
    return false;
}

void Storage::addContainer(std::shared_ptr<IContainer> container, int X, int Y, int Z){
    if(X < 1 || Y < 1 || Z < 1){
        throw std::invalid_argument("Coordinates should be positive");
    }
    ContainerPosition<Point<int>> pos = Octree<Point<int>, std::shared_ptr<IContainer>>::calculateContainerPosition(X, Y, Z, container);
    auto cache = containers.SearchInsert(container, Point<int>(X, Y, Z));
    if(cache == false){
        throw std::invalid_argument("Valid place for container doesn t exist");
    }
    checker.applyChecks(*this ,container, pos);
    (containers).push(container, Point<int>(X, Y, Z));
    container->setId(X, Y, Z);
}



//Информация о складе
std::string Storage::getInfoAboutStorage() const{
    std::string result = "Length: " + std::to_string(length) + ", Width: " + std::to_string(width) + ", Height: "
    + std::to_string(height) + ", Temperature: " + std::to_string(temperature);
    return result;
}



bool Storage::isNoTop(const ContainerPosition<Point<int>>& position){
    int minX = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(position);
    int maxX = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(position);
    int minY = Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(position);
    int maxY = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(position);
    int maxZ = Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxZ(position);
    auto copy = containers.searchDepth();
    
    for(auto i : copy){
        const ContainerPosition<Point<int>>& containerPos = i.first;
        if (Octree<Point<int>, std::shared_ptr<IContainer>>::getMinX(containerPos) <= maxX && Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxX(containerPos) >= minX &&
            Octree<Point<int>, std::shared_ptr<IContainer>>::getMinY(containerPos) <= maxY && Octree<Point<int>, std::shared_ptr<IContainer>>::getMaxY(containerPos) >= minY &&
            Octree<Point<int>, std::shared_ptr<IContainer>>::getMinZ(containerPos) == maxZ + 1) {
                return false;
        }
    }

    return true;
}


//перемещения
void Storage::moveContainer(std::string identification, int X, int Y, int Z){
    if(X < 1 || Y < 1 || Z < 1){
        throw std::invalid_argument("Invalid coordinate");
    }
    Point<int> id = parsePoint(identification);
    auto item = containers.search(id);
    containers.remove(id);
    if(!isNoTop(item.first)){
        containers.push(item.second, item.first.LLDown);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z); 
        throw std::invalid_argument("Not a top containerMove");
    }
    try{
        addContainer(item.second, X, Y, Z);
    }catch(std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        containers.push(item.second, item.first.LLDown);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        throw std::invalid_argument("Can't move container "); 
    }
}


//Повороты 
void Storage::rotateContainer(std::string identification, int method) {
    Point<int> id = parsePoint(identification);
    auto item = containers.search(id);
    containers.remove(id);
    std::shared_ptr<IContainer> container = item.second;
    if(container->isType() == "Fragile" || container->isType() == "Fragile and Refraged Container"){
        containers.push(item.second, item.first.LLDown);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        throw std::invalid_argument("Fragile container cannot be rotated");
    }
    ContainerPosition<Point<int>> pos = item.first;
    if(!isNoTop(pos)){
        containers.push(item.second, item.first.LLDown);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        throw std::invalid_argument("No top container");
    }
    int X = pos.LLDown.x;
    int Y = pos.LLDown.y;
    int Z = pos.LLDown.z;
    std::shared_ptr<IContainer> newContainer = container->Clone(0, method);
    try{
        addContainer(newContainer, X, Y, Z);
        item.second.reset();
    }catch(std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        containers.push(item.second, item.first.LLDown);
        item.second->setId(item.first.LLDown.x, item.first.LLDown.y, item.first.LLDown.z);
        newContainer.reset();
        throw std::invalid_argument("Can't rotate container ");
    }
}


Point<int> Storage::multitread(std::shared_ptr<IContainer> container, int X, int Y, int Z){
    ContainerPosition<Point<int>> pos = Octree<Point<int>, std::shared_ptr<IContainer>>::calculateContainerPosition(X, Y, Z, container);
    {
        std::shared_lock<std::shared_mutex> lock(smtx);    
        auto cache = containers.checkCollisions(container, Point(X, Y, Z));
        if(cache == true){
            throw std::invalid_argument("Valid place for container doesn t exist");
        }
    }

    checker.applyChecks(*this, container, pos);

    if(containerAdded.load()){
        throw std::invalid_argument("Container already added");
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        if(!containerAdded.load()){
            containerAdded.store(true);
            return Point<int>{X, Y, Z};
        }
    }

    throw std::invalid_argument("Container already added");
}

bool Storage::addContainerR(std::shared_ptr<IContainer> container, int yStart, int yEnd, Point<int>& point){
    for(int y = yStart; y <= yEnd; y++){
        for(int x = 1; x <= length; x++){
            for(int z = 1; z <= height; z++){
                try{
                    if(containerAdded.load()){
                        return true;
                    }
                    Point<int> localPoint;
                    localPoint = multitread(container, x, y, z);
                    point = localPoint;
                    return true;
                }catch(std::invalid_argument &e){
                }
            }
        }
    }
    return false;
}

std::string Storage::addContainer(std::shared_ptr<IContainer> container){
    std::vector<std::thread>  threads;
    Point<int> point;
    for (int y = 1; y <= width; y += 20) {
        int yEnd = std::min(y + 20, width);
        threads.emplace_back(&Storage::addContainerR, this, container, y, yEnd, std::ref(point));
    }
    for (auto& t : threads) {
        t.join();
    }
    if (!containerAdded.load()) {
        return "_";
    } else {
        containerAdded.store(false);
        addContainer(container, point.x, point.y, point.z);
        return container->getId();
    }
}


void Storage::removeContainer(std::string identification){
    auto cache = find(identification);
    Point<int> id = parsePoint(identification);
    std::vector<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> con = searchUpperContainer(cache.first);
    if(con.empty()){
        //Простой случай, если на верху нет 
        containers.remove(id);
    }else{
        //Сложный случай, если на верху есть контейнеры
        std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>> copy_delete = std::make_pair(cache.first, cache.second->Clone());
        std::list<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> con_copy;
        std::list<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> lst;
        lst.insert(lst.end(), con.begin(), con.end());
        lst = searchAllContainersUpper(lst);
        
        //Удаляем контейнеры, делаем их копии, т.к. они в разных узлах дерева


        for(auto& i : lst){
            if(i.second != nullptr){
                con_copy.insert(con_copy.begin(), std::make_pair(i.first, i.second->Clone()));
                bool flag = containers.remove(i.first.LLDown);
            }
        }

        containers.remove(id);
        //Пытаемся раскидать контейнеры по новым позициям

        std::vector<std::string> newPlacement;
        for(auto& container : con_copy){
            auto last = container.second->Clone();
            std::string newId = addContainer(last);

            //Возврат, если не получилось раскидать контейнеры
            if(newId == "_"){
                last.reset();
                for(auto& return_containerId : newPlacement){
                    containers.remove(parsePoint(return_containerId));
                }
                    addContainer(copy_delete.second, copy_delete.first.LLDown.x, copy_delete.first.LLDown.y, copy_delete.first.LLDown.z);
                    for(auto& ret : con_copy){
                        addContainer(ret.second, ret.first.LLDown.x, ret.first.LLDown.y, ret.first.LLDown.z);
                    }
                throw std::invalid_argument("No space found to move container with id " + numeric(id));
            }
            //Страховка         
            
            else{
                newPlacement.push_back(newId);
            }


        }
        //Удаляем копии контейнеров
        for(auto& i : con_copy){
            i.second.reset();
        }
        copy_delete.second.reset();
    }
}


std::string Storage::getInfo() const{
    std::string result;
    auto container = containers.searchDepth();
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


void Storage::howContai(std::shared_ptr<IContainer> container, std::vector<size_t>& result, size_t method){
    size_t count = 0;
    std::shared_ptr<IContainer> currentContainer = container->Clone(0, method);
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
    currentContainer.reset();
}


 size_t Storage::howContainer(std::shared_ptr<IContainer> container){
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


std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>> Storage::find(std::string identification){
    Point<int> id = parsePoint(identification);
    return containers.search(id);
}


std::vector<std::string> Storage::getListContainers() const{
    std::vector<std::string> con;
    auto iterator = containers.createIterator();
    while(iterator.hasNext()){
        if((*iterator) != nullptr && !(*iterator)->con.empty()){
            for(auto it2 = (*iterator)->con.begin(); it2 != (*iterator)->con.end(); ++it2){
                con.push_back((*it2).second->getId());
            }
        }
        ++iterator;
    }
    return con;
}


void Storage::checkTemperature(Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> pos){
    auto ref = std::dynamic_pointer_cast<IRefragedContainer>(container);
    if(ref == nullptr){
        return;
    }
    if(container == nullptr){
        throw std::invalid_argument("Container is not refraged");
    }
    if(((*container).isType() == "Refraged" || (*container).isType() == "Fragile and Refraged Container") &&
    storage.temperature > (*ref).getMaxTemperature())
    {
        throw std::invalid_argument("Container is too hot");
    }
}


void Storage::checkPressure(Storage& storage, std::shared_ptr<IContainer> container, ContainerPosition<Point<int>> pos){
    if(container == nullptr){
        throw std::invalid_argument("Container is not fragile");
    }
    if(pos.LLDown.z != 1){
        std::vector<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> con = storage.searchUnderContainer(pos);
        if(con.empty() || con[0].first.LLDown.z != 1){
            throw std::invalid_argument("Container can t fly 1");
        }
        if(!checkSupport(pos, con)){
            throw std::invalid_argument("Support doesn t exist");
        }
        for(size_t i = 0; i < con.size(); i++){
            ContainerPosition<Point<int>> check = con[i].first;
            if(con[i].second == nullptr){
                continue;
            }
            auto fragileContainer = std::dynamic_pointer_cast<IFragileContainer>(con[i].second);
            if(fragileContainer == nullptr){
                continue;
            }
            if((fragileContainer->isType() == "Fragile" || fragileContainer->isType() == "Fragile and Refraged Container") && calculatemass(con, i) + (container)->getMass() > fragileContainer->getMaxPressure()){
                throw std::invalid_argument("Container would be too heavy");
            }
        }
    }
}


std::list<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> Storage::searchAllContainersUpper(std::list<std::pair<ContainerPosition<Point<int>>, std::shared_ptr<IContainer>>> lst){
    for(auto it = lst.begin(); it != lst.end(); it++){
        auto i = searchUpperContainer(it->first);
        if(!i.empty()){
            for(auto it2 = i.begin(); it2 != i.end(); it2++){
                if(std::find(lst.begin(), lst.end(), *it2) == lst.end()){
                    lst.push_back(*it2);
                }
            }
        }
    }
    return lst;
}


Point<int> Storage::parsePoint(const std::string& str) {
    std::regex pattern(R"((\d+|\d+\.\d+)_(\d+|\d+\.\d+)_(\d+|\d+\.\d+))");
    std::smatch match;
    if (std::regex_match(str, match, pattern)) {
        Point<int> point;
        point.x = static_cast<int>(std::stod(match[1].str()));
        point.y = static_cast<int>(std::stod(match[2].str()));
        point.z = static_cast<int>(std::stod(match[3].str()));
        return point;
    } else {
        throw std::invalid_argument("Invalid format for point string");
    }
}


std::string Storage::numeric(const Point<int>& p) {
    return std::to_string(p.x) + "_" + std::to_string(p.y) + "_" + std::to_string(p.z);
}