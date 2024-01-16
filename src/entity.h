#pragma once
// #include "game.h"
#include<SDL.h>
#include<vector>
#include<Eigen/Dense>

#include<memory>


namespace eg = Eigen;


class BVHLeaf;
class Camera;
class ParticleEmittor;

class Entity{
    public:
        static std::vector<Entity*> entity_instances;
        bool m_alive = true;
        float m_pos_x;
        float m_pos_y;
        float m_vel_x;
        float m_vel_y;

        // representation
        std::vector<eg::Vector2f> m_repr_global;
        
        // behaviour
        float m_fric;
        
        // rotation
        float m_rot_angle;
        float m_rot_angle_vel;
        float m_rot_angle_fric;

        
        

        std::array<Uint8, 4> color;

    public: // methods
        Entity(float x, float y);
        virtual ~Entity(){};
        static void cleanUp();
        virtual void update(float delta_time);
        virtual void draw(SDL_Renderer* renderer, Camera& camera);
        void setRepresentation(std::vector<eg::Vector2f> vertices);
        void setRepresentation(std::string file_path);
        void apply_force (const eg::Vector2f &impulse_dir){
            this->m_vel_x += impulse_dir(0,0);
            this->m_vel_y += impulse_dir(1,0);
        }

        
        

        // eg::Vector2f repr_vertex[5];
    protected:
        std::vector<eg::Vector2f> m_repr_local;

    
};

class Collidable : public Entity{
    private:
        float m_mass;
        eg::Vector2f m_center_of_mass;
        // BVHLeaf* m_bvh_leaf;
        std::shared_ptr<BVHLeaf> m_bvh_leaf;
    public:
        bool m_temp_colliding = false;
        static std::vector<Collidable*> collidable_instances;
        float collision_radius;

    // crate an instance by reading vertices from a .txt file
    Collidable (float x, float y, std::string repr_filepath);
    // Create an instance with a rectangular shape
    Collidable (float x, float y, float w, float b);
    // draw method for a collidable
    void draw(SDL_Renderer* renderer, Camera& camera);
    // update method for a collidable
    void update(float delta_time);
    // support function for the GJK collision detection. Only call after update()
    std::unique_ptr<eg::Vector3d> support_func(eg::Vector3d direction);

        
        
}; 

