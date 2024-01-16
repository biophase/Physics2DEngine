#include <Eigen/Dense>
#include "utils.h"
#include <math.h>
#include <SDL.h>
#include <fstream>
// // uint64_t GUIDManager::get_next_free_id(){
// //     u_int64_t free_id = next_free_id;
// //     next_free_id ++;
// //     return free_id;
// // }



void Utils::rotate_vec_by_angle(float theta, eg::Vector2f &vec){
    // rotate single vector
    eg::Matrix2f rot_matrix;
    rot_matrix <<
        cos(theta), -sin(theta),
        sin(theta), cos(theta);
    vec = rot_matrix * vec;
}

void Utils::rotate_vec_by_angle(float theta, std::vector<eg::Vector2f> &vec){
    // rotate multiple vectors
    eg::Matrix2f rot_matrix;
    rot_matrix <<
        cos(theta), -sin(theta),
        sin(theta), cos(theta);
    for(int i=0; i<=vec.size(); i++){
        vec[i] = rot_matrix * vec[i];
    }
}

void Utils::vecs_to_points (eg::Vector2f* vectors, SDL_FPoint* points, int num_points){
    // Converts an array of 2D vectors to SDL floating points for rendering
    for (int i=0; i<=num_points-1; i++){
        points[i] = {vectors[i](0,0),vectors[i](1,0)};
    }
}

void Utils::vecs_to_points (std::vector<eg::Vector2f> vectors, SDL_FPoint* points, int num_points){
    // Converts an array of 2D vectors to SDL floating points for rendering
    for (int i=0; i<=vectors.size() -1; i++){
        points[i] = {vectors[i](0,0),vectors[i](1,0)};
    }
}
void Utils::rotate_vec_90_CCW (eg::Vector2f &vec){
    eg::Matrix2f perp_rot;
    perp_rot << +0 , -1,
                +1 , +0;
    vec = perp_rot * vec;
}
void Utils::read_txt_into_vectorlist(std::vector<eg::Vector2f>& vec, std::string& file_path){
    
    std::ifstream reader;
    reader.open(file_path);
    std::string item;
    std::vector<float> coordinate;
    while(reader.good()){
        reader >> item;
        if (item == "<endpoint>") continue;
        else{
            coordinate.push_back(std::stof(item));
        }
        if (coordinate.size() >= 2){
            vec.push_back(eg::Vector2f(coordinate[0], coordinate[1]));
            coordinate.clear();
        }
    }
}

