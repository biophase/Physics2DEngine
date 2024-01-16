#pragma once
// #include "game.h"



#include<SDL.h>
#include<vector>
#include<Eigen/Dense>

struct Controls{
    SDL_Scancode key_up;
    SDL_Scancode key_down;
    SDL_Scancode key_left;
    SDL_Scancode key_right;

    SDL_Scancode boost;
    SDL_Scancode apply_break;

    SDL_Scancode debug_action;
};
class Player{
    public:
        static std::vector<Player*> player_instances;
        std::string name;
        // Uint8 color[4];
        std::array<Uint8, 4> color;
        Controls controls;   
        Player();
}; 