/**
 * map.h
 *
 * Created on: Dec 12, 2016
 * Author: mufferm
 */

#ifndef MAP_H_
#define MAP_H_

#include <map>
#include <vector>

class Map {
 public:
  struct single_landmark_s {
    float x;  // Landmark x-position in the map (global coordinates)
    float y;  // Landmark y-position in the map (global coordinates)
  };

  std::map<int, single_landmark_s> landmark_map;
};

#endif  // MAP_H_
