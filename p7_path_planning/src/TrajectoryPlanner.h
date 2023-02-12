#ifndef TRAJECTORY_PLANNER_H
#define TRAJECTORY_PLANNER_H

#include <vector>

#include "Vehicle.h"

class TrajectoryPlanner {
 private:
  // Simulation time change, time between two segments on path
  const double D_T_ = 0.02;

  // Vehicle's destincation is considered to be this much ahead
  const double TARGET_X_ = 30.;

  // Planned trajectory will consist of this many points
  const int NUM_POINT_IN_PATH_ = 50;

  // Points ahead of vehicle used for spline calculation
  const double LOOK_AHEAD_POINT_1_ = 30;
  const double LOOK_AHEAD_POINT_2_ = 60;
  const double LOOK_AHEAD_POINT_3_ = 90;

 public:
  TrajectoryPlanner(){};
  void plan(const Vehicle& car, const std::vector<double>& previous_path_x,
            const std::vector<double>& previous_path_y,
            const std::vector<double>& map_waypoints_s,
            const std::vector<double>& map_waypoints_x,
            const std::vector<double>& map_waypoints_y,
            std::vector<double>* next_x_vals, std::vector<double>* next_y_vals,
            double end_path_s);
};

#endif