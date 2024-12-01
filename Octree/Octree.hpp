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


template<typename T>
concept NumericConcept = std::is_arithmetic_v<T> || std::convertible_to<T, double>;


template <typename T>
concept SmartPointerConcept = std::is_base_of<std::shared_ptr<typename T::element_type>, T>::value || std::is_base_of<std::unique_ptr<typename T::element_type>, T>::value;


template<typename T>
concept PointConcept = requires(T a) {
    { a.x } -> NumericConcept;
    { a.y } -> NumericConcept;
    { a.z } -> NumericConcept;
    
    { a == std::declval<T>() } -> std::same_as<bool>;
    { a < std::declval<T>() } -> std::same_as<bool>;
};


template<typename T>
concept ContainerPositionConcept = requires(T c) {
    { c.LLDown } -> PointConcept;
    { c.LLUp } -> PointConcept;
    { c.LRDown } -> PointConcept;
    { c.LRUp } -> PointConcept;
    { c.RRDown } -> PointConcept;
    { c.RRUp } -> PointConcept;
    { c.RLDown } -> PointConcept;
    { c.RLUp } -> PointConcept;

    { c == std::declval<T>() } -> std::same_as<bool>;
    { c < std::declval<T>() } -> std::same_as<bool>;
};


template <typename N>
concept ContainerConcept = SmartPointerConcept<N>;


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

    bool intersects(BoundingBox& other) const {
        return (max.x >= other.min.x && min.x <= other.max.x) &&
                (max.y >= other.min.y && min.y <= other.max.y) &&
                (max.z >= other.min.z && min.z <= other.max.z);
    }
};



template <typename T, typename N, typename CPosType>
requires ContainerConcept<N> && ContainerPositionConcept<CPosType> && BoundingBoxConcept<T>
class Octree{
    public:
        struct Node{
            std::vector<std::pair<CPosType, N>> con;
            std::array<std::shared_ptr<Node>, 8> children;
            BoundingBox<T> box;
            std::weak_ptr<Node> parent;
            int height;
            Node(BoundingBox<T> box, int height) : box(box), height(height) {}

            bool isLeaf() const {
               return std::all_of(children.begin(), children.end(), [](const std::shared_ptr<Node>& child) { return child == nullptr; });
            }

            std::vector<std::pair<CPosType, N>> getCon(){
                return con;
            }

        };


        class iterator {
            private:
                std::stack<std::shared_ptr<Node>> nodeStack;

            public:
                using value_type = std::shared_ptr<Node>;
                using pointer = value_type*;
                using reference = value_type&;
                using difference_type = std::ptrdiff_t;
                using iterator_category = std::forward_iterator_tag;

            public:

                explicit iterator(std::shared_ptr<Node>  root) {
                    if (root != nullptr) {
                        nodeStack.push(root);
                    }
                }

                reference operator*() {
                    return nodeStack.top();
                }


                iterator& operator++() {
                    if (nodeStack.empty()) return *this;
                    std::shared_ptr<Node> current = nodeStack.top();
                    nodeStack.pop();
                    for (int i = 7; i >= 0; --i) {
                        if (current->children[i]) {
                            nodeStack.push(current->children[i]);
                        }
                    }
                    return *this;
                }

                
                bool operator!=(const iterator& other) const {
                    return nodeStack != other.nodeStack;
                }
        };

    private:
        std::shared_ptr<Node> root;
        int depth_;
    public:
        Octree(BoundingBox<T> bbox, int depth) {
        depth_ = depth;
        root = std::make_shared<Node>(bbox, 0);
    }


        std::shared_ptr<Node>  getRoot(){
            return root;
        }


        iterator begin() {
            return iterator(root);
        }

        iterator end() {
            return iterator(nullptr);
        }


        bool insert(N container, CPosType& position, std::shared_ptr<Node> target){
            if(target == nullptr){
                return false;
            }
            target->con.push_back(std::make_pair(position, container));
            if (target->con.empty() == false && target->con.size() > 4 && target->isLeaf() && target->height < depth_){
                std::cout << "Split\n";
                split(target);
            }
            return true;
        }


       void split(std::shared_ptr<Node> node) {
    if (!node) return;  // Проверка на nullptr
    if (node->children[0] != nullptr) return;  // Если дочерние узлы уже созданы, ничего не делаем

    Point<T> min = node->box.min;
    Point<T> max = node->box.max;

    T midX = (min.x + max.x) / 2;
    T midY = (min.y + max.y) / 2;
    T midZ = (min.z + max.z) / 2;

    // Создание детей с их соответствующими границами
    node->children[0] = std::make_shared<Node>(BoundingBox<T>(min, Point<T>(midX, midY, midZ)), node->height + 1); // 0: мин
    node->children[1] = std::make_shared<Node>(BoundingBox<T>(Point<T>(midX, min.y, min.z), Point<T>(max.x, midY, midZ)), node->height + 1); // 1: x+
    node->children[2] = std::make_shared<Node>(BoundingBox<T>(Point<T>(min.x, midY, min.z), Point<T>(midX, max.y, midZ)), node->height + 1); // 2: y+
    node->children[3] = std::make_shared<Node>(BoundingBox<T>(Point<T>(midX, midY, min.z), Point<T>(max.x, max.y, midZ)), node->height + 1); // 3: xy+
    node->children[4] = std::make_shared<Node>(BoundingBox<T>(Point<T>(min.x, min.y, midZ), Point<T>(midX, midY, max.z)), node->height + 1); // 4: z+
    node->children[5] = std::make_shared<Node>(BoundingBox<T>(Point<T>(midX, min.y, midZ), Point<T>(max.x, midY, max.z)), node->height + 1); // 5: x+z+
    node->children[6] = std::make_shared<Node>(BoundingBox<T>(Point<T>(min.x, midY, midZ), Point<T>(midX, max.y, max.z)), node->height + 1); // 6: y+z+
    node->children[7] = std::make_shared<Node>(BoundingBox<T>(Point<T>(midX, midY, midZ), max), node->height + 1); // 7: xyz+
    node->children[0]->parent = node;
    node->children[1]->parent = node;
    node->children[2]->parent = node;
    node->children[3]->parent = node;
    node->children[4]->parent = node;
    node->children[5]->parent = node;
    node->children[6]->parent = node;
    node->children[7]->parent = node;
    // Перемещение объектов из текущего узла в дочерние
    for (auto it = node->con.begin(); it != node->con.end(); ) {
        bool moved = false;  // Флаг для отслеживания перемещения

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
                moved = true; // Успех перемещения
                break; // Если успешно переместили, выходим из цикла
            }
        }

        if (moved) {
            it = node->con.erase(it); // Удаляем элемент и перемещаем итератор
        } else {
            ++it; // Если не переместили, просто увеличиваем итератор
        }
    }
}

        std::shared_ptr<Octree> Clone(){
            auto clone = std::make_shared<Octree>(this->root->box, this->depth_);
            return clone;
        }



        bool remove(std::string id){
            bool collision = false;
            std::shared_ptr<Node> copy = nullptr;
            removeR(id, root, collision, copy);
            if(collision == false){
                return false;
            }
            collision = false;
            Update(copy);
            return true;
        }


        std::shared_ptr<Node> search(std::string id){
            std::shared_ptr<Node> copyCache = nullptr;
            searchR(id, root, copyCache);
            return copyCache;
        }


        std::pair<CPosType, N> findI(std::string id){
            std::pair<CPosType, N> it = std::make_pair(CPosType(), nullptr);
            findI(id, root, &it);
            return it;
        }


        std::vector<std::pair<CPosType, N>> searchDepth(){
            std::vector<std::pair<CPosType, N>> copyCache(0);
            searchDepth(root, copyCache);
            return copyCache;
        }


        std::shared_ptr<Node> SearchInsert(N container, CPosType pos){
            bool collision = false;
            std::shared_ptr<Node> copyCache = nullptr;
            std::vector<std::pair<CPosType, N>> copy(0);
            SearchInsert(container, pos, root, copyCache, copy, collision);
            return copyCache;
        }


        void print() const {
            printNode(root, 0);
        }


        static T getMinX(CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }


        static T getMaxX(CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }


        static T getMinY(CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }


        static T getMaxY(CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }


        static T getMinZ(CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        static T getMaxZ(CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        static T getMinX(const CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }

        static T getMaxX(const CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
        }


        static T getMinY(const CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }


        static T getMaxY(const CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
        }



        static T getMinZ(const CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::min({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        static T getMaxZ(const CPosType& position){
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            return std::max({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
        }


        bool push(N container, CPosType& position){
            std::shared_ptr<Node> target = SearchInsert(container, position);
            return insert(container, position, target);
        }

        static bool pointincontainer(Point<T>& p, CPosType& position){
            static_assert(PointConcept<Point<T>>, "T должен удовлетворять PointConcept");
            static_assert(ContainerPositionConcept<CPosType>, "CPosType должен удовлетворять ContainerPositionConcept");
            T minX = getMinX(position);
            T maxX = getMaxX(position);
            T minY = getMinY(position);
            T maxY = getMaxY(position);
            T minZ = getMinZ(position);
            T maxZ = getMaxZ(position);
            return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY && p.z >= minZ && p.z <= maxZ);
        }


        private:


        void  searchUnderOct(std::shared_ptr<Node> node, std::vector<std::pair<CPosType, N>>& vec){
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


        void findI(std::string id, std::shared_ptr<Node> node, std::pair<CPosType, N>* it){
            if(node == nullptr){
                return;
            }
            if(node->con.empty() == false){
                for(auto& i : node->con){
                    if(i.first.LLDown.x != -1 && i.second != nullptr && number(i.first.LLDown) == id){
                        *it = i;
                        return;
                    }
                }
            }
            if(node->isLeaf() == false){
                for(int i = 0; i < 8; ++i){
                    findI(id, node->children[i], it);
                }
            }
        }


        void searchDepth(std::shared_ptr<Node> node, std::vector<std::pair<CPosType, N>>& copyCache){
            if(node == nullptr){
                return;
            }
            if(!node->con.empty()){//
                (copyCache).insert((copyCache).end(), node->con.begin(), node->con.end());
            }
            for(int i = 0; i < 8; ++i){
                searchDepth(node->children[i], copyCache);
            }
        }


        bool SearchInsert(N container, CPosType pos, std::shared_ptr<Node> node, std::shared_ptr<Node>& copyCache, std::vector<std::pair<CPosType, N>>& copy, bool& collision){
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


            void searchR(std::string id, std::shared_ptr<Node> node, std::shared_ptr<Node>& copyCache){
                if(node == nullptr){
                    return;
                }
                if(node->con.empty() == false){
                for(auto& i : node->con){
                    if(i.first.LLDown.x != -1 && i.second != nullptr && number(i.first.LLDown) == id){
                        copyCache = node;
                        return;
                    }
                }
            }
            if(node->isLeaf() == false){
                for(int i = 0; i < 8; ++i){
                    searchR(id, node->children[i], copyCache);
                }
            }
        }


            void removeR(std::string id, std::shared_ptr<Node> node, bool& collision, std::shared_ptr<Node>& copyCache){
                if(node == nullptr){
                    return;
                }
                if(collision == true || node == nullptr){
                    return;
                }
                if(node->con.empty() == false){
                    for(auto& i : node->con){
                        if(i.first.LLDown.x != -1 && i.second != nullptr && number(i.first.LLDown) == id){
                            if(i.second != nullptr){
                                i.second.reset();
                                //i.second = nullptr;
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
                        removeR(id, node->children[i], collision, copyCache);
                    }
                }

            }


            bool checkEmptyNode(std::shared_ptr<Node> node) {
            if (node == nullptr) return true; // Добавлена проверка на nullptr
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


            bool ContainEntity(CPosType pos1, CPosType pos2) const {
                
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


            bool checkCollision(N container, CPosType pos, std::vector<std::pair<CPosType, N>> copy) const {
                for(auto it : copy){
                    if(ContainEntity(it.first, pos)){
                        return true;
                    }
                }
                return false; 
            }


            void printNode(std::shared_ptr<Node> node, int depth) const {
                if (!node) return;

                std::cout << std::string(depth, '-') << "Node at depth " << depth 
                        << ": " << node->box.min.x << ", " << node->box.min.y << ", " << node->box.min.z 
                        << " to " << node->box.max.x << ", " << node->box.max.y << ", " << node->box.max.z << "\n";
                for (auto& child : node->children) {
                    printNode(child, depth + 1);
                }
            }

            std::string number(const Point<T>& p) const{
            return std::to_string(p.x) + "_" + std::to_string(p.y) + "_" + std::to_string(p.z);
        }
                
};


#endif