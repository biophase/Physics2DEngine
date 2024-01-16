#include "collision.h"
#include "camera.h"
#include "entity.h"
#include <memory>
#include <iostream>

AABB::AABB(std::vector<eg::Vector2f> vertices){
    // init as first vector
    this->setAABBaround(vertices);
#ifdef DEBUG_GRAPHICS
    this->m_colliding = false;
#endif

}
void AABB::setAABBaround(std::vector<eg::Vector2f> vertices){
    this->m_start_x = vertices[0][0];
    this->m_end_x = vertices[0][0];

    this->m_start_y = vertices[0][1];
    this->m_end_y = vertices[0][1];

    // for(size_t i=0; i < vertices.size(); ++i){
        
    // }
    for (const auto& vertex : vertices){
        if (vertex[0] < this->m_start_x) this->m_start_x = vertex[0];
        if (vertex[1] < this->m_start_y) this->m_start_y = vertex[1];
        if (vertex[0] > this->m_end_x) this->m_end_x = vertex[0];
        if (vertex[1] > this->m_end_y) this->m_end_y = vertex[1];
    }
}

std::unique_ptr<AABB> AABB::getAABB(std::vector<eg::Vector2f> vertices){
    return std::make_unique<AABB>(vertices);
    

}

#ifdef DEBUG_GRAPHICS
void AABB::draw(SDL_Renderer* renderer, Camera& camera){
    // set colors
    if(this->m_colliding) SDL_SetRenderDrawColor(renderer, DEBUG_AABB_COLLIDING_COLOR);
    else SDL_SetRenderDrawColor(renderer, DEBUG_AABB_DEFAULT_COLOR);

    eg::Vector2f aabb_start(m_start_x, m_start_y);
    eg::Vector2f aabb_end(m_end_x, m_end_y);
    

    aabb_start = camera.transform_world_to_camera(aabb_start);
    aabb_end = camera.transform_world_to_camera(aabb_end);

    
    SDL_RenderDrawLineF(renderer, aabb_start[0], aabb_start[1], aabb_end[0], aabb_start[1]);
    SDL_RenderDrawLineF(renderer, aabb_end[0], aabb_start[1], aabb_end[0], aabb_end[1]);
    SDL_RenderDrawLineF(renderer, aabb_end[0], aabb_end[1], aabb_start[0], aabb_end[1]);
    SDL_RenderDrawLineF(renderer, aabb_start[0], aabb_end[1], aabb_start[0], aabb_start[1]);
    
    

    SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);
}
#endif

// BVHTree constructor
BVHTree::BVHTree(std::vector<BVHLeaf*>* bvhLeaf_instances): m_bvhLeaf_instances(bvhLeaf_instances){};

std::unique_ptr<std::vector<std::pair<BVHLeaf*, BVHLeaf*>>> BVHTree::getCandidatePairs(){
    std::unique_ptr<std::vector<std::pair<BVHLeaf*, BVHLeaf*>>> pairs = std::make_unique<std::vector<std::pair<BVHLeaf*, BVHLeaf*>>>();
    for (size_t i=0; i < m_bvhLeaf_instances->size(); ++i){
        for (size_t j=i+1; j < m_bvhLeaf_instances->size(); ++j){
            pairs->push_back(std::pair<BVHLeaf*, BVHLeaf*>(m_bvhLeaf_instances->at(i), m_bvhLeaf_instances->at(j)));
        }
    }
    return pairs;
}

BVHLeaf::BVHLeaf(Collidable* collidable){
    bvhLeaf_instances.push_back(this);
    m_collidable = collidable;
    m_aabb = new AABB(collidable->m_repr_global);
    // this->m_aabb = std::make_unique<AABB>(collidable->m_repr_global);
}

bool collision_test_AABB_AABB(const std::pair<BVHLeaf*, BVHLeaf*>& pair){
    bool colliding =  (((pair.second->m_aabb->m_start_x < pair.first->m_aabb->m_end_x) && (pair.second->m_aabb->m_end_x > pair.first->m_aabb->m_start_x)) && // test x dir
                       ((pair.second->m_aabb->m_start_y < pair.first->m_aabb->m_end_y) && (pair.second->m_aabb->m_end_y > pair.first->m_aabb->m_start_y)));  // test y dir
#ifdef DEBUG_GRAPHICS
    if (colliding){
        pair.first->m_aabb->m_colliding = true;
        pair.second->m_aabb->m_colliding = true;
    }
#endif
    return colliding;
}

eg::Vector3d tripleCrossProd(const eg::Vector3d& A, const eg::Vector3d& B, const eg::Vector3d& C){
    return (A.cross(B)).cross(C);
}

bool simplexLineCase(std::vector<eg::Vector3d>& simplex, eg::Vector3d& d){
    const auto& A = simplex.at(1); // A = simplex(1)
    const auto& B = simplex.at(0);// B = simplex(0)
    eg::Vector3d AB = B - A;
    eg::Vector3d AO = -A;
    d = tripleCrossProd(AB,AO,AB);
    d.normalize();
    return false;
}

bool simplexTriangleCase(std::vector<eg::Vector3d>& simplex, eg::Vector3d& d){
    const auto& A = simplex.at(2); // A = simplex(2)
    const auto& B = simplex.at(1); // B = simplex(1)
    const auto& C = simplex.at(0); // C = simplex(0)

    eg::Vector3d AB = B - A; AB.normalize();
    eg::Vector3d AC = C - A; AC.normalize();
    eg::Vector3d AO = -A; AO.normalize();

    eg::Vector3d ABo = tripleCrossProd(AC, AB, AB); ABo.normalize();
    eg::Vector3d ACo = tripleCrossProd(AB, AC, AC); ACo.normalize();


    bool check_AB = (ABo.dot(AO) > 0);
    bool check_AC = (ACo.dot(AO) > 0);

    if (check_AB){
        // origin in region AB
        simplex.erase(simplex.begin());
        d = ABo;
        return false;
    }
    if (check_AC){
        // origin in region AC
        simplex.erase(simplex.begin()+1);
        d = ACo;
        return false;
    }
    return true;
}

bool handleSimplex(std::vector<eg::Vector3d>& simplex, eg::Vector3d& d){
    if (simplex.size() == 2){
        return simplexLineCase(simplex, d);
    }
    return simplexTriangleCase(simplex, d);
}


GJK_return collision_test_GJK(const std::pair<BVHLeaf*, BVHLeaf*>& pair){

    // initialize
    const auto& coll_1 = pair.first->m_collidable;
    const auto& coll_2 = pair.second->m_collidable;
    
    eg::Vector3d d;
    d <<
        (double)pair.second->m_collidable->m_pos_x - (double)pair.first->m_collidable->m_pos_x,
        (double)pair.second->m_collidable->m_pos_y - (double)pair.first->m_collidable->m_pos_y,
        0;
    d.normalize();
    // simplex global
    std::vector<eg::Vector3d> simplex;
    // simplex local 1
    std::vector<size_t> simplex_1_ind; // TODO: as struct of simplex
    // simplex local 2
    std::vector<size_t> simplex_2_ind; 
    // find first point on the simplex
    simplex.push_back(*coll_1->support_func(d) - *coll_2->support_func(-d));
    // initial vector
    d = -simplex.at(0);
    while(true){
        // std::cout << "Entering GJK with simplex of size "<< simplex.size() << std::endl;
        // find next point on the simplex 
        eg::Vector3d A = *(coll_1->support_func(d)) - *(coll_2->support_func(-d));
        // check whether A passed the origin
        if(A.dot(d) < 0){
            return {false, nullptr};
        }        
        simplex.push_back(A);
        if (handleSimplex(simplex, d)){
            return {
                true,                                                           // collision indicator
                std::make_unique<std::vector<eg::Vector3d>>(std::move(simplex)) // simplex
            };
        }
    }        
}

std::unique_ptr<PolyEdge> getClosestEdge(std::vector<eg::Vector3d>* simplex){
    // initialize the edge
    std::unique_ptr<PolyEdge> closestEdge = std::make_unique<PolyEdge>();
    closestEdge->distance = std::numeric_limits<float>::infinity();

    // search for closest edge
    for(size_t i = 0; i < simplex->size(); ++i){
        size_t j = i + 1 == simplex->size() ? 0 : i+1;
        eg::Vector3d edge_ij = simplex->at(j) - simplex->at(i);
        eg::Vector3d& oi = simplex->at(i);
        eg::Vector3d ij_normal = tripleCrossProd(edge_ij, oi, edge_ij); ij_normal.normalize(); // FIXME: to be replaced with a per-product ... 
        float distance = oi.dot(ij_normal);
        // new closest found -> set it
        if (distance < closestEdge->distance){
            closestEdge->distance = distance;
            closestEdge->edge_start_ind = i;
            closestEdge->normal = ij_normal;
        }
    }
    return closestEdge;
}

EPA_return collision_test_EPA(std::unique_ptr<std::vector<eg::Vector3d>> simplex, const std::pair<BVHLeaf*, BVHLeaf*>& pair){
    const auto& coll_1 = pair.first->m_collidable;
    const auto& coll_2 = pair.second->m_collidable;
    while (true){
        // get new edge
        auto closest_edge = getClosestEdge(simplex.get());
        // get new point on minkowski difference in direction of normal
        eg::Vector3d new_p = *(coll_1->support_func(closest_edge->normal)) - *(coll_2->support_func(-closest_edge->normal));
        // check if new point is on the same edge
        float tolerance = 0.01;
        if (!(abs(new_p.dot(closest_edge->normal)-closest_edge->distance) < tolerance)){
            // insert new point to polytope
            simplex->insert(simplex->begin() + closest_edge->edge_start_ind + 1, new_p);
        }
        else {
            return {
                eg::Vector2f(closest_edge->normal[0], closest_edge->normal[1]), // penetration_vector;
                closest_edge->distance                                          // penetration_depth; 
            };
        }
    }

}





// bool collision_test_GJK_archived(const std::pair<BVHLeaf*, BVHLeaf*>& pair){

//     // initialize
//     const auto& coll_1 = pair.first->m_collidable;
//     const auto& coll_2 = pair.second->m_collidable;
    
//     eg::Vector3d d;
//     d <<
//         (double)pair.second->m_collidable->m_pos_x - (double)pair.first->m_collidable->m_pos_x,
//         (double)pair.second->m_collidable->m_pos_y - (double)pair.first->m_collidable->m_pos_y,
//         0;
//     d.normalize();
//     std::vector<eg::Vector3d> simplex;
//     // find first point on the simplex
//     simplex.push_back(*(coll_1->support_func(d)) - *(coll_2->support_func(-d)));
//     // initial vector
//     d = -simplex.at(0);
//     while(true){
//         std::cout << "Entering GJK with simplex of size "<< simplex.size() << std::endl;
//         // find next point on the simplex 
//         eg::Vector3d A = *(coll_1->support_func(d)) - *(coll_2->support_func(-d));
//         // check whether A passed the origin
//         if(A.dot(d) < 0){
//             return false;
//         }
//         simplex.push_back(A);
//         // handle line
//         if (simplex.size() == 2){
//             eg::Vector3d BA = simplex.at(1)-simplex.at(0);
//             eg::Vector3d BO = -simplex.at(1);
            
//             d = BA.cross(BO.cross(BA));
//             d.normalize();

//         }
//         // handle triangle
//         if (simplex.size() == 3){
//             eg::Vector3d CO = (-simplex.at(2)); CO.normalize();
//             eg::Vector3d CB = simplex.at(2) - simplex.at(1); CB.normalize();
//             eg::Vector3d CA = simplex.at(2) - simplex.at(0); CA.normalize();
//             eg::Vector3d CBxCA = CB.cross(CA); CBxCA.normalize();
//             // eg::Vector3d CBo = CB.cross(CBxCA);
//             // eg::Vector3d CAo = CBxCA.cross(CA);
//             eg::Vector3d CBo = CB.cross(CBxCA); CBo.normalize();
//             eg::Vector3d CAo = -(CA.cross(CBxCA)); CAo.normalize();
//             const double epsilon = 2e-5;
//             bool check_CBo = CO.dot(CBo) < 0 - epsilon;
//             bool check_CAo = CO.dot(CAo) < 0 - epsilon;
            
//             std::cout << "Entered GJK check for simplex of size 3: " << CO.dot(CBo) << " "<< CO.dot(CAo)<< std::endl;
//             std::cout << "GJK Checks: " << check_CBo << " " << check_CAo << std::endl;
           

            
//             if (check_CBo){
//                 // origin in region CB
//                 d = CBo;
//                 simplex.erase(simplex.begin());

//             }
//             if (check_CAo){
//                 // origin in region CA
//                 d = CAo;
//                 simplex.erase(simplex.begin()+1);
//             }
//             if ((!check_CAo)&&(!check_CBo)){
//                 // found origin --> objects are intersecting
//                 return true;
//             }
//             return false; //FIXME: this should not be here --> check algo.


//             // Region BC
            
//         }      
//     }
// }