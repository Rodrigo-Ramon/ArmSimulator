// Kinematics.h - header file for Kinematics library

#include "Motor.h"
#include <cmath>

class Kinematics {
  private:
    Motor joint1_;
    Motor joint2_;
    float link1_length_;
    float link2_length_;

  public:
    // Constructor
    Kinematics(float link1_length, float link2_length, int joint1_can_address, int joint2_can_address);

    // Forward kinematics method
    void calculate_forward_kinematics(float joint1_angle, float joint2_angle, float& x, float& y);

    // Inverse kinematics method
    bool calculate_inverse_kinematics(float x, float y, float joint1_angle, float joint2_angle, float& new_joint1_angle, float& new_joint2_angle);
};