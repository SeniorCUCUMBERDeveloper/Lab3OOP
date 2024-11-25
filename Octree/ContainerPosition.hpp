#ifndef CONTAINERPOSITION_HPP
#define CONTAINERPOSITION_HPP
#include "Point.hpp"

template <typename T>
struct ContainerPosition{
  Point<T> LLDown, LLUp;
  Point<T> LRDown, LRUp;
  Point<T> RRDown, RRUp;
  Point<T> RLDown, RLUp;

  ContainerPosition(){}
  ContainerPosition(Point<T> llDown, Point<T> llUp, Point<T> lrDown, Point<T> lrUp, 
                      Point<T> rrDown, Point<T> rrUp, Point<T> rlDown, Point<T> rlUp)
        : LLDown(llDown), LLUp(llUp), LRDown(lrDown), LRUp(lrUp),
          RRDown(rrDown), RRUp(rrUp), RLDown(rlDown), RLUp(rlUp) {}
//delete
  bool operator==(const ContainerPosition<T>& other) const {
        return LLDown == other.LLDown && LLUp == other.LLUp &&
               LRDown == other.LRDown && LRUp == other.LRUp &&
               RRDown == other.RRDown && RRUp == other.RRUp &&
               RLDown == other.RLDown && RLUp == other.RLUp;
    }

    bool operator<(const ContainerPosition<T>& other) const {
        return std::tie(LLDown, LLUp, LRDown, LRUp, RRDown, RRUp, RLDown, RLUp) <
               std::tie(other.LLDown, other.LLUp, other.LRDown, other.LRUp, other.RRDown, other.RRUp, other.RLDown, other.RLUp);
    }
};

#endif