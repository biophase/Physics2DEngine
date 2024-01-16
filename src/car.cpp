
#include<SDL.h>
#include<vector>
// #include<math.h>
#include<Eigen/Dense>


#include "constants.h"
#include "entity.h"
#include "player.h"
#include "utils.h"

#include "car.h"
#include "particle.h"


void Car::process_input(const Uint8* keystate){

    
    

    // DIRECTIONS
    // Rear
    if (keystate[controlled_by->controls.key_down]) m_rear_active = true;
    else m_rear_active = false;

    // Forward
    if (keystate[controlled_by->controls.key_up]) m_gas_active = true;
    else m_gas_active = false;
    
    // KEY PRESS LEFT
    if (keystate[controlled_by->controls.key_left]) m_left_turn_active = true;
    else m_left_turn_active = false;

    // KEY PRESS RIGHT
    if (keystate[controlled_by->controls.key_right]) m_right_turn_active = true; 
    else m_right_turn_active = false;

    // ABILITIES
    // boost
    if (keystate[controlled_by->controls.boost]){
        this->m_nitro_active = true;
        std::cout << "boosting " << m_smoke_emittor << std::endl;
        if (m_smoke_emittor == nullptr) {
            
            m_smoke_emittor = new ParticleEmittor(m_pos_x,m_pos_y,0.06,
            [](
                float pos_x,
                float pos_y,
                float particle_vel_x,
                float particle_vel_y,
                float particle_rot_angle){
                    ParticleSmoke* particle = new ParticleSmoke(pos_x, pos_y,1,30,0.8);
                    particle->m_rot_angle = particle_rot_angle;
                    particle->m_vel_x = particle_vel_x;
                    particle->m_vel_y = particle_vel_y;
                    particle->m_rot_angle_vel=3;
                    particle->m_fric=.05;
                    particle->color={235, 152, 52, 255};
                    // particle->m_rot_angle_vel=0.001;
                    // particle->m_rot_angle_fric = .95;
                    std::cout << "Smoke LAMBDA called " << std::endl;
                });
                std::cout << "Creating a new particle emittor" << std::endl;
                }
    }
    else {
        this->m_nitro_active = false;
        delete m_smoke_emittor;
        m_smoke_emittor = nullptr;
        }
    // breaking
    if (keystate[controlled_by->controls.apply_break]) this->m_breaking_active = true;
    else this->m_breaking_active = false;

    // for debugging, e.g. entity spawning
    if (keystate[controlled_by->controls.debug_action]) {
        auto temp = new Collidable(m_pos_x, m_pos_y,50,50);
        temp->m_vel_x = this->m_vel_x*1.5;
        temp->m_vel_y = this->m_vel_y*1.5;
        temp->m_rot_angle = this->m_rot_angle;
        temp->m_fric = 0.3;

    }
}



void Car::update(float delta_time){
    Collidable::update(delta_time);
    Entity::update(delta_time); // FIXME: this is redundant
    float delta_time_40 = 40.f * delta_time; // such that a factor of 1 at 60 FPS
    // temp
    float car_velocity = sqrt(pow(this->m_vel_x,2)+pow(this->m_vel_y,2));
    int travel_forward = 1;
    if (this->m_engine_torque <0) travel_forward = -1;

    // steering and controls
    if (m_rear_active) this->m_engine_torque -= m_engine_toruqe_acc * delta_time_40;
    if (m_gas_active)  this->m_engine_torque += m_engine_toruqe_acc * delta_time_40;
    if (m_left_turn_active){
        if (car_velocity > this->m_rot_min_vel) this->m_rot_angle_vel -= this->m_rot_angle_vel_acc * travel_forward * delta_time;
        else this->m_rot_angle_vel -= this->m_rot_angle_vel_acc * (car_velocity/this->m_rot_min_vel) * travel_forward * delta_time;
    }
    if (m_right_turn_active){
        if (car_velocity > this->m_rot_min_vel) this->m_rot_angle_vel += this->m_rot_angle_vel_acc * travel_forward * delta_time;
        else this->m_rot_angle_vel += this->m_rot_angle_vel_acc * (car_velocity/this->m_rot_min_vel) * travel_forward * delta_time;
    }



    
    // abilities
    // breaking
    // std::cout << "Engine Torque "<< this->m_engine_torque << std::endl;
    if (m_breaking_active) {
            this->m_engine_torque *= pow(m_break_factor,delta_time_40);
            this->m_skid_thresh = this->m_skid_thresh_break;
            float breaking_lin = 5;
            if (this->m_engine_torque > breaking_lin*delta_time){
                this->m_engine_torque -= breaking_lin*delta_time;
            }
    }
    else this->m_skid_thresh = this->m_skid_thresh_regular;
    // boosting
    
    if (m_nitro_active){
        this->m_engine_torque *= pow(1.025, delta_time_40);
        this->m_engine_torque += 17*delta_time;

        if (m_smoke_emittor != nullptr){
            std::cout << "Particle Emittor should be emitting" << std::endl;
            m_smoke_emittor->updateSpawn(delta_time);
            m_smoke_emittor->m_pos_x = this->m_pos_x;
            m_smoke_emittor->m_pos_y = this->m_pos_y;
            m_smoke_emittor->m_particle_vel_x = -this->m_vel_x/ sqrt(pow(m_vel_x,2) + pow(m_vel_y,2))*1000;
            m_smoke_emittor->m_particle_vel_y = -this->m_vel_y/ sqrt(pow(m_vel_x,2) + pow(m_vel_y,2))*1000;
            m_smoke_emittor->m_particle_rot_angle = this->m_rot_angle;
        }
        
    }
        
    // tire rotation
    this->m_rot_angle += this->m_rot_angle_vel*delta_time_40;
    this->m_rot_angle_vel *= pow(this->m_rot_angle_fric, delta_time_40);
       
    // forward (engine) force
    
    this->m_engine_torque *= pow(this->m_engine_torque_fric, delta_time_40);
    
    // calculate initial impules
    eg::Vector2f force; force << m_engine_torque, 0;
    Utils::rotate_vec_by_angle(this->m_rot_angle, force);

    // handle lateral forces
    eg::Vector2f car_lateral_dir; 
    car_lateral_dir << 1 , 0;
    Utils::rotate_vec_by_angle(this->m_rot_angle, car_lateral_dir);
    Utils::rotate_vec_90_CCW(car_lateral_dir);
    eg::Vector2f car_vel_dir;
    car_vel_dir << this->m_vel_x, this->m_vel_y;
    eg::Vector2f force_lateral_component;
    force_lateral_component = (car_vel_dir.dot(car_lateral_dir))/(car_lateral_dir.dot(car_lateral_dir))*car_lateral_dir;
    

    // std::cout << "velocity: " << car_vel_dir.norm() << std::endl;
    if (force_lateral_component.norm() < this->m_skid_thresh){
        // skid = False
        force -= force_lateral_component;
    }
    else{
        // skid = True
        force -= force_lateral_component * this->m_skid_fric;
    }
    


    // apply friction
    this->m_vel_x *= pow(this->m_fric, delta_time_40);
    this->m_vel_y *= pow(this->m_fric, delta_time_40);

    // apply impulse
    this->apply_force(force * delta_time_40);
}

Car::Car (Player* player, float x, float y, std::string repr_filepath) : Collidable(x, y, repr_filepath){

    // this->m_acc = acc;

    this->controlled_by = player;
    car_instances.push_back(this);

    // default car properties

    // engine properties
    m_engine_torque = 0;
    m_engine_toruqe_acc = 0.7;
    m_engine_torque_fric = 0.935;
    
    
    // steering
    m_rot_angle_vel_acc = 0.18;
    m_rot_angle_fric = .935;    
    m_rot_min_vel = 200.;
    force_skid = false;

    // car properties
    m_skid_fric = 0.025;
    m_skid_thresh = 16.;
    m_skid_thresh_regular= 16.;
    m_skid_thresh_break= 8.;
    m_break_factor = 0.85;

    // abilities
    m_left_turn_active = false;
    m_right_turn_active = false;
    m_rear_active = false;
    m_breaking_active = false;
    m_nitro_active = false;
    // TODO: Car code
    this->color = {255,50,50,10};
}