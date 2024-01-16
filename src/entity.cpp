// #include "game.h"
#include <SDL.h>
#include <vector>
#include <Eigen/Dense>

#include "constants.h"
#include "utils.h"

#include "entity.h"
#include "camera.h"
#include "collision.h"
#include "particle.h"

Entity::Entity(float x, float y){
    // id = guid_manager->get_next_free_id();
    entity_instances.push_back(this);
    this->m_pos_x = x;
    this->m_pos_y = y;
    this->m_vel_x = 0;
    this->m_vel_y = 0;
    this->m_fric = 0.96;

    this->m_rot_angle = 0;
    this->m_rot_angle_vel = 0;
    this->m_rot_angle_fric = .935;

    // default repr
    this->m_repr_local.push_back(eg::Vector2f(-18,0));
    this->m_repr_local.push_back(eg::Vector2f(+18,0));
    this->m_repr_local.push_back(eg::Vector2f(0,0));
    this->m_repr_local.push_back(eg::Vector2f(0,+18));
    this->m_repr_local.push_back(eg::Vector2f(0,-18));

    m_repr_global = m_repr_local;
    this->color = {DEFAULT_COLOR}; 

}

void Entity::cleanUp(){
     for (auto it = Entity::entity_instances.begin(); it != Entity::entity_instances.end();){
        if (!(*(it))->m_alive){
            delete *it;
            it = Entity::entity_instances.erase(it);
        }
        else{
            it ++;
        }
     }
}

// set representation by directly supplying vertices
void Entity::setRepresentation(std::vector<eg::Vector2f> vertices){
    this->m_repr_local = vertices;
}

// set representation by reading vertices from a .txt file
void Entity::setRepresentation(std::string file_path){
    m_repr_local.clear();
    Utils::read_txt_into_vectorlist(m_repr_local, file_path);
}

void Entity::update(float delta_time){
    
    // update position
    this->m_pos_x += delta_time * this->m_vel_x;
    this->m_pos_y += delta_time * this->m_vel_y;

    this->m_vel_x *= pow(m_fric, delta_time);
    this->m_vel_y *= pow(m_fric, delta_time);
    
    
    // update vertices
    // rotation definition
    eg::Vector2f rot_1;    
    rot_1 << 1 , 0;
    Utils::rotate_vec_by_angle(this->m_rot_angle, rot_1);
    

    eg::Matrix<float,2,2> perp_rot;
    perp_rot << +0 , -1,
                +1 , +0;

    eg::Vector2f rot_2 = perp_rot * rot_1;
    eg::Matrix<float, 2, 2> rot_matrix;
    
    rot_matrix <<
        rot_1(0,0), rot_2(0,0),
        rot_1(1,0), rot_2(1,0);

    m_repr_global = m_repr_local; // copy non-transformed repr
    
    // transform repr
    Utils::rotate_vec_by_angle(m_rot_angle, m_repr_global);
    for (int i=0; i<= m_repr_local.size()-1; i++){
        // m_repr_global[i] = rot_matrix*m_repr_global[i]; // orientation
        m_repr_global[i](0,0) += this->m_pos_x; // translation x
        m_repr_global[i](1,0) += this->m_pos_y; // translation y

    }
}

void Entity::draw(SDL_Renderer *renderer, Camera& camera){


    std::array<Uint8,4>& c = color;
    SDL_SetRenderDrawColor(renderer,c[0],c[1],c[2],10);

    // create repr in camera coordinates
    auto draw_repr = m_repr_global; 
     
    for (int i=0; i<= m_repr_local.size()-1; i++){
        draw_repr[i] = camera.transform_world_to_camera(draw_repr[i]);
    }


    SDL_FPoint sdl_fpoints[m_repr_local.size()];
    Utils::vecs_to_points(draw_repr, sdl_fpoints, m_repr_local.size());
    SDL_RenderDrawLinesF(renderer,sdl_fpoints, m_repr_local.size());
    SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);       

}

void Collidable::draw(SDL_Renderer* renderer, Camera& camera){
    Entity::draw(renderer, camera);

#ifdef DEBUG_GRAPHICS
    this->m_bvh_leaf->m_aabb->draw(renderer, camera);
    this->m_bvh_leaf->m_aabb->m_colliding = false;
    SDL_FPoint sdl_fpoints[2];
    std::vector<eg::Vector2f> direction = {
        eg::Vector2f(m_pos_x, m_pos_y),
        eg::Vector2f(m_pos_x + m_vel_x, m_pos_y + m_vel_y)
    };
    for ( auto& d: direction){
        d = camera.transform_world_to_camera(d);
    }
    Utils::vecs_to_points(direction,sdl_fpoints, 2);

    // Utils::vecs_to_points(&camera.transform_world_to_camera(eg::Vector2f(m_pos_x, m_pos_y)),sdl_fpoints,1);
    // Utils::vecs_to_points(&camera.transform_world_to_camera(eg::Vector2f(m_pos_x, m_pos_y)),sdl_fpoints+1,1);
    SDL_SetRenderDrawColor(renderer, DEBUG_VEC_COLOR);
    SDL_RenderDrawLinesF(renderer, sdl_fpoints,2);
    SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);

    if (m_temp_colliding){
        SDL_SetRenderDrawColor(renderer, DEBUG_AABB_COLLIDING_COLOR);
        std::vector<eg::Vector2f> points_cross_collision_1 = {
            eg::Vector2f(m_pos_x -50, m_pos_y -50),
            eg::Vector2f(m_pos_x +50, m_pos_y +50),
        };
        for ( auto& p: points_cross_collision_1){
            p = camera.transform_world_to_camera(p);
        }
        SDL_FPoint sdl_fpoints_cross_1[2];
        Utils::vecs_to_points(points_cross_collision_1,sdl_fpoints_cross_1,2);
        SDL_RenderDrawLinesF(renderer, sdl_fpoints_cross_1, 2);
        // ----
        std::vector<eg::Vector2f> points_cross_collision_2 = {
            eg::Vector2f(m_pos_x -50, m_pos_y +50),
            eg::Vector2f(m_pos_x +50, m_pos_y -50),
        };
        for ( auto& p: points_cross_collision_2){
            p = camera.transform_world_to_camera(p);
        }
        SDL_FPoint sdl_fpoints_cross_2[2];
        Utils::vecs_to_points(points_cross_collision_2,sdl_fpoints_cross_2,2);
        SDL_RenderDrawLinesF(renderer, sdl_fpoints_cross_2, 2);
        SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);
        m_temp_colliding = false;
    
    }
#endif

}

void Collidable::update(float delta_time) {
    Entity::update(delta_time);
    this->m_bvh_leaf->m_aabb->setAABBaround(this->m_repr_global);
}

// create an instance with an arbitrary shape read from a .txt file 
Collidable::Collidable (float x, float y, std::string repr_filepath): Entity(x, y){
    collidable_instances.push_back(this);
    this->setRepresentation(repr_filepath);
    this->m_bvh_leaf = std::make_shared<BVHLeaf>(this);
  
}

// Create an instance with a rectangular shape
Collidable::Collidable (float x, float y, float w, float h): Entity(x, y){
    collidable_instances.push_back(this);
    std::vector<eg::Vector2f> repr;
    repr.push_back(eg::Vector2f(-w/2, -h/2));
    repr.push_back(eg::Vector2f(-w/2, +h/2));
    repr.push_back(eg::Vector2f(+w/2, +h/2));
    repr.push_back(eg::Vector2f(+w/2, -h/2));
    repr.push_back(eg::Vector2f(-w/2, -h/2));
    this->setRepresentation(repr);
    this->m_bvh_leaf = std::make_shared<BVHLeaf>(this);
    this->m_mass = w * h;
    this->m_center_of_mass << 0, 0;

}

// only call after update()
std::unique_ptr<eg::Vector3d> Collidable::support_func(eg::Vector3d direction){
    // TODO: this reuses global repr --> make sure to call after update has already been called this frame
    float max_dot = -std::numeric_limits<float>::infinity();
    int32_t max_ind = -1;
    // for(const auto& vertex : this->m_repr_global)
    for(size_t i = 0; i < m_repr_global.size();++i)
    {
        eg::Vector3d repr_vertex_3f;
        // TODO: check whether the unqiue pointer can't already be allocated here. Is a call to "NEW" that expensive?
        repr_vertex_3f << (double)m_repr_global[i][0]-(double)m_pos_x,(double)m_repr_global[i][1]-(double)m_pos_y ,0; // TODO: subtraction can be avoided?
        float dot = repr_vertex_3f.dot(direction);
        // std::cout << "Dot product in support was " << dot << std::endl;
        if (dot > max_dot) {
            max_dot = dot;
            max_ind = (int32_t)i;
        }
    }
    // std::cout << "Called support func" << std::endl;
    if (max_ind == -1){
        std::cout << "Error" << std::endl;
    }
    std::unique_ptr<eg::Vector3d> vertex = std::make_unique<eg::Vector3d>();
    *vertex << m_repr_global.at(max_ind).cast<double>(), 0;
    return vertex;
}




