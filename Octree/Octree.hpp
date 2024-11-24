#ifndef OCTREE_HPP
#define OCTREE_HPP


#include <vector>
#include <iostream>
#include <algorithm>
#include "CoordinateEntity.hpp"
#include <tuple>


template <typename T>
struct BoundingBox{
    Point<T> min, max;
    BoundingBox(const Point<T>& m, const Point<T>& ma) : min(m), max(ma) {}

    bool contains(Point<T>& p) const {
        return min.x <= p.x && p.x <= max.x && min.y <= p.y && p.y <= max.y && min.z <= p.z && p.z <= max.z;
    }

    bool intersects(BoundingBox& other) const {
        return (max.x >= other.min.x && min.x <= other.max.x) &&
                (max.y >= other.min.y && min.y <= other.max.y) &&
                (max.z >= other.min.z && min.z <= other.max.z);
    }
};
template <typename T, typename N, typename CPosType>
class Octree{
    public:
        struct Node{
            std::vector<std::pair<CPosType, N>> con;
            Node* children[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
            BoundingBox<T> box;
            Node* parent = nullptr;
            Node(BoundingBox<T>& box) : box(box) {}
            ~Node() {
                for (auto c : con) {
                    delete c.second; // Освобождение памяти для контейнеров
                }
                for (Node* child : children) {
                    delete child; // Рекурсивное освобождение памяти для дочерних узлов
                }
            }

            bool isLeaf() const {
                return children[0] == nullptr;
            }

            std::vector<std::pair<CPosType, N>> getCon(){
                return con;
            }

        };
    private:
        Node* root = nullptr;
        int depth_;
    public:
        Octree(BoundingBox<T>& bbox, int depth) {
        depth_ = depth;
        root = new Node(bbox);
        createTree(root, depth);
    }


        ~Octree() {
            delete root; // Освобождение памяти корневого узла
        }


        Node* getRoot(){
            return root;
        }


        bool insert(N container, CPosType& position, Node* target){
            if(target == nullptr){
                return false;
            }
            target->con.push_back(std::make_pair(position, container));
            return true;
        }


        Octree* Clone(){
            Octree* clone = new Octree(this->root->box, this->depth_);
            return clone;
        }


        void createTree(Node* node, int depth) {
            if (depth <= 0) return;
            
            Point<T> min = node->box.min;
            Point<T> max = node->box.max;

            T midX = (min.x + max.x) / 2;
            T midY = (min.y + max.y) / 2;
            T midZ = (min.z + max.z) / 2;

            Point<T> first = min;
            Point<T> second = Point<T>(midX, midY, midZ);
            BoundingBox<T> box = BoundingBox<T>(first, second);
            node->children[0] = new Node(box); // 0: min
            node->children[0]->parent = node;


            first = Point<T>(midX, min.y, min.z);
            second = Point<T>(max.x, midY, midZ);
            box = BoundingBox<T>(first, second);
            node->children[1] = new Node(box); // 1: x+
            node->children[1]->parent = node;


            first = Point<T>(min.x, midY, min.z);
            second = Point<T>(midX, max.y, midZ);
            box = BoundingBox<T>(first, second);
            node->children[2] = new Node(box); // 2: y+
            node->children[2]->parent = node;


            first = Point<T>(midX, midY, min.z);
            second = Point<T>(max.x, max.y, midZ);
            box = BoundingBox<T>(first, second);
            node->children[3] = new Node(box); // 3: xy+
            node->children[3]->parent = node;


            first = Point<T>(min.x, min.y, midZ);
            second = Point<T>(midX, midY, max.z);
            box = BoundingBox<T>(first, second);
            node->children[4] = new Node(box); // 4: z+
            node->children[4]->parent = node;


            first = Point<T>(midX, min.y, midZ);
            second = Point<T>(max.x, midY, max.z);
            box = BoundingBox<T>(first, second);
            node->children[5] = new Node(box); // 5: x+z+
            node->children[5]->parent = node;


            first = Point<T>(min.x, midY, midZ);
            second =  Point<T>(midX, max.y, max.z);
            box = BoundingBox<T>(first, second);
            node->children[6] = new Node(box); // 6: y+z+
            node->children[6]->parent = node;


            first = Point<T>(midX, midY, midZ);
            second = max;
            box = BoundingBox<T>(first, second);
            node->children[7] = new Node(box); // 7: xyz+
            node->children[7]->parent = node;


            for (auto& child : node->children) {
            createTree(child, depth - 1);
            }
        }


        bool remove(std::string id){
            bool collision = false;
            removeR(id, root, collision);
            if(collision == false){
                return false;
            }
            collision = false;
            return true;
        }


        Node* search(std::string id){
            Node* copyCache = nullptr;
            searchR(id, root, &copyCache);
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


        Node* SearchInsert(N container, CPosType pos){
            bool collision = false;
            Node* copyCache = nullptr;
            std::vector<std::pair<CPosType, N>> copy(0);
            SearchInsert(container, pos, root, &copyCache, copy, collision);
            return copyCache;
        }


        void print() const {
            printNode(root, 0);
        }


            static T getMinX(CPosType& position){
                return std::min({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
            }


            static T getMaxX(CPosType& position){
                return std::max({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
            }


            static T getMinY(CPosType& position){
                return std::min({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
            }


            static T getMaxY(CPosType& position){
                return std::max({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
            }


            static T getMinZ(CPosType& position){
                return std::min({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
            }


            static T getMaxZ(CPosType& position){
                return std::max({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
            }


            static T getMinX(const CPosType& position){
                return std::min({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
            }

            static T getMaxX(const CPosType& position){
                return std::max({position.LLDown.x, position.LLUp.x, position.LRDown.x, position.LRUp.x, position.RRDown.x, position.RRUp.x, position.RLDown.x, position.RLUp.x});
            }


            static T getMinY(const CPosType& position){
                return std::min({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
            }


            static T getMaxY(const CPosType& position){
                return std::max({position.LLDown.y, position.LLUp.y, position.LRDown.y, position.LRUp.y, position.RRDown.y, position.RRUp.y, position.RLDown.y, position.RLUp.y});
            }



            static T getMinZ(const CPosType& position){
                return std::min({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
            }


            static T getMaxZ(const CPosType& position){
                return std::max({position.LLDown.z, position.LLUp.z, position.LRDown.z, position.LRUp.z, position.RRDown.z, position.RRUp.z, position.RLDown.z, position.RLUp.z});
            }


            static bool pointincontainer(Point<T>& p, CPosType& position){
                T minX = getMinX(position);
                T maxX = getMaxX(position);
                T minY = getMinY(position);
                T maxY = getMaxY(position);
                T minZ = getMinZ(position);
                T maxZ = getMaxZ(position);
                return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY && p.z >= minZ && p.z <= maxZ);
            }


        private:


        void  searchUnderOct(Node* node, std::vector<std::pair<CPosType, N>>& vec){
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


        void findI(std::string id, Node* node, std::pair<CPosType, N>* it){
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


        void searchDepth(Node* node, std::vector<std::pair<CPosType, N>>& copyCache){
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


        bool SearchInsert(N container, CPosType pos, Node* node, Node** copyCache, std::vector<std::pair<CPosType, N>>& copy, bool& collision){
            //std::unique_lock<std::mutex> ul(mtx, std::defer_lock);
            if(node == nullptr){
                return false; // узел пустой
            }
             if (!node->box.contains(pos.LLDown) || !node->box.contains(pos.LLUp) ||
                !node->box.contains(pos.LRDown) || !node->box.contains(pos.LRUp) ||
                !node->box.contains(pos.RLDown) || !node->box.contains(pos.RLUp) ||
                !node->box.contains(pos.RRDown) || !node->box.contains(pos.RRUp)) {
                return false; // позиция выходит за границы узла
            }
            if(!node->con.empty()){
                (copy).insert((copy).end(), node->con.begin(), node->con.end());
            }
            if (node->isLeaf() == false && collision == false && *copyCache == nullptr) {
                for (size_t i = 0; i < 8; ++i) {
                     SearchInsert(container, pos, node->children[i], copyCache, copy, collision);
                }
            }
            if(collision == false && *copyCache == nullptr){
                
                searchUnderOct(node, copy);
                
                if(!checkCollision(container, pos, copy)){
                    //ul.lock();
                    *copyCache = node;
                   // ul.unlock();
                    return true;
                }else{
                    collision = true;
                    return false;
                }
            }else if(collision == false && *copyCache != nullptr){
                return true;
            }
            return false;
        }


            void searchR(std::string id, Node* node, Node** copyCache){
                if(node->con.empty() == false){
                for(auto& i : node->con){
                    if(i.first.LLDown.x != -1 && i.second != nullptr && number(i.first.LLDown) == id){
                        *copyCache = node;
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


            void removeR(std::string id, Node* node, bool& collision){
                if(collision == true || node == nullptr){
                    return;
                }
                if(node->con.empty() == false){
                    for(auto& i : node->con){
                        if(i.first.LLDown.x != -1 && i.second != nullptr && number(i.first.LLDown) == id){
                            delete i.second;
                            node->con.erase(std::remove(node->con.begin(), node->con.end(), i), node->con.end());
                            collision = true;
                            return;
                        }
                    }
                }
                if(node->isLeaf() == false){
                    for(int i = 0; i < 8; ++i){
                        removeR(id, node->children[i], collision);
                    }
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


            void printNode(Node* node, int depth) const {
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