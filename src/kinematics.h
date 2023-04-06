// Kinematics.h - header file for Kinematics library

#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <cmath>
#include <stdio.h>

class Kinematics {
  private:
    float link1_length_;
    float link2_length_;

  public:
    // Constructor
    Kinematics(float link1_length, float link2_length);

    // Forward kinematics method
    void forward_kinematics(float joint1_angle, float joint2_angle, float& x, float& y);

    // Inverse kinematics method
    bool inverse_kinematics(float x, float y, float joint1_angle_prev, float joint2_angle_prev, float& joint1_angle, float& joint2_angle);
};

#endif // KINEMATICS_H