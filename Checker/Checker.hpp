#ifndef CHECKER_HPP
#define CHECKER_HPP


#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>
#include "../Container/Container.hpp"
#include "../Container/FragileContainer.hpp"
#include "../Container/RefragedContainer.hpp"
#include "../Container/Frag_and_Ref.hpp"
#include "../Storage/Storage.hpp"


template <typename T>
class Checker {
public:
    using CheckFunction = std::function<void(IContainer*, ContainerPosition<T>)>;



    Checker() = default;

    Checker(const Checker& other)
        : checkFunctions(other.checkFunctions) {}
        
    void addCheckFunction(CheckFunction func) {
        checkFunctions.push_back(func);
    }

    
    void removeCheckFunction(size_t index) {
        if (index >= checkFunctions.size()) {
            throw std::out_of_range("Index is out of range");
        }
        checkFunctions.erase(checkFunctions.begin() + index);
    }

    
    void applyChecks(IContainer* container, ContainerPosition<int> position) const {
        for (const auto& func : checkFunctions) {
            func(container, position);
        }
    }

private:
    std::vector<CheckFunction> checkFunctions;
};


#endif