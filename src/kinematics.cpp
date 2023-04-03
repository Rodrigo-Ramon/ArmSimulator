// Kinematics.cpp - implementation file for Kinematics library

// Constructor
Kinematics::Kinematics(float link1_length, float link2_length, int joint1_can_address, int joint2_can_address)
  : joint1_(joint1_can_address), joint2_(joint2_can_address), link1_length_(link1_length), link2_length_(link2_length) {}

// Forward kinematics method
void Kinematics::calculate_forward_kinematics(float joint1_angle, float joint2_angle, float& x, float& y) {
  // Calculate joint positions
  float joint1_x = link1_length_ * std::cos(joint1_angle);
  float joint1_y = link1_length_ * std::sin(joint1_angle);
  float joint2_x = joint1_x + link2_length_ * std::cos(joint1_angle + joint2_angle);
  float joint2_y = joint1_y + link2_length_ * std::sin(joint1_angle + joint2_angle);

  // Set joint positions in motors
  joint1_.set_position(joint1_angle);
  joint2_.set_position(joint2_angle);

  // Set end effector position
  x = joint2_x;
  y = joint2_y;
}

// Inverse kinematics method
bool Kinematics::calculate_inverse_kinematics(float x, float y, float joint1_angle, float joint2_angle, float& new_joint1_angle, float& new_joint2_angle) {
  // Calculate intermediate values
  float r_squared = x * x + y * y;
  float cos_theta2 = (r_squared - link1_length_ * link1_length_ - link2_length_ * link2_length_) / (2.0f * link1_length_ * link2_length_);
  
  // Check if the desired point is reachable
  if (std::abs(cos_theta2) > 1.0f) {
    // Desired point is not reachable
    return false;
  }

  // Calculate joint angles
  float sin_theta2 = std::sqrt(1.0f - cos_theta2 * cos_theta2);
  new_joint2_angle = std::atan2(sin_theta2, cos_theta2);
  float k1 = link1_length_ + link2_length_ * cos_theta2;
  float k2 = link2_length_ * sin_theta2;
  float atan2_arg = y * k1 - x * k2;
  new_joint1_angle = std::atan2(atan2_arg, x * k1 + y * k2);

  // Set joint angles in motors
  joint1_.set_position(new_joint1_angle);
  joint2_.set_position(new_joint1_angle + new_joint2_angle);

  // Inverse kinematics calculation succeeded
  return true;
}