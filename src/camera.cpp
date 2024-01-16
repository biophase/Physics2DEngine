#include<iostream>
#include "entity.h"
#include "camera.h"
#include "constants.h"

Camera::Camera(Entity* target) : Entity(target->m_pos_x, target->m_pos_y) {
    m_target=target;
    camera_instances.push_back(this);
    m_fric = 0.1;
}

void Camera::update(float delta_time){
    
    float acc = 2;

    eg::Vector2f target_dir(m_target->m_pos_x - this->m_pos_x, m_target->m_pos_y - this->m_pos_y);
    this->m_vel_x = target_dir[0] * acc;
    this->m_vel_y = target_dir[1] * acc;

    Entity::update(delta_time);
}
eg::Vector2f Camera::transform_world_to_camera(eg::Vector2f point){
    
    // float camera_vel = sqrt(pow(this->m_target->m_vel_x,2) + pow(this->m_target->m_vel_y,2));
    float camera_vel = sqrt(pow(this->m_vel_x,2) + pow(this->m_vel_y,2));
    

    // zoom effect
    float max_factor = 0.8;
    float min_factor = 0.1;
    
    float max_vel = 500;
    if (camera_vel > max_vel) camera_vel = max_vel;

    float factor = max_factor + (max_vel - camera_vel)*(max_factor-min_factor)/max_vel;

    point[0] -= this->m_pos_x;
    point[1] -= this->m_pos_y;

    point[0] *= factor;
    point[1] *= factor;

    point[0] += ((float)WINDOW_WIDTH)/2;
    point[1] += ((float)WINDOW_HEIGHT)/2;
    return point;
}



