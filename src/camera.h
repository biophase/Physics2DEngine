#pragma once

#include<SDL.h>
#include<vector>
#include<Eigen/Dense>

#include "camera.h"



class Camera : Entity{
    public:
        Camera(Entity* target);
        void update(float delta_t);
        static std::vector<Camera*> camera_instances;
        eg::Vector2f transform_world_to_camera(eg::Vector2f);

    private:
        Entity* m_target;

};