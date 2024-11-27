#ifndef CHECKER_HPP
#define CHECKER_HPP


#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>
#include "../Container/Container.hpp"


class Storage;

template <typename T>
class Checker {
public:
    using CheckFunction = std::function<void(Storage&, IContainer*, ContainerPosition<T>)>;
private:
    std::vector<CheckFunction> checkFunctions;

public:



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

    
    void applyChecks(Storage& storage, IContainer* container, ContainerPosition<int> position) const {
        for (const auto& func : checkFunctions) {
            func(storage, container, position);
        }
    }

};


#endif