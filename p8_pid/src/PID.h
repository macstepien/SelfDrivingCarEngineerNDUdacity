#ifndef PID_H
#define PID_H

class PID {
 public:
  /**
   * Constructor
   * @param (Kp, Ki, Kd) The initial PID coefficients
   */
  PID(double Kp, double Ki, double Kd) : Kp_(Kp), Ki_(Ki), Kd_(Kd){};

  /**
   * Destructor.
   */
  virtual ~PID(){};

  /**
   * Update and calculate the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  double Calculate(double cte) {
    cte_sum_ += cte;
    double y = Kp_ * cte + Ki_ * cte_sum_ + Kd_ * (cte - last_cte_);
    last_cte_ = cte;
    return y;
  };

 private:
  /**
   * PID Coefficients
   */
  double Kp_;
  double Ki_;
  double Kd_;

  double last_cte_ = 0.;
  double cte_sum_ = 0.;
};

#endif  // PID_H