#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <vector>

class Vehicle {
 private:
  //  Constant acceleration value that is used for changing speeds. It is around
  //  5m/s2
  const double MAX_ACCELERATION_ = 0.224;

  double current_vel_;
  double target_vel_;
  int lane_;

  // Current state. Can be "KL" (Keep lane) or "CL" (Change lane)
  std::string state_;

  double x_, y_, yaw_, s_, d_;

 public:
  Vehicle(double ref_vel = 0, double target_vel = 21, int lane = 1,
          std::string current_state = "KL")
      : current_vel_(ref_vel),
        target_vel_(target_vel),
        lane_(lane),
        state_(current_state) {}

  void updateSpeed() {
    if (current_vel_ < target_vel_) {
      current_vel_ += MAX_ACCELERATION_;
    } else {
      current_vel_ -= MAX_ACCELERATION_;
    }
  }

  void updatePosition(double x, double y, double yaw, double s, double d) {
    x_ = x;
    y_ = y;
    yaw_ = yaw;
    s_ = s;
    d_ = d;
  }

  void changeTargetVel(double new_target_vel) { target_vel_ = new_target_vel; }
  void changeLane(int new_lane) { lane_ = new_lane; }
  void changeState(std::string new_state) { state_ = new_state; }

  std::string getState() const { return state_; }
  std::vector<double> getPositionXY() const { return {x_, y_, yaw_}; }
  std::vector<double> getPositionFrenet() const { return {s_, d_}; }
  double getVelocity() const { return current_vel_; }
  int getLane() const { return lane_; }
};

#endif