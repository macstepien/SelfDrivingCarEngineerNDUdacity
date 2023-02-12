#ifndef BEHAVIOR_PANNER_H
#define BEHAVIOR_PANNER_H

#include <cmath>
#include <map>

#include "Vehicle.h"

class BehaviorPlanner {
 private:
  // Parameters

  // If distance between current d and lane d is less than this threshold state
  // is changed from CL to KL
  const double LANE_TRANSITION_DIST_THRES_ = 0.2;

  // If other vehicle in lane is closer than this, then speed is set to this
  // car's or lane could be changed
  const double CLOSE_CAR_DIST_THRES_ = 30;

  // If other vehicle is detected below this distance car's speed is set to
  // (other's vehicle speed - DISTANCE_GAIN_SPEED_DIFFERENCE_)
  const double CLOSE_CAR_DIST_CRITICAL_THRES_ = 10;
  const double DISTANCE_GAIN_SPEED_DIFFERENCE_ = 4;

  // Simulation time change, time between two segments on path
  const double D_T_ = 0.02;

  // Speed limit (a little lower than 50MPH)
  const double MAX_SPEED_ = 21;

  // There shouldn't be a vehicle behind and ahead in this threshold in
  // destination lane for maneuver to be considered safe
  const double MIN_DIST_TO_VEHICLE_CHANGE_LANE_ = 10;

  // Distance between vehicles in destination lane is also checked for the
  // future using calculated speeds
  const double LANE_CHANGE_CHECK_TIME_HORIZON_ = 5.;

  // If cost of the lane is similar to the one we are currently on it won't
  // change lane. It has to be above this threshold
  const double LANE_CHANGE_COST_THRESHOLD_ = 2;

  // Weights for the lane cost function
  const double WEIGHT_SPEED_ = 1;
  const double WEIGHT_DISTANCE = 150;

  double getLaneD(int lane) { return 4. * lane + 2.; };
  int getLane(double d) { return d / 4; };

  double calcSpeed(double vx, double vy) {
    return std::sqrt(vx * vx + vy * vy);
  }

  std::vector<double> findClosestVehicleInLaneAhead(
      int lane, const std::vector<std::vector<double>>& sensor_fusion,
      int prev_size, int car_s_end_path);

  std::vector<int> possibleLaneChanges(int lane);

  bool safeToChangeLane(int dest_lane, double car_s, double car_speed,
                        int prev_size,
                        const std::vector<std::vector<double>>& sensor_fusion);

  std::map<int, double> laneCostFunction(
      const std::vector<std::vector<double>>& sensor_fusion,
      double car_s_end_path, int prev_size);

  int checkLaneCosts(int current_lane, const std::vector<int>& candidates,
                     const std::vector<std::vector<double>>& sensor_fusion,
                     double car_s, int prev_size);

 public:
  BehaviorPlanner(){};
  void plan(double car_s_end_path, Vehicle* car,
            const std::vector<double>& previous_path_x,
            const std::vector<double>& previous_path_y,
            const std::vector<std::vector<double>>& sensor_fusion);
};

#endif