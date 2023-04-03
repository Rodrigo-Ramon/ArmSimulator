class Motor {
  private:
    const static int maxSpeed = 1;
    const static int max_position = 90;
    const static int min_position = -90;
    const static int position_scale = 100;
    float position_;
    float velocity_;
    float torque_;
    int can_address_;

  public:
    // Constructor
    Motor(int can_address = 0);

    // Getter methods
    float get_position() const;
    float get_velocity() const;
    float get_torque() const;
    int get_can_address() const;


    // Control methods
    void read_position();                                //Gets the position from the motor
    void set_zero();                                    //Sets Encoder-Zero to Current Position
    bool in_range();
    bool in_range(float position_check);
    void shutdown_cmd();
    void set_position(float new_position);
    void set_velocity(float new_velocity);
    void set_torque(float new_torque);
    void set_can_address(int new_address);
};