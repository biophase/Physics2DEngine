#pragma once
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <SDL.h>


class World{
    public:
        World(u_int32_t size_x, u_int32_t size_y, std::string& fname);
        ~World();
    
    enum BlockType : char{
        EMPTY, WALL};
    
    BlockType getBlock(const int& x, const int& y) {return m_content->at(y).at(x);}
    void draw(SDL_Renderer *renderer);
    private:
        u_int32_t m_size_x;
        u_int32_t m_size_y;
        float32_t m_cellSize_x = 50;
        float32_t m_cellSize_y = 50;
        std::unordered_map<char, BlockType> m_parseMap;
        // std::vector<std::vector<BlockType>> *m_content;
        std::array<std::array<BlockType, 52>, 28> *m_content;
        // BlockType **m_content;
        
        
        

};