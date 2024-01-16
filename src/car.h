#pragma once

// #include "game.h"



#include<SDL.h>
#include<vector>
#include<Eigen/Dense>



class ParticleEmittor;
class Car: public Collidable{
    public:
        static std::vector<Car*> car_instances;
        Player* controlled_by;
        
        // engine properties
        float m_engine_torque;
        float m_engine_toruqe_acc;
        float m_engine_torque_fric;
        
        // steering
        float m_rot_angle_vel_acc;
        float m_rot_min_vel;
        bool force_skid;

        // car properties
        float m_skid_fric;
        float m_skid_thresh;
        float m_skid_thresh_regular;
        float m_skid_thresh_break;
        float m_break_factor;

        // abilities
        bool m_gas_active;
        bool m_left_turn_active;
        bool m_right_turn_active;
        bool m_rear_active;
        bool m_breaking_active;
        bool m_nitro_active;

        // emittors
        ParticleEmittor* m_smoke_emittor = nullptr;

        void process_input(const Uint8* keystate);

        void update(float delta_time);

        Car (Player* player, float x, float y, std::string repr_filepath);


}; 

