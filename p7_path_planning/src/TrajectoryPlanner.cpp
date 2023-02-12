#include "TrajectoryPlanner.h"

#include "helpers.h"
#include "spline.h"

using std::vector;

void TrajectoryPlanner::plan(const Vehicle& car,
                             const vector<double>& previous_path_x,
                             const vector<double>& previous_path_y,
                             const vector<double>& map_waypoints_s,
                             const vector<double>& map_waypoints_x,
                             const vector<double>& map_waypoints_y,
                             vector<double>* next_x_vals,
                             vector<double>* next_y_vals, double end_path_s) {
  int prev_size = previous_path_x.size();

  std::vector<double> car_position = car.getPositionXY();

  double ref_x = car_position[0];
  double ref_y = car_position[1];
  double ref_yaw = deg2rad(car_position[2]);

  vector<double> ptsx;
  vector<double> ptsy;

  // Get previous positions to ensure spline smoothness and continuity
  if (prev_size < 2) {
    double prev_car_x = car_position[0] - cos(car_position[2]);
    double prev_car_y = car_position[1] - sin(car_position[2]);

    ptsx.push_back(prev_car_x);
    ptsx.push_back(car_position[0]);

    ptsy.push_back(prev_car_y);
    ptsy.push_back(car_position[1]);
  } else {
    ref_x = previous_path_x[prev_size - 1];
    double ref_x_prev = previous_path_x[prev_size - 2];

    ref_y = previous_path_y[prev_size - 1];
    double ref_y_prev = previous_path_y[prev_size - 2];

    ref_yaw = atan2(ref_y - ref_y_prev, ref_x - ref_x_prev);

    ptsx.push_back(ref_x_prev);
    ptsx.push_back(ref_x);

    ptsy.push_back(ref_y_prev);
    ptsy.push_back(ref_y);
  }

  // Points ahead of vehicle
  vector<double> next_wp0 =
      getXY(end_path_s + LOOK_AHEAD_POINT_1_, (2 + 4. * car.getLane()),
            map_waypoints_s, map_waypoints_x, map_waypoints_y);
  vector<double> next_wp1 =
      getXY(end_path_s + LOOK_AHEAD_POINT_2_, (2 + 4. * car.getLane()),
            map_waypoints_s, map_waypoints_x, map_waypoints_y);
  vector<double> next_wp2 =
      getXY(end_path_s + LOOK_AHEAD_POINT_3_, (2 + 4. * car.getLane()),
            map_waypoints_s, map_waypoints_x, map_waypoints_y);

  ptsx.push_back(next_wp0[0]);
  ptsx.push_back(next_wp1[0]);
  ptsx.push_back(next_wp2[0]);

  ptsy.push_back(next_wp0[1]);
  ptsy.push_back(next_wp1[1]);
  ptsy.push_back(next_wp2[1]);

  // Convert to local coordinate system (ensure that from points function can be
  // made - always xi+1 > xi, otherwise spline function would result in error)
  for (int i = 0; i < ptsx.size(); ++i) {
    double shift_x = ptsx[i] - ref_x;
    double shift_y = ptsy[i] - ref_y;

    ptsx[i] = (shift_x * cos(-ref_yaw) - shift_y * sin(-ref_yaw));
    ptsy[i] = (shift_x * sin(-ref_yaw) + shift_y * cos(-ref_yaw));
  }

  tk::spline s;

  s.set_points(ptsx, ptsy);

  // Push old path points to ensure path continuity
  for (int i = 0; i < previous_path_x.size(); ++i) {
    next_x_vals->push_back(previous_path_x[i]);
    next_y_vals->push_back(previous_path_y[i]);
  }

  double target_y = s(TARGET_X_);
  double target_dist = sqrt(TARGET_X_ * TARGET_X_ + target_y * target_y);

  double x_add_on = 0.;
  // Calculate missing points so that path has NUM_POINT_IN_PATH_ points.
  // Current car speed is used to calculate how far apart they should be.
  for (int i = 1; i <= NUM_POINT_IN_PATH_ - previous_path_x.size(); ++i) {
    double N = (target_dist / (D_T_ * car.getVelocity()));
    double x_point = x_add_on + (TARGET_X_ / N);
    double y_point = s(x_point);
    x_add_on = x_point;
    double x_ref = x_point;
    double y_ref = y_point;

    x_point = ref_x + (x_ref * cos(ref_yaw) - y_ref * sin(ref_yaw));
    y_point = ref_y + (x_ref * sin(ref_yaw) + y_ref * cos(ref_yaw));

    next_x_vals->push_back(x_point);
    next_y_vals->push_back(y_point);
  }
}