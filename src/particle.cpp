#include "particle.h"
#include <iostream>



void Particle::update(float delta_time){
        Entity::update(delta_time);
        p_life -= delta_time;
        if (p_life < 0){
            m_alive = false;
            
        }
    }

Particle::Particle(float x, float y, float life) : Entity(x,y), p_life(life){
    s_particle_instances.push_back(this);
};

ParticleSmoke::ParticleSmoke(float x, float y, float s, float spread_factor, float life)
    : Particle(x, y, life), p_spread_factor(spread_factor){
    std::vector<eg::Vector2f> repr;
    repr.push_back(eg::Vector2f(-s/2, -s/2));
    repr.push_back(eg::Vector2f(-s/2, +s/2));
    repr.push_back(eg::Vector2f(+s/2, +s/2));
    repr.push_back(eg::Vector2f(+s/2, -s/2));
    repr.push_back(eg::Vector2f(-s/2, -s/2));
    setRepresentation(repr);

    s_particleSmoke_instances.push_back(this);
}

void ParticleSmoke::update(float delta_time){
    Particle::update(delta_time);
    m_repr_local.at(0) += eg::Vector2f(-p_spread_factor, -p_spread_factor) * delta_time;
    m_repr_local.at(1) += eg::Vector2f(-p_spread_factor, +p_spread_factor) * delta_time;
    m_repr_local.at(2) += eg::Vector2f(+p_spread_factor, +p_spread_factor) * delta_time;
    m_repr_local.at(3) += eg::Vector2f(+p_spread_factor, -p_spread_factor) * delta_time;
    m_repr_local.at(4) += eg::Vector2f(-p_spread_factor, -p_spread_factor) * delta_time;
    m_rot_angle += m_rot_angle_vel * delta_time;
    // color[4] *= 0.95;
    
    for(auto& c : color){
        if ( c > 70){
            c *= 0.98;
        }
    }
}

ParticleEmittor::ParticleEmittor(float pos_x, float pos_y, float emit_every, HandleParticleSpawnEvent emittorFunc)
    : m_pos_x(pos_x), m_pos_y(pos_y), m_emittorFunc(emittorFunc), p_emit_every(emit_every){}



void ParticleEmittor::updateSpawn(float delta_time){
    p_clock += delta_time;
    std::cout << "updating emittor with clock = " << p_clock << std::endl;
    
    // trigger spawn event when it's time
    if (p_clock > p_emit_every){
        p_clock = 0;
        m_emittorFunc(
            m_pos_x, 
            m_pos_y, 
            m_particle_vel_x, 
            m_particle_vel_y, 
            m_particle_rot_angle);
    }
}