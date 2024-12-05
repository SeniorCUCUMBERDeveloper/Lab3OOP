#ifndef OCTREE_HPP
#define OCTREE_HPP


#include <vector>
#include <iostream>
#include <algorithm>
#include "ContainerPosition.hpp"
#include <tuple>
#include <stack>
#include <memory>
#include <type_traits>
#include <regex>
#define MAX_ITEMS 4



template<typename T>
concept PointConcept = requires(T a) {
    { a.x };
    { a.y };
    { a.z };
    { a == std::declval<T>() } -> std::same_as<bool>;
    { a < std::declval<T>() } -> std::same_as<bool>;
};


template<typename PointType, typename SizeType>
concept CompatibleCoordinates = requires(PointType p, SizeType s) {
    { p.x + s->getLength() } -> std::convertible_to<decltype(p.x)>;
    { p.x == s->getLength() } -> std::same_as<bool>;
};


template<typename T>
concept HasDimensions = requires(T a) {
    { a->getLength() } -> std::convertible_to<double>;
    { a->getWidth() } -> std::convertible_to<double>;
    { a->getHeight() } -> std::convertible_to<double>;
};


template<typename PointType, typename SizeType>
concept CanAdd = PointConcept<PointType> && 
                 HasDimensions<SizeType> && 
                 CompatibleCoordinates<PointType, SizeType>;


template<typename T>
concept BoundingBoxConcept = PointConcept<Point<T>>;


template <typename T>
requires BoundingBoxConcept<T>
struct BoundingBox{
    Point<T> min, max;
    BoundingBox(const Point<T>& m, const Point<T>& ma) : min(m), max(ma) {}

    bool contains(Point<T>& p) const {
        return min.x < p.x && p.x < max.x && min.y < p.y && p.y < max.y && min.z < p.z && p.z < max.z;
    }

    bool isValid(const T minSize) const {
        T width = max.x - min.x;
        T height = max.y - min.y;
        T depth = max.z - min.z;
        return width > minSize && height > minSize && depth > minSize;
    }

};



template <typename T, typename N>
requires BoundingBoxConcept<T> && CanAdd<Point<T>, N>
class Octree{
    public:
        struct Node{
            std::vector<std::pair<ContainerPosition<T>, N>> con;
            std::array<std::shared_ptr<Node>, 8> children;
            BoundingBox<T> box;
            std::weak_ptr<Node> parent;
            Node(BoundingBox<T> box) : box(box) {}

            bool isLeaf() const {
               return std::all_of(children.begin(), children.end(), [](const std::shared_ptr<Node>& child) { return child == nullptr; });
            }

            std::vector<std::pair<ContainerPosition<T>, N>> getCon(){
                return con;
            }

        };


    class BidirectionalIterator {
        private:
            std::stack<std::shared_ptr<Node>> forwardStack;
            std::stack<std::shared_ptr<Node>> backupStack;

        public:
            using value_type = std::shared_ptr<Node>;
            using pointer = value_type*;
            using reference = value_type&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            explicit BidirectionalIterator(std::shared_ptr<Node> root) {
                if (root != nullptr) {
                    forwardStack.push(root);
                }
            }

            
            reference operator*() {
                if (forwardStack.empty()) {
                    throw std::out_of_range("Stack of iterator's is empty");
                }
                return forwardStack.top();
            }

            
            BidirectionalIterator& operator++() {
                if (forwardStack.empty()){ 
                    return *this;
                }
                auto currentNode = forwardStack.top();
                forwardStack.pop();
                backupStack.push(currentNode);
                for (int i = 7; i >= 0; --i) {
                    if (currentNode->children[i]) {
                        forwardStack.push(currentNode->children[i]);
                    }
                }
                return *this; 
            }

            
            BidirectionalIterator& operator--() {
                if (backupStack.empty()) return *this;
                auto prevNode = backupStack.top();
                backupStack.pop();
                forwardStack.push(prevNode);
                return *this;
            }

            bool operator!=(const BidirectionalIterator& other) const {
                return forwardStack != other.forwardStack || backupStack != other.backupStack;
            }

            bool hasNext() const {
                return !forwardStack.empty();
            }

            bool hasPrev() const {
                return !backupStack.empty();
            }
    };
    private:
        std::shared_ptr<Node> root;
        T MIN_SIZE = 1;
    public:

        Octree(){}
        Octree(BoundingBox<T> bbox) {
            root = std::make_shared<Node>(bbox);
        }


        std::shared_ptr<Node>  getRoot(){
            return root;
        }


       BidirectionalIterator createIterator() const{
            return BidirectionalIterator(root);
        }


        bool checkCollisions(N container, ContainerPosition<T> pos) const {
        BidirectionalIterator it = createIterator();
        if(!root->box.contains(pos.LLDown) || !root->box.contains(pos.LLUp) ||
        !root->box.contains(pos.LRUp) ||  !root->box.contains(pos.LRDown) ||
        !root->box.contains(pos.RLDown) || !root->box.contains(pos.RLUp) ||
        !root->box.contains(pos.RRUp) || !root->box.contains(pos.RRDown)){
            return true;
        }
        while (it.hasNext()) {
            auto currentNode = *it;
            if (checkCollision(container, pos, currentNode->con)) {
                return true;
            }
            ++it;
        }
        return false;
    }


        Octree Clone() const{
            auto clone = Octree(this->root->box);
            return clone;
        }


        bool remove(std::string id){
            bool collision = false;
            std::shared_ptr<Node> copy = nullptr;
            Point<T> point = parsePoint(id);
            removeR(id, root, collision, copy, point);
            if(collision == false){
                return false;
            }
            collision = false;
            Update(copy);
            return true;
        }


        std::pair<ContainerPosition<T>, N> search(std::string id) const{
            Point<T> point = parsePoint(id);
            std::pair<ContainerPosition<T>, N> copyCache;
            bool found = false;
            searchR(id, root, copyCache, point, found);
            if(found == false){
                throw std::invalid_argument("Item not found");
            }
            return copyCache;
        }


        std::vector<std::pair<ContainerPosition<T>, N>> searchDepth() const{
            std::vector<std::pair<ContainerPosition<T>, N>> copyCache(0);
            searchDepth(root, copyCache);
            return copyCache;
        }


        bool SearchInsert(N container, ContainerPosition<T> pos) const{
            bool collision = false;
            std::shared_ptr<Node> copyCache = nullptr;
            std::vector<std::pair<ContainerPosition<T>, N>> copy(0);
            SearchInsert(container, pos, root, copyCache, copy, collision);
            if(copyCache == nullptr){
                return false;
            }
            return true;
        }


        static T getMinX(ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }


        static T getMaxX(ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }


        static T getMinY(ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }


        static T getMaxY(ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }


        static T getMinZ(ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        static T getMaxZ(ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        static T getMinX(const ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }

        static T getMaxX(const ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }


        static T getMinY(const ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }


        static T getMaxY(const ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }



        static T getMinZ(const ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        static T getMaxZ(const ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "ContainerPosition<T> должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        bool push(N container, ContainerPosition<T>& position){
            std::shared_ptr<Node> target = SearchPush(container, position);
            return insert(container, position, target);
        }

        static bool pointincontainer(Point<T>& p, ContainerPosition<T>& position){
            static_assert(PointConcept<Point<T>>, "T должен удовлетворять PointConcept");
            T minX = getMinX(position);
            T maxX = getMaxX(position);
            T minY = getMinY(position);
            T maxY = getMaxY(position);
            T minZ = getMinZ(position);
            T maxZ = getMaxZ(position);
            return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY && p.z >= minZ && p.z <= maxZ);
        }


        private:

        bool insert(N container, ContainerPosition<T>& position, std::shared_ptr<Node> target){
            if(target == nullptr){
                return false;
            }
            target->con.push_back(std::make_pair(position, container));
            if (target->con.empty() == false && target->con.size() > MAX_ITEMS && target->isLeaf()){
                std::cout << "Split\n";
                split(target);
            }
            return true;
        }

        void split(std::shared_ptr<Node> node) {
            if (!node){
                return;
            }
            if (node->children[0] != nullptr){
                return;
            }

            Point<T> min = node->box.min;
            Point<T> max = node->box.max;

            T midX = (min.x + max.x) / 2;
            T midY = (min.y + max.y) / 2;
            T midZ = (min.z + max.z) / 2;

            BoundingBox<T> box0 = BoundingBox<T>(min, Point<T>(midX, midY, midZ));
            BoundingBox<T> box1 = BoundingBox<T>(Point<T>(midX, min.y, min.z), Point<T>(max.x, midY, midZ));
            BoundingBox<T> box2 = BoundingBox<T>(Point<T>(min.x, midY, min.z), Point<T>(midX, max.y, midZ));
            BoundingBox<T> box3 = BoundingBox<T>(Point<T>(midX, midY, min.z), Point<T>(max.x, max.y, midZ));
            BoundingBox<T> box4 = BoundingBox<T>(Point<T>(min.x, min.y, midZ), Point<T>(midX, midY, max.z));
            BoundingBox<T> box5 = BoundingBox<T>(Point<T>(midX, min.y, midZ), Point<T>(max.x, midY, max.z));
            BoundingBox<T> box6 = BoundingBox<T>(Point<T>(min.x, midY, midZ), Point<T>(midX, max.y, max.z));
            BoundingBox<T> box7 = BoundingBox<T>(Point<T>(midX, midY, midZ), max);

            if(!box0.isValid(MIN_SIZE) || !box1.isValid(MIN_SIZE)
            || !box2.isValid(MIN_SIZE) || !box3.isValid(MIN_SIZE)
            ||!box4.isValid(MIN_SIZE) || !box5.isValid(MIN_SIZE)
            ||!box6.isValid(MIN_SIZE) || !box7.isValid(MIN_SIZE)){
                return;
            }

            node->children[0] = std::make_shared<Node>(box0); // 0: мин
            node->children[1] = std::make_shared<Node>(box1); // 1: x+
            node->children[2] = std::make_shared<Node>(box2); // 2: y+
            node->children[3] = std::make_shared<Node>(box3); // 3: xy+
            node->children[4] = std::make_shared<Node>(box4); // 4: z+
            node->children[5] = std::make_shared<Node>(box5); // 5: x+z+
            node->children[6] = std::make_shared<Node>(box6); // 6: y+z+
            node->children[7] = std::make_shared<Node>(box7); // 7: xyz+
            node->children[0]->parent = node;
            node->children[1]->parent = node;
            node->children[2]->parent = node;
            node->children[3]->parent = node;
            node->children[4]->parent = node;
            node->children[5]->parent = node;
            node->children[6]->parent = node;
            node->children[7]->parent = node;
            
            for (auto it = node->con.begin(); it != node->con.end(); ) {
                bool moved = false;

                for (int i = 0; i < 8; ++i) {
                    if (node->children[i]->box.contains(it->first.LLDown) && 
                        node->children[i]->box.contains(it->first.LLUp) &&
                        node->children[i]->box.contains(it->first.LRDown) &&
                        node->children[i]->box.contains(it->first.LRUp) &&
                        node->children[i]->box.contains(it->first.RLDown) &&
                        node->children[i]->box.contains(it->first.RLUp) &&
                        node->children[i]->box.contains(it->first.RRDown) &&
                        node->children[i]->box.contains(it->first.RRUp)) {
                        node->children[i]->con.push_back(*it);
                        moved = true;
                        break;
                    }
                }

                if (moved) {
                    it = node->con.erase(it);
                } else {
                    ++it;
                }
            }
        }



        void  searchUnderOct(std::shared_ptr<Node> node, std::vector<std::pair<ContainerPosition<T>, N>>& vec) const{
            if(node == nullptr){
                return;
            }
            if(!node->con.empty()){
                vec.insert(vec.end(), node->con.begin(), node->con.end());
            }
            for(int i = 0; i < 8; ++i){
                searchUnderOct(node->children[i], vec);
            }
        }


        void searchDepth(std::shared_ptr<Node> node, std::vector<std::pair<ContainerPosition<T>, N>>& copyCache) const{
            if(node == nullptr){
                return;
            }
            if(!node->con.empty()){
                (copyCache).insert((copyCache).end(), node->con.begin(), node->con.end());
            }
            for(int i = 0; i < 8; ++i){
                searchDepth(node->children[i], copyCache);
            }
        }


        bool SearchInsert(N container, ContainerPosition<T> pos, std::shared_ptr<Node> node, std::shared_ptr<Node>& copyCache, std::vector<std::pair<ContainerPosition<T>, N>>& copy, bool& collision) const{
            if(node == nullptr){
                return false;
            }
             if (!node->box.contains(pos.LLDown) || !node->box.contains(pos.LLUp) ||
                !node->box.contains(pos.LRDown) || !node->box.contains(pos.LRUp) ||
                !node->box.contains(pos.RLDown) || !node->box.contains(pos.RLUp) ||
                !node->box.contains(pos.RRDown) || !node->box.contains(pos.RRUp)) {
                return false;
            }
            if(!node->con.empty()){
                (copy).insert((copy).end(), node->con.begin(), node->con.end());
            }
            if (node->isLeaf() == false && collision == false && copyCache == nullptr) {
                for (size_t i = 0; i < 8; ++i) {
                     SearchInsert(container, pos, node->children[i], copyCache, copy, collision);
                }
            }
            if(collision == false && copyCache == nullptr){
                
                searchUnderOct(node, copy);
                
                if(!checkCollision(container, pos, copy)){
                    copyCache = node;
                    return true;
                }else{
                    collision = true;
                    return false;
                }
            }else if(collision == false && copyCache != nullptr){
                return true;
            }
            return false;
        }



        std::shared_ptr<Node> SearchPush(N container, ContainerPosition<T> pos) const{
            bool collision = false;
            std::shared_ptr<Node> copyCache = nullptr;
            std::vector<std::pair<ContainerPosition<T>, N>> copy(0);
            SearchInsert(container, pos, root, copyCache, copy, collision);
            return copyCache;
        }


            void searchR(std::string id, std::shared_ptr<Node> node, std::pair<ContainerPosition<T>, N>& copyCache, Point<T> point, bool& flag)const{
                if(node == nullptr){
                    return;
                }
                if(!node->box.contains(point)){
                    return;
                }
                if(node->con.empty() == false){
                    for(auto& i : node->con){
                        if(i.first.LLDown.x != -1  && number(i.first.LLDown) == id){
                            flag = true;
                            copyCache = std::make_pair(i.first, i.second);
                            return;
                        }
                    }
                }
                if(node->isLeaf() == false){
                    for(int i = 0; i < 8; ++i){
                        searchR(id, node->children[i], copyCache, point, flag);
                    }
                }
            }


            void removeR(std::string id, std::shared_ptr<Node> node, bool& collision, std::shared_ptr<Node>& copyCache, Point<T> point){
                if(node == nullptr){
                    return;
                }
                if(collision == true || node == nullptr){
                    return;
                }
                if(!node->box.contains(point)){
                    return;
                }
                if(node->con.empty() == false){
                    for(auto& i : node->con){
                        if(i.first.LLDown.x != -1  && number(i.first.LLDown) == id){
                            if(i.second != nullptr){
                                //i.second.reset();
                                node->con.erase(std::remove(node->con.begin(), node->con.end(), i), node->con.end());
                            }
                            collision = true;
                            copyCache = node;
                            return;
                        }
                    }
                }
                if(node->isLeaf() == false){
                    for(int i = 0; i < 8; ++i){
                        removeR(id, node->children[i], collision, copyCache, point);
                    }
                }

            }


            bool checkEmptyNode(std::shared_ptr<Node> node) const{
            if (node == nullptr) return true;
            for (int i = 0; i < 8; ++i) {
                if (node->children[i] != nullptr && !node->children[i]->con.empty()) {
                    return false;
                }
            }
            return true;
        }

           void mearge(std::shared_ptr<Node> node) {
                if (node == nullptr || node->isLeaf()) return;
                for (int i = 0; i < 8; ++i) {
                    if (node->children[i] != nullptr && !node->children[i]->con.empty()) {
                        node->con.insert(node->con.end(), node->children[i]->con.begin(), node->children[i]->con.end());
                        node->children[i]->con.clear();
                    }
                }
                for (int i = 0; i < 8; ++i) {
                    mearge(node->children[i]);
                }
            }

            void decreaseHightTree(std::shared_ptr<Node> node) {
                if (node == nullptr || node->isLeaf()) {
                    return;
                }
                for (int i = 0; i < 8; ++i) {
                    decreaseHightTree(node->children[i]);
                }
                if (checkEmptyNode(node)) {
                    for (int i = 0; i < 8; ++i) {
                        if (node->children[i] != nullptr && node->children[i]->isLeaf()) { 
                            node->children[i].reset();
                            node->children[i] = nullptr;
                        }
                    }
                }
            }


            void Update(std::shared_ptr<Node> node) {
                 auto parentNode = node->parent.lock(); 
                if(node == root && node->isLeaf() == false && node->con.empty()){
                    mearge(node);
                    decreaseHightTree(node);
                    return;
                }
                if(checkEmptyNode(parentNode)){
                    mearge(parentNode);
                    decreaseHightTree(parentNode);
                }
            }


            bool ContainEntity(ContainerPosition<T> pos1, ContainerPosition<T> pos2) const {
                
                int pos1MinX = getMinX(pos1);
                int pos1MaxX = getMaxX(pos1);
                int pos1MinY = getMinY(pos1);
                int pos1MaxY = getMaxY(pos1);
                int pos1MinZ = getMinZ(pos1);
                int pos1MaxZ = getMaxZ(pos1);
                

                int pos2MinX = getMinX(pos2);
                int pos2MaxX = getMaxX(pos2);
                int pos2MinY = getMinY(pos2);
                int pos2MaxY = getMaxY(pos2);
                int pos2MinZ = getMinZ(pos2);
                int pos2MaxZ = getMaxZ(pos2);


                return (pos1MinX <= pos2MaxX && pos1MaxX >= pos2MinX) &&
                    (pos1MinY <= pos2MaxY && pos1MaxY >= pos2MinY) &&
                    (pos1MinZ <= pos2MaxZ && pos1MaxZ >= pos2MinZ);
            }


            bool checkCollision(N container, ContainerPosition<T> pos, std::vector<std::pair<ContainerPosition<T>, N>> copy) const {
                for(auto it : copy){
                    if(ContainEntity(it.first, pos)){
                        return true;
                    }
                }
                return false; 
            }


            static std::string number(const Point<T>& p) {
                return std::to_string(p.x) + "_" + std::to_string(p.y) + "_" + std::to_string(p.z);
            }


            static Point<T> parsePoint(std::string str){
                static_assert(PointConcept<Point<T>>, "Point должен удовлетворять PointConcept");
                Point<T> point;
                std::regex pattern(R"((\d+|\d+\.\d+)_(\d+|\d+\.\d+)_(\d+|\d+\.\d+))");
                std::smatch match;
                std::regex_match(str, match, pattern);
                point.x = static_cast<T>(std::stod(match[1].str()));
                point.y = static_cast<T>(std::stod(match[2].str()));
                point.z = static_cast<T>(std::stod(match[3].str()));
                return point;
            }
                
};


#endif