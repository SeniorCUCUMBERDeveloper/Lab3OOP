#include "Terminal.hpp"

void Terminal::add(int id, Storage* storage){
    if(terminal.find(id) != terminal.end()){
        throw std::invalid_argument("Terminal already");
    }
    terminal[id] = storage;
}

void Terminal::remove(int id){
    if(terminal.find(id) == terminal.end()){
        throw std::invalid_argument("Terminal does not exist");
    }
    delete terminal[id];
    terminal.erase(id);
}

void Terminal::getsizeIStorage(int id, int length, int width, int height){
    if(terminal.find(id) == terminal.end()){
        throw std::invalid_argument("Terminal does not exist");
    }
    terminal[id]->getSize(length, width, height);
}

void Terminal::getallInfo(std::ostream& output){
    for (const auto& entry : terminal) {
        int id = entry.first;
        Storage* storage = entry.second;
        output << "Storage ID: " << id << std::endl;
    }
}

Terminal::~Terminal(){
    for (auto& entry : terminal) {
        delete entry.second;
    }
    terminal.clear();
}
