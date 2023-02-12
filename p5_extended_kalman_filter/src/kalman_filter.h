#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

#include "Eigen/Dense"

class KalmanFilter
{
public:
  /**
   * Constructor
   */
  KalmanFilter();

  /**
   * Destructor
   */
  virtual ~KalmanFilter();

  /**
   * Init Initializes Kalman filter
   * @param x_in Initial state
   * @param P_in Initial state covariance
   * @param F_in Transition matrix
   * @param Q_in Process covariance matrix
   * @param H_kf_in Measurement matrix for Kalman Filter
   * @param R_kf_in Measurement covariance matrix for Kalman filter
   * @param h_ekf_in Measurement function for Extended Kalman Filter
   * @param R_ekf_in Measurement covariance matrix for Extended Kalman filter
   * @param noise_ax_in Acceleration standard deviation used for calculating Q
   * @param noise_ay_in Acceleration standard deviation used for calculating Q
   */
  void Init(Eigen::VectorXd &x_in, Eigen::MatrixXd &P_in,
            Eigen::MatrixXd &F_in, Eigen::MatrixXd &Q_in,
            Eigen::MatrixXd &H_kf_in, Eigen::MatrixXd &R_kf_in,
            std::function<Eigen::VectorXd(const Eigen::VectorXd &x)> &h_ekf_in, Eigen::MatrixXd &R_ekf_in,
            float noise_ax_in, float noise_ay_in);

  /**
   * Prediction Predicts the state and the state covariance
   * using the process model
   * @param dt Time between k and k+1 in s
   */
  void Predict(float dt);

  /**
   * Updates the state by using standard Kalman Filter equations
   * @param z The measurement at k+1
   */
  void Update(const Eigen::VectorXd &z);

  /**
   * Updates the state by using Extended Kalman Filter equations
   * @param z The measurement at k+1
   */
  void UpdateEKF(const Eigen::VectorXd &z);

  void FirstMeasurement(const Eigen::VectorXd &x, const Eigen::MatrixXd &P);

  Eigen::VectorXd getState() { return x_; };
  Eigen::MatrixXd getCovariance() { return P_; };

private:
  // state vector
  Eigen::VectorXd x_;
  // state covariance matrix
  Eigen::MatrixXd P_;

  // Prediction
  // state transition matrix
  Eigen::MatrixXd F_;
  // process covariance matrix
  Eigen::MatrixXd Q_;

  // Kalman Filter
  // measurement matrix
  Eigen::MatrixXd H_kf_;
  // measurement covariance matrix for regular Kalman Filter
  Eigen::MatrixXd R_kf_;

  // Extended Kalman Filter
  // measurement matrix
  std::function<Eigen::VectorXd(const Eigen::VectorXd &)> h_ekf_;
  // measurement covariance matrix for Extended Kalman Filter
  Eigen::MatrixXd R_ekf_;

  float noise_ax_;
  float noise_ay_;
};

#endif // KALMAN_FILTER_H_
