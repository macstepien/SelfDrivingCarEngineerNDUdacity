# Particle Filter Project

[image1]: ./project.png "Finished simulation of the particle filter"

Goal of this project was to implement Particle Filter that will estimate position of the robot (x,y,theta) given initial position from GPS, map (list of landmark x,y positions) series of control information (linear and angular velocity) and detected landmarks (x,y) in local coordinate frame of the robot. As landmarks were only provided in terms of positions it was additionally necessary to implement landmark association (based on closest distance). Each particle has weight value (based on value of multivariable gaussian function of difference between predicted and detected landmark positions). Using these weights resampling of particles takes place and particle with highest weight provides current estimate of the position of the robot. Implementation can be found in `particle_filter.h` and `particle_filter.cpp`.


![alt text][image1]