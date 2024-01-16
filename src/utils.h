#pragma once
#include<iostream>
#include<Eigen/Dense>
#include<vector>
#include <SDL.h>

namespace eg = Eigen;

class GUIDManager{
    private:
        static uint64_t next_free_id;
    public:
        // uint64_t get_next_free_id();
}; 

class Utils{
    public:
        static void rotate_vec_by_angle(float theta, eg::Vector2f &point);
        static void rotate_vec_by_angle(float theta, std::vector<eg::Vector2f> &points);
        static void vecs_to_points (eg::Vector2f* vectors, SDL_FPoint* points, int num_points);
        static void vecs_to_points (std::vector<eg::Vector2f> vectors, SDL_FPoint* points, int num_points);
        static void rotate_vec_90_CCW (eg::Vector2f &vec);

        // reads a list of points from a file into a vector
        static void read_txt_into_vectorlist(std::vector<eg::Vector2f>& vec, std::string& file_path);
        // static void Utils::setRenderDefaultColor(SDL_Renderer*);
};
