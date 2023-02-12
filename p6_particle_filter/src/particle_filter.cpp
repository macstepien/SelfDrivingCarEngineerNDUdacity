/**
 * particle_filter.cpp
 *
 * Created on: Dec 12, 2016
 * Author: Tiffany Huang
 */

#include "particle_filter.h"

#include <math.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <map>

#include "helper_functions.h"

using std::string;
using std::vector;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  num_particles = 100;  

  std::default_random_engine gen;

  std::normal_distribution<double> dist_x(x, std[0]);
  std::normal_distribution<double> dist_y(y, std[1]);
  std::normal_distribution<double> dist_theta(theta, std[2]);

  Particle newParticle;
  for (int i = 0; i < num_particles; ++i) {
    newParticle.id = i;

    newParticle.x = dist_x(gen);
    newParticle.y = dist_y(gen);
    newParticle.theta = dist_theta(gen);

    newParticle.weight = 1.;

    particles.push_back(newParticle);
  }

  is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[],
                                double velocity, double yaw_rate) {

  std::default_random_engine gen;

  for (Particle &p : particles) {
    double x, y, theta;
    if (yaw_rate != 0) {
      x = p.x + (velocity / yaw_rate) *
                    (sin(p.theta + yaw_rate * delta_t) - sin(p.theta));
      y = p.y + (velocity / yaw_rate) *
                    (cos(p.theta) - cos(p.theta + yaw_rate * delta_t));
      theta = p.theta + yaw_rate * delta_t;
    } else {
      x = p.x + velocity * delta_t * cos(p.theta);
      y = p.y + velocity * delta_t * sin(p.theta);
      theta = p.theta;
    }

    std::normal_distribution<double> dist_x(x, std_pos[0]);
    std::normal_distribution<double> dist_y(y, std_pos[1]);
    std::normal_distribution<double> dist_theta(theta, std_pos[2]);

    p.x = dist_x(gen);
    p.y = dist_y(gen);
    p.theta = dist_theta(gen);
  }
}

void ParticleFilter::dataAssociation(
    std::map<int, Map::single_landmark_s> predicted,
    vector<LandmarkObs> &observations) {
  for (LandmarkObs &obs : observations) {
    double minDist = std::numeric_limits<double>::max();
    int minId = 0;
    for (auto pred : predicted) {
      double calcDist = dist(obs.x, obs.y, pred.second.x, pred.second.y);
      if (calcDist < minDist) {
        minDist = calcDist;
        minId = pred.first;
      }
    }

    obs.id = minId;
  }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[],
                                   const vector<LandmarkObs> &observations,
                                   const Map &map_landmarks) {
  double weightSum = 0;
  for (Particle &p : particles) {
    vector<LandmarkObs> observationsTransformed;
    LandmarkObs newObs;
    for (LandmarkObs l : observations) {
      newObs.id = l.id;
      newObs.x = p.x + (cos(p.theta) * l.x) - (sin(p.theta) * l.y);
      newObs.y = p.y + (sin(p.theta) * l.x) + (cos(p.theta) * l.y);
      observationsTransformed.push_back(newObs);
    }

    dataAssociation(map_landmarks.landmark_map, observationsTransformed);
    double newWeight = 1.;

    for (auto obs : observationsTransformed) {
      newWeight *=
          multivariateGaussian(map_landmarks.landmark_map.at(obs.id).x,
                               map_landmarks.landmark_map.at(obs.id).y, obs.x,
                               obs.y, std_landmark[0], std_landmark[1]);
    }
    p.weight = newWeight;
    weightSum += newWeight;
  }

  for (Particle &p : particles) {
    p.weight /= weightSum;
  }
}

void ParticleFilter::resample() {
  std::vector<Particle> particlesResampled;
  std::default_random_engine gen;

  std::vector<double> weights;
  for (auto p : particles) {
    weights.push_back(p.weight);
  }

  std::discrete_distribution<> d(weights.begin(), weights.end());

  for (int i = 0; i < num_particles; ++i) {
    particlesResampled.push_back(particles[d(gen)]);
  }

  particles = particlesResampled;
}

void ParticleFilter::SetAssociations(Particle &particle,
                                     const vector<int> &associations,
                                     const vector<double> &sense_x,
                                     const vector<double> &sense_y) {
  // particle: the particle to which assign each listed association,
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations = associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length() - 1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length() - 1);  // get rid of the trailing space
  return s;
}