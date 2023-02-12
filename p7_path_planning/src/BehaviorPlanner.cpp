#include "BehaviorPlanner.h"

#include <iostream>
#include <limits>

using std::map;
using std::vector;

void BehaviorPlanner::plan(double car_s_end_path, Vehicle* car,
                           const vector<double>& previous_path_x,
                           const vector<double>& previous_path_y,
                           const vector<vector<double>>& sensor_fusion) {
  int prev_size = previous_path_x.size();

  int current_lane = car->getLane();

  if (car->getState() == "CL") {
    double dist_to_lane =
        fabs(getLaneD(current_lane) - car->getPositionFrenet()[1]);
    if (dist_to_lane < LANE_TRANSITION_DIST_THRES_) car->changeState("KL");
  }

  vector<double> closest_vehicle = findClosestVehicleInLaneAhead(
      current_lane, sensor_fusion, prev_size, car_s_end_path);

  if (closest_vehicle.size() == 0) {
    car->changeTargetVel(MAX_SPEED_);
    return;
  }

  if (closest_vehicle[1] > CLOSE_CAR_DIST_THRES_) {
    car->changeTargetVel(MAX_SPEED_);
    return;
  }

  if (closest_vehicle[1] < CLOSE_CAR_DIST_CRITICAL_THRES_) {
    car->changeTargetVel(closest_vehicle[0] - DISTANCE_GAIN_SPEED_DIFFERENCE_);
    return;
  }

  bool lane_changed = false;

  if (car->getState() == "KL") {
    vector<int> possibleLanes = possibleLaneChanges(current_lane);
    vector<int> laneChangeCandidates;

    for (auto l : possibleLanes) {
      if (safeToChangeLane(l, car_s_end_path, car->getVelocity(), prev_size,
                           sensor_fusion)) {
        laneChangeCandidates.push_back(l);
      }
    }

    if (laneChangeCandidates.size() > 0) {
      int result = checkLaneCosts(current_lane, laneChangeCandidates,
                                  sensor_fusion, car_s_end_path, prev_size);
      if (result != -1) {
        lane_changed = true;
        car->changeLane(result);
        car->changeState("CL");
        std::cout << "Changing lane " << current_lane << "->" << result
                  << std::endl;
      }
    }
  }

  if (!lane_changed) {
    car->changeTargetVel(closest_vehicle[0]);
  }
}

vector<double> BehaviorPlanner::findClosestVehicleInLaneAhead(
    int lane, const vector<vector<double>>& sensor_fusion, int prev_size,
    int car_s_end_path) {
  vector<double> closest_vehicle = {-1,
                                    std::numeric_limits<double>::infinity()};
  bool vehicle_found = false;

  for (const vector<double>& measurement : sensor_fusion) {
    if (getLane(measurement[6]) != lane) continue;

    double checked_car_speed = calcSpeed(measurement[3], measurement[4]);

    // We are creating plan for the future (end of current path), so we have to
    // check other cars' positions accordingly
    double checked_car_s =
        measurement[5] + (double(prev_size) * D_T_ * checked_car_speed);

    double dist_to_car = checked_car_s - car_s_end_path;
    if (dist_to_car > 0 && dist_to_car < closest_vehicle[1]) {
      closest_vehicle = {checked_car_speed, dist_to_car};
      vehicle_found = true;
    }
  }

  if (vehicle_found)
    return closest_vehicle;
  else
    return vector<double>();
}

map<int, double> BehaviorPlanner::laneCostFunction(
    const vector<vector<double>>& sensor_fusion, double car_s_end_path,
    int prev_size) {
  map<int, double> costs;
  for (int i = 0; i < 3; ++i) {
    std::vector<double> closest_vehicle = findClosestVehicleInLaneAhead(
        i, sensor_fusion, prev_size, car_s_end_path);

    // Further distance is preffered so closest vehicle distance is inverted
    double cost = 0;
    if (closest_vehicle.size() > 0) {
      cost = WEIGHT_SPEED_ * (MAX_SPEED_ - closest_vehicle[0]) +
             WEIGHT_DISTANCE * (1. / closest_vehicle[1]);
    }

    costs[i] = cost;
  }
  return costs;
}

int BehaviorPlanner::checkLaneCosts(int current_lane,
                                    const vector<int>& candidates,
                                    const vector<vector<double>>& sensor_fusion,
                                    double car_s, int prev_size) {
  if (candidates.size() == 0) return -1;

  map<int, double> costs = laneCostFunction(sensor_fusion, car_s, prev_size);

  int best_candidate_idx = candidates[0];

  for (int candidate : candidates) {
    if (costs[candidate] < costs[best_candidate_idx]) {
      best_candidate_idx = candidate;
    }
  }

  if (costs[current_lane] - costs[best_candidate_idx] >
      LANE_CHANGE_COST_THRESHOLD_)
    return best_candidate_idx;
  else
    return -1;
}

bool BehaviorPlanner::safeToChangeLane(
    int dest_lane, double car_s, double car_speed, int prev_size,
    const vector<vector<double>>& sensor_fusion) {
  for (const vector<double>& measurement : sensor_fusion) {
    if (getLane(measurement[6]) != dest_lane) continue;

    double checked_car_speed = calcSpeed(measurement[3], measurement[4]);

    double checked_car_s =
        measurement[5] + (double(prev_size) * D_T_ * checked_car_speed);

    // Check five seconds into future
    double time_interval = 0.5;
    double car_s_predictions = car_s;
    double time = 0;
    while (time < LANE_CHANGE_CHECK_TIME_HORIZON_) {
      float dist_to_car = fabs(checked_car_s - car_s_predictions);
      if (dist_to_car < MIN_DIST_TO_VEHICLE_CHANGE_LANE_) return false;
      checked_car_s += checked_car_speed * time_interval;
      car_s_predictions += car_speed * time_interval;
      time += time_interval;
    }
  }
  return true;
}

vector<int> BehaviorPlanner::possibleLaneChanges(int lane) {
  vector<int> lane_changes;

  if (lane == 1) {
    lane_changes.push_back(0);
    lane_changes.push_back(2);
  } else if (lane == 0 || lane == 2) {
    lane_changes.push_back(1);
  }

  return lane_changes;
}