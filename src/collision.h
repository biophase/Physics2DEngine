#pragma once

#include<Eigen/Dense>
#include<SDL.h>
#include<vector>
#include<memory>
#include<utility>

#include "constants.h"
#include "entity.h"


namespace eg = Eigen;


class Collidable;
class Camera;


class AABB{
    public:
        float m_start_x;
        float m_end_x;
        float m_start_y;
        float m_end_y;
        // create a BBox around a list of vertices
        AABB(std::vector<eg::Vector2f> vertices);
        void setAABBaround(std::vector<eg::Vector2f> vertices);

        #ifdef DEBUG_GRAPHICS
        
        bool m_colliding;
        void draw(SDL_Renderer* renderer, Camera& camera);
        #endif

        static std::unique_ptr<AABB> getAABB(std::vector<eg::Vector2f> vertices);

};

class BVHTree{
    public:

        std::vector<BVHLeaf*>* m_bvhLeaf_instances;
        BVHTree(std::vector<BVHLeaf*>* bvhLeaf_instances);
        std::unique_ptr<std::vector<std::pair<BVHLeaf*, BVHLeaf*>>>getCandidatePairs();
        void buildTree(std::vector<AABB>);


};

struct GJK_return{
    bool colliding;
    std::unique_ptr<std::vector<eg::Vector3d>> termination_simplex;
};

struct EPA_return{
    eg::Vector2f penetration_vector;
    float penetration_depth; // could be deduced from penetration_vector's magnitude?
};


bool        collision_test_AABB_AABB(const std::pair<BVHLeaf*, BVHLeaf*>& pair);
GJK_return  collision_test_GJK(const std::pair<BVHLeaf*, BVHLeaf*>& pair);
EPA_return  collision_test_EPA(std::unique_ptr<std::vector<eg::Vector3d>> simplex, const std::pair<BVHLeaf*, BVHLeaf*>& pair);

class BVHNode{
    public:
        AABB* m_child_left;
        AABB* m_child_right;
};

struct PolyEdge{
    float distance;
    eg::Vector3d normal;
    size_t edge_start_ind;
};

class BVHLeaf{
    public:
        Collidable* m_collidable;
        AABB* m_aabb;
        

        BVHLeaf(Collidable* collidable);

        static std::vector<BVHLeaf*> bvhLeaf_instances;
        
};





