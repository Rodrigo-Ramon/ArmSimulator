#include "motor.h"
#include "CAN_BUS_HCPS.h"
#include "kinematics.h"
#include <stdio.h>
#include <stdlib.h>


// Constructor
Motor::Motor(int can_address)
  : can_address_(can_address) {}

// Getter methods
float Motor::get_position() const {
  return position_;
}

float Motor::get_velocity() const {
  return velocity_;
}

float Motor::get_torque() const {
  return torque_;
}

int Motor::get_can_address() const {
  return can_address_;
}

// Control methods
float Motor::read_position() 
{

    CAN_BUS_Send(get_position_msg, can_address_);

    usleep(readDelay);
    
    can_reply = CAN_BUS_Receive(can_address_, get_position_msg[0]);
    // printf("Checking- %X, ", get_position_msg[0]);
    // printf("%X, ", (0X0F & can_address_)); 
    if (can_reply != NULL)

    {
        
        memmove(rec_buf,can_reply,9);
        
        // printf("Checking- %X, ", rec_buf[1]);
        // printf("%X, ", get_position_msg[0]);
        // printf("%X, ", rec_buf[0]);
        // printf("%X\n", (0X0F & can_address_));  

        if ( rec_buf[1] == get_position_msg[0] &&  rec_buf[0] == (0X0F & can_address_) )
        {
            position_ =  (rec_buf[5] + ( rec_buf[6] << 8 ) + ( rec_buf[7] << 16 ) + (rec_buf[8] << 24)) * 0.01;
        }
        return position_;
    }
    return position_;

}

void Motor::set_position(float new_position) 
{
    if (in_range(new_position))
    {
      
      //printf("Original- %f2.0, %X ", new_position, new_position);

      u_int32_t new_position_scaled = new_position * position_scale;

      //printf("Scaled - %d, %X \n", new_position_scaled, new_position_scaled);

      u_int16_t maxSpeed = 10;

      char send_buf[9] = {0};

      memmove(send_buf,set_position_msg,8);

      send_buf[0] = { set_position_msg[0] };
      send_buf[1] = { 0x00 };
      send_buf[2] = {  (u_int8_t) maxSpeed  & 0xFF };
      send_buf[3] = {   maxSpeed >> 8 & 0xFF };
      send_buf[4] = {   (u_int8_t)new_position_scaled };
      send_buf[5] = {   new_position_scaled >> 8 };
      send_buf[6] = {   new_position_scaled >> 16 };
      send_buf[7] = {   new_position_scaled >> 24 };
      
      // printf("4- %d, %X \n", send_buf[4], send_buf[4]);
      // printf("5- %d, %X \n", send_buf[5], send_buf[5]);
      // printf("6- %d, %X \n", send_buf[6], send_buf[6]);
      // printf("7- %d, %X \n", send_buf[7], send_buf[7]);

      // for (int i=0;i<8;i++)
      // {
      //   printf("%d, %X ", i, send_buf[i]);
      // }

      // printf("\n");

      CAN_BUS_Send(send_buf, this->can_address_);

      usleep(readDelay);
    
      can_reply = CAN_BUS_Receive(this->can_address_, set_position_msg[0]);

      
      }
    else
    {

    }
    

}

void Motor::shutdown_cmd() 
{

    CAN_BUS_Send(shutdown_msg, can_address_);

    //usleep(readDelay);  // May be able to remove this??
    
    can_reply = CAN_BUS_Receive(can_address_, get_position_msg[0]); // May be able to remove this??
}

bool Motor::in_range()
{
  return ( this->position_ >= this->min_position && this->position_ <= this->max_position);
}

bool Motor::in_range(float position_check) 
{
  if (position_check < min_position) {
    return false;
  }
  else if (position_check > max_position) {
    return false;
  }
  else
  {
    //printf("inRange");
    return true;
  }
}

void Motor::set_velocity(float new_velocity) 
{
  velocity_ = new_velocity;
}

void Motor::set_torque(float new_torque) 
{
  torque_ = new_torque;
}

void Motor::set_can_address(int new_address)
{
  can_address_ = new_address;
}

void Motor::set_zero_cmd ()
{
    CAN_BUS_Send(set_zero_pos_msg, can_address_);
    
    //usleep(readDelay);
    
    can_reply = CAN_BUS_Receive(can_address_, set_zero_pos_msg[0]);

}


/* bool move_to_zero_cmd(int Send_Can_BUS_addr)
{
    CAN_BUS_Send(zero_pos_msg, Send_Can_BUS_addr);
    //printf("ZERO-%X",Send_Can_BUS_addr);
    
    can_reply = CAN_BUS_Receive(Send_Can_BUS_addr,zero_pos_msg[0]);

    if (can_reply != NULL)

    {
        memmove(rec_buf,can_reply,9);
        if ( rec_buf[1] == 0xA3 &&  rec_buf[0] == (0X0F & Send_Can_BUS_addr) )
        {
            printf("TRUE");
            return true;
        }

    
    }
    return false;
}

void set_zero_cmd (int Send_Can_BUS_addr)
{
    CAN_BUS_Send(set_zero_pos_msg, Send_Can_BUS_addr);
    
    usleep(readDelay);
    
    can_reply = CAN_BUS_Receive(Send_Can_BUS_addr, set_zero_pos_msg[0]);

    
}

void shutdown_cmd (int Send_Can_BUS_addr)
{
    CAN_BUS_Send(shutdown_msg, Send_Can_BUS_addr);

    usleep(readDelay);
    
    can_reply = CAN_BUS_Receive(Send_Can_BUS_addr, get_position_msg[0]);


}
 */