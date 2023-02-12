#include "FusionEKF.h"
#include <iostream>
#include "Eigen/Dense"
#include "Eigen/Geometry"
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/**
 * Constructor.
 */
FusionEKF::FusionEKF()
{
  is_initialized_ = false;
  previous_timestamp_ = 0;

  // state vector x
  VectorXd x = VectorXd::Zero(4);

  // state covariance matrix P
  MatrixXd P = MatrixXd(4, 4);
  P << 1, 0, 0, 0,
       0, 1, 0, 0,
       0, 0, 1000, 0,
       0, 0, 0, 1000;

  // the initial transition matrix F_
  MatrixXd F = MatrixXd::Zero(4, 4);
  MatrixXd Q = MatrixXd::Zero(4, 4);

  // measurement matrix
  MatrixXd H_laser = MatrixXd(2, 4);
  H_laser << 1, 0, 0, 0,
             0, 1, 0, 0;

  //measurement covariance matrix - laser
  R_laser_ = MatrixXd(2, 2);
  R_laser_ << 0.0225, 0,
             0, 0.0225;

  std::function<VectorXd(const VectorXd &)> h_radar([](const VectorXd & x){
    VectorXd xp(3);
    xp[0] = sqrt(x[0] * x[0] + x[1] * x[1]);
    
    if(x[0] == 0 && x[1] == 0)
    {
      std::cerr << "[h function] Both position values are 0, can't calculate angle";
      xp[1] = 0;
    }
    else
      xp[1] = std::atan2(x[1], x[0]);
    
    if (xp[0] != 0)
      xp[2] = (x[0] * x[2] + x[1] * x[3]) / xp[0];
    else
    {
      std::cerr << "[h function] Both position values are 0, setting velocity to 0";
      xp[2] = 0;
    }
    
    return xp;
  });

  //measurement covariance matrix - radar
  R_radar_ = MatrixXd(3, 3);
  R_radar_ << 0.09, 0, 0,
             0, 0.0009, 0,
             0, 0, 0.09;

  float noise_ax = 9;
  float noise_ay = 9;

  ekf_.Init(x, P, F, Q, H_laser, R_laser_, h_radar, R_radar_, noise_ax, noise_ay);
}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack)
{
  /**
   * Initialization
   */
  if (!is_initialized_)
  {
    VectorXd first_x(4);
    MatrixXd first_P(4, 4);

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR)
    {
      std::cout << "Kalman Filter Radar Initialization " << std::endl;

      //Convert from polar to cartesian
      float x = measurement_pack.raw_measurements_[0] * cos(measurement_pack.raw_measurements_[1]);
      float y = measurement_pack.raw_measurements_[0] * sin(measurement_pack.raw_measurements_[1]);
      first_x << x, y, 0, 0;

      //Initialize covariance matrix with first measurement covariance (sensors) 
      //Convert from polar to cartesian
      MatrixXd rotation_matrix = Eigen::Rotation2Dd(measurement_pack.raw_measurements_[1]).toRotationMatrix();
      MatrixXd converted_R_radar = rotation_matrix * R_radar_.block(0,0,2,2) * rotation_matrix.transpose();
      first_P << converted_R_radar(0,0), 0, 0, 0,
                 0, converted_R_radar(1,1), 0, 0,
                 0, 0, 1000, 0,
                 0, 0, 0, 1000;
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER)
    {
      std::cout << "Kalman Filter Laser Initialization " << std::endl;

      // set the state with the initial location and zero velocity
      first_x << measurement_pack.raw_measurements_[0],
                 measurement_pack.raw_measurements_[1],
                 0,
                 0;
                 
      //Initialize covariance matrix with first measurement covariance (sensors) 
      first_P << R_laser_(0,0), 0, 0, 0,
                 0, R_laser_(1,1), 0, 0,
                 0, 0, 1000, 0,
                 0, 0, 0, 1000;
    }
    else
    {
      std::cerr << "[ERROR] Unknown type of sensor, can't initialize Kalman Filter" << std::endl;
      return;
    }

    ekf_.FirstMeasurement(first_x, first_P);
    previous_timestamp_ = measurement_pack.timestamp_;
    is_initialized_ = true;
    return;
  }

  /**
   * Prediction
   */

  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
  previous_timestamp_ = measurement_pack.timestamp_;

  ekf_.Predict(dt);

  /**
   * Update
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR)
  {
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
  }
  else
  {
    ekf_.Update(measurement_pack.raw_measurements_);
  }

  std::cout << "x_ = " << ekf_.getState() << std::endl;
  std::cout << "P_ = " << ekf_.getCovariance() << std::endl;
}
