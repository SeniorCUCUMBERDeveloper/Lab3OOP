#ifndef TERMINAL_HPP
#define TERMINAL_HPP


#include <map>
#include <iostream>
#include "../Storage/Storage.hpp"


class Terminal{
    private:
        std::map<int, Storage*> terminal;
    public:
        void add(int id, Storage* storage);
        void remove(int id);
        void setsizeStorage(int id, int length, int width, int height);
        void getallInfo(std::ostream& output);
        Storage* find(int id);
        ~Terminal();
};




#endif