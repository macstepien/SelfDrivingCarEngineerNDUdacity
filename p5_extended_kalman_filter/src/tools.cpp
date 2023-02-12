#include "tools.h"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth)
{
  VectorXd rmse(4);
  rmse << 0, 0, 0, 0;

  if (estimations.size() == 0 || estimations.size() != ground_truth.size())
  {
    std::cerr << "Size error";
    return rmse;
  }

  for (int i = 0; i < estimations.size(); ++i)
  {
    VectorXd errors = (estimations[i] - ground_truth[i]);
    errors = errors.array() * errors.array();
    rmse += errors;
  }

  rmse = (rmse / estimations.size()).array().sqrt();

  return rmse;
}

MatrixXd Tools::CalculateJacobian(const VectorXd &x_state)
{
  MatrixXd Hj(3, 4);

  float px = x_state(0);
  float py = x_state(1);
  float vx = x_state(2);
  float vy = x_state(3);

  // check division by zero
  if (px == 0 && py == 0)
  {
    std::cerr << "Error, 0 division";
    return Hj;
  }

  // compute the Jacobian matrix
  float l2 = px * px + py * py;
  float l = sqrt(l2);
  float l32 = l2 * l;

  Hj << px / l, py / l, 0, 0,
        -py / l2, px / l2, 0, 0,
        py * (vx * py - vy * px) / l32, px * (vy * px - vx * py) / l32, px / l, py / l;

  return Hj;
}
