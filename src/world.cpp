#include "world.h"
#include<iostream>

World::World(u_int32_t size_x, u_int32_t size_y, std::string& fname)
    
    : m_size_x(size_x), m_size_y(size_y){    
        m_parseMap['.'] = EMPTY;
        m_parseMap['o'] = WALL;
        m_content = new std::array<std::array<BlockType, 52>, 28>;

        std::ifstream reader;
        reader.open(fname);
        char row_buffer[size_x];
        if (!reader.is_open()){
            exit(EXIT_FAILURE);
        }
        while (reader.good()){
            reader >> row_buffer;
            int n_row = 0;
            std::vector<BlockType> row;
            for (int x=0; x < size_x; x++){
                // auto val = m_parseMap[row_buffer[x]];
                m_content->at(n_row).at(x) = m_parseMap[row_buffer[x]];
                // std::cout << row_buffer[x];
            }
            

            n_row ++;
        }
        reader.close();
    }
World::~World(){
    delete m_content;
}
void World::draw(SDL_Renderer *renderer){
    for(int y=0; y < m_size_y; y++){
        for(int x=0; x < m_size_x; x++){
            float32_t x_pos = x*m_cellSize_x;
            float32_t y_pos = y*m_cellSize_y;
            if (m_content->at(y).at(x) == WALL){
                SDL_RenderDrawLineF(renderer, x_pos, y_pos, x_pos+m_cellSize_x, y_pos);
                SDL_RenderDrawLineF(renderer, x_pos+m_cellSize_x, y_pos, x_pos+m_cellSize_x, y_pos+m_cellSize_y);
                SDL_RenderDrawLineF(renderer, x_pos+m_cellSize_x, y_pos+m_cellSize_y, x_pos, y_pos+m_cellSize_y);
                SDL_RenderDrawLineF(renderer, x_pos, y_pos+m_cellSize_y, x_pos, y_pos);
            }
        }
    }
}