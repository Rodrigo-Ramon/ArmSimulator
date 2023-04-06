// Kinematics.cpp - implementation file for Kinematics library

#include "kinematics.h"

// Constructor
Kinematics::Kinematics(float link1_length, float link2_length)
  : link1_length_(link1_length), link2_length_(link2_length) {}

// Forward kinematics method
void Kinematics::forward_kinematics(float joint1_angle, float joint2_angle, float& x, float& y) {
  // Convert angles to radians
  float theta1 = joint1_angle * M_PI / 180.0f;
  float theta2 = joint2_angle * M_PI / 180.0f;

  // Calculate x and y coordinates
  x = link1_length_ * cos(theta1) + link2_length_ * cos(theta1 + theta2);
  y = link1_length_ * sin(theta1) + link2_length_ * sin(theta1 + theta2);
}

// Inverse kinematics method
bool Kinematics::inverse_kinematics(float x, float y, float joint1_angle_prev, float joint2_angle_prev, float& joint1_angle, float& joint2_angle) {
  // Calculate intermediate variables
  float c2 = (pow(x, 2) + pow(y, 2) - pow(link1_length_, 2) - pow(link2_length_, 2)) / (2 * link1_length_ * link2_length_);
  if (c2 < -1.0f || c2 > 1.0f) {
    //printf("NoGo");
    return false;
  }
  float s2 = sqrt(1 - pow(c2, 2));
  float theta2 = atan2(s2, c2);
  float k1 = link1_length_ + link2_length_ * c2;
  float k2 = link2_length_ * s2;
  float theta1 = atan2(y, x) - atan2(k2, k1);

  // Check joint limits
  if (theta1 < 0 || theta1 > 45 || theta2 < 0 || theta2 > 45) {
      // Invalid joint angles
      printf("NoJoint");
      return false;
    }

  // Apply joint velocity limits
  float delta1 = joint1_angle - joint1_angle_prev;
  float delta2 = joint2_angle - joint2_angle_prev;
  float max_delta = M_PI / 60.0f; // maximum joint velocity of 6 degrees per second
  if (fabs(delta1) > max_delta) {
    delta1 = copysign(max_delta, delta1);
  }
  if (fabs(delta2) > max_delta) {
    delta2 = copysign(max_delta, delta2);
  }

  // Update joint angles
  joint1_angle = joint1_angle_prev + delta1;
  joint2_angle = joint2_angle_prev + delta2;

  return true;
}