#include "entity.h"


class Particle : public Entity{
    public:
        Particle(float x, float y, float life);
        void update(float delta_time);

        static std::vector<Particle*> s_particle_instances;
    private:
        float p_life = 0;
        

};

class ParticleSmoke : public Particle {
    public:
        // create with initial size s
        ParticleSmoke(float x, float y, float s, float spread_factor, float life);
        void update(float delta_time);

        static std::vector<ParticleSmoke*> s_particleSmoke_instances;
        
    private:
        float p_spread_factor = 0.3;
};

// the initial values of a particle after it is spawned
typedef void(*HandleParticleSpawnEvent)(
    float pos_x,
    float pos_y,
    float particle_vel_x,
    float particle_vel_y,
    float particle_rot_angle
);

class ParticleEmittor{
    public:
        ParticleEmittor(float pos_x, float pos_y, float emit_every, HandleParticleSpawnEvent emittorFunc);
        void updateSpawn(float delta_time);
        float m_pos_x;
        float m_pos_y;
        float m_particle_vel_x;
        float m_particle_vel_y;
        float m_particle_rot_angle;
        HandleParticleSpawnEvent m_emittorFunc;
    private:
        float p_clock = 0;
        float p_emit_every;
};


