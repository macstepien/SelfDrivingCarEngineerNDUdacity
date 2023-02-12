#include "kalman_filter.h"
#include "tools.h"

#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in,
                        MatrixXd &F_in, MatrixXd &Q_in,
                        MatrixXd &H_kf_in, MatrixXd &R_kf_in,
                        std::function<VectorXd(const VectorXd &x)> &h_ekf_in, MatrixXd &R_ekf_in,
                        float noise_ax_in, float noise_ay_in)
{
  x_ = x_in;
  P_ = P_in;

  F_ = F_in;
  Q_ = Q_in;

  H_kf_ = H_kf_in;
  R_kf_ = R_kf_in;

  h_ekf_ = h_ekf_in;
  R_ekf_ = R_ekf_in;

  noise_ax_ = noise_ax_in;
  noise_ay_ = noise_ay_in;
}

void KalmanFilter::FirstMeasurement(const Eigen::VectorXd &x, const Eigen::MatrixXd &P)
{
  x_ = x;
  P_ = P;
}

void KalmanFilter::Predict(float dt)
{
  //Calculate new F and Q with given dt
  F_ = MatrixXd::Identity(4, 4);
  F_(0, 2) = dt;
  F_(1, 3) = dt;

  float dt2 = dt * dt;
  float dt3 = dt2 * dt;
  float dt4 = dt3 * dt;

  Q_ = MatrixXd(4, 4);
  Q_ << noise_ax_ * dt4 / 4, 0, noise_ax_ * dt3 / 2, 0,
        0, noise_ay_ * dt4 / 4, 0, noise_ay_ * dt3 / 2,
        noise_ax_ * dt3 / 2, 0, noise_ax_ * dt2, 0,
        0, noise_ay_ * dt3 / 2, 0, noise_ay_ * dt2;

  // KF Prediction step
  x_ = F_ * x_;
  P_ = F_ * P_ * F_.transpose() + Q_;
}

void KalmanFilter::Update(const VectorXd &z)
{
  MatrixXd I = MatrixXd::Identity(4, 4);
  MatrixXd y = z - H_kf_ * x_;
  MatrixXd S = H_kf_ * P_ * H_kf_.transpose() + R_kf_;
  MatrixXd K = P_ * H_kf_.transpose() * S.inverse();
  
  x_ += K * y;
  P_ = (I - K * H_kf_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z)
{
  MatrixXd I = MatrixXd::Identity(4, 4);
  MatrixXd Hj = Tools::CalculateJacobian(x_);

  MatrixXd y = z - h_ekf_(x_);
  y(1) = atan2(sin(y(1)), cos(y(1))); //convert to [-pi, pi] range
  MatrixXd S = Hj * P_ * Hj.transpose() + R_ekf_;
  MatrixXd K = P_ * Hj.transpose() * S.inverse();

  x_ += K * y;
  P_ = (I - K * Hj) * P_;
}