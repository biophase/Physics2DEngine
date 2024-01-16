#include<iostream>
#include<SDL.h>
#include<vector>
#include<Eigen/Dense>
#include<memory>

// #include "utils.h"
// #include "constants.h"
// #include "world.h"
// #include "game.h"

#include "utils.h"
#include "entity.h"
#include "player.h"
#include "car.h"
#include "world.h"
#include "constants.h"
#include "camera.h"
#include "collision.h"
#include "particle.h"


// globals
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool is_running = false;
int last_frame_time = 0;
GUIDManager* guid_manager;
World* world = NULL;
BVHTree* bvhTree = NULL;

// containers
std::vector<Entity*> Entity::entity_instances;
std::vector<Collidable*> Collidable::collidable_instances;
std::vector<Player*> Player::player_instances;
std::vector<Car*> Car::car_instances;
std::vector<Camera*> Camera::camera_instances;
// std::vector<std::shared_ptr<BVHLeaf>> BVHLeaf::bvhLeaf_instances;
std::vector<BVHLeaf*> BVHLeaf::bvhLeaf_instances;
std::vector<Particle*> Particle::s_particle_instances;
std::vector<ParticleSmoke*> ParticleSmoke::s_particleSmoke_instances;











void setup(){
    guid_manager = new GUIDManager;

    // create world
    // std::string fname = "./assets/world/world_2.txt";
    // world = new World(52,28, fname);

    // collision setup
    bvhTree = new BVHTree(&BVHLeaf::bvhLeaf_instances);

    // create players
    new Player; // Player 0
    Player::player_instances.at(0)->controls.key_down=SDL_SCANCODE_S;
    Player::player_instances.at(0)->controls.key_left=SDL_SCANCODE_A;
    Player::player_instances.at(0)->controls.key_up=SDL_SCANCODE_W;
    Player::player_instances.at(0)->controls.key_right=SDL_SCANCODE_D;
    
    Player::player_instances.at(0)->controls.apply_break=SDL_SCANCODE_SPACE;
    Player::player_instances.at(0)->controls.boost=SDL_SCANCODE_LSHIFT;
    Player::player_instances.at(0)->controls.debug_action=SDL_SCANCODE_Q;

    Player::player_instances.at(0)->color = {255,0,250,255};
    new Car(Player::player_instances.at(0), 20, WINDOW_HEIGHT/2, CAR_REPR_FP);
    // Car::car_instances.at(0)->setRepresentation(CAR_REPR);
    // Camera for player 1
    new Camera(Car::car_instances[0]);


   
    new Player; // Player 1
    Player::player_instances.at(1)->controls.key_down=SDL_SCANCODE_DOWN;
    Player::player_instances.at(1)->controls.key_left=SDL_SCANCODE_LEFT;
    Player::player_instances.at(1)->controls.key_up=SDL_SCANCODE_UP;
    Player::player_instances.at(1)->controls.key_right=SDL_SCANCODE_RIGHT;
    
    Player::player_instances.at(1)->controls.apply_break=SDL_SCANCODE_RSHIFT;
    Player::player_instances.at(1)->controls.boost=SDL_SCANCODE_L;

    
    new Car(Player::player_instances.at(1), WINDOW_WIDTH-20, WINDOW_HEIGHT/2, CAR_REPR_FP);
    // Car::car_instances.at(1)->setRepresentation("./assets/objects/polygon.txt");
    Car::car_instances.at(1)->color={50,50,255,255};
    
    // world grid
    for (float i = 0; i < 1500; i += 100){
        for (float j = 0; j < 2600; j += 100){
            Entity* grid = new Entity(j,i);
            grid->color = {60,60,60,0};
        }
    }

    // some temp entities for collision tests
    Collidable* a =  new Collidable(200,200,40,80);
    a->m_rot_angle = M_PI/4.456;
    a->m_fric = 0.2;
    Collidable* b = new Collidable(300,400,50,300);
    b->m_rot_angle = -M_PI*1.232;
    b->m_fric = 0.2;
    Collidable* c = new Collidable(200,1000,300,600);
    c->m_rot_angle = 80;
    c->m_fric = 0.2;
    // temp entitites grid
    // for (int i = 500; i<5000; i+=200){
    //     for (int j=1300; j <5000; j += 200){
    //         auto temp = new Collidable((float)i, (float)j, 50,50);
    //         temp->m_fric = 0.5;
    //     }
    // }
}

void process_input(){
    
    // handle continous keys

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    for (auto car : Car::car_instances){
        car->process_input(keystate);
    }
    // handle single presses and others 
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT: // OS level event (e.g. "X" button)
            is_running = false;
            break;

        // handle keys
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case SDLK_ESCAPE:
                    is_running = false;
                    break;
            }
            break;
        
    }
}

void update(){
    // waste some time / sleep until we reach the frame target time


    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));

    int time_to_wait = FRAME_TARGET_TIME -(last_frame_time - SDL_GetTicks());
    SDL_Delay(time_to_wait);
    float delta_time = (SDL_GetTicks() - last_frame_time)/1000.0f;
    // logic to keep fixed time step
    last_frame_time = SDL_GetTicks();


    // clean dead entities
    Entity::cleanUp();

    

    // TODO: WHY DOES UPDATE() NEED TO BE CALLED ON EACH LEVEL BUT DRAW() DOES NOT?
    for (auto entity : Entity::entity_instances){
        entity->update(delta_time);
    }
    // for (auto tire : Car::car_instances){
    //     tire->Car::update(delta_time);
    // }

    // for (auto collidable : Collidable::collidable_instances){
    //     collidable->Collidable::update(delta_time);
    // }

    // for (auto camera : Camera::camera_instances){
    //     camera->Camera::update(delta_time);
    // }

    // for (auto particle : Particle::s_particle_instances){
    //     particle->Particle::update(delta_time);
    // }
    
    // for (auto particle_smoke : ParticleSmoke::s_particleSmoke_instances){
    //     particle_smoke->ParticleSmoke::update(delta_time);
    // }
    auto pairs = bvhTree->getCandidatePairs();
    std::cout << "Found " << (*pairs).size() << " AABB potential colliding pairs" << std::endl;
    for (const auto& pair : *pairs){
        bool pairs_colliding = collision_test_AABB_AABB(pair);
        if (pairs_colliding){
            // std::cout << "Found AABB collision!" << std::endl;
            auto gjk_result = collision_test_GJK(pair);
            if (gjk_result.colliding) {
                // std::cout << "Narrow collision detected" << std::endl;
                pair.first->m_collidable->m_temp_colliding = true;
                pair.second->m_collidable->m_temp_colliding = true;

                auto epa_result = collision_test_EPA(std::move(gjk_result.termination_simplex), pair);
                // std::cout << "EPA depth = " << epa_result.penetration_depth << std::endl;
                

                if (epa_result.penetration_depth > 0){
                    pair.first->m_collidable->m_pos_x += -epa_result.penetration_vector[0]*epa_result.penetration_depth/3;
                    pair.first->m_collidable->m_pos_y += -epa_result.penetration_vector[1]*epa_result.penetration_depth/3;
                    pair.second->m_collidable->m_pos_x += epa_result.penetration_vector[0]*epa_result.penetration_depth/3;
                    pair.second->m_collidable->m_pos_y += epa_result.penetration_vector[1]*epa_result.penetration_depth/3;
                }

                pair.first->m_collidable->m_vel_x += -epa_result.penetration_vector[0]*pow(epa_result.penetration_depth,1.5);
                pair.first->m_collidable->m_vel_y += -epa_result.penetration_vector[1]*pow(epa_result.penetration_depth,1.5);
                pair.second->m_collidable->m_vel_x += epa_result.penetration_vector[0]*pow(epa_result.penetration_depth,1.5);
                pair.second->m_collidable->m_vel_y += epa_result.penetration_vector[1]*pow(epa_result.penetration_depth,1.5);
                
            }
        }
    }
    
    
    

    std::cout << "FPS: " << 1 / delta_time << std::endl;
}

void draw(SDL_Renderer* renderer){
    // SDL_SetRenderDrawColor(renderer, 48, 10, 36, 255); // set background color
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND); // set background color
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);
    
    for(auto entity : Entity::entity_instances){
        entity->draw(renderer,  *Camera::camera_instances.at(0));
    }

    // for(auto car : Car::car_instances){
    //     car->draw(renderer);
    // }
    // world->draw(renderer);

    // swap front and back buffer
    SDL_RenderPresent(renderer);

}

void destroy_window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool initialize_window(){

    // sdl init
    if (SDL_Init(SDL_INIT_EVERYTHING) > 0)
    {
        std::cout << "SDL_Init failed with error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    
    // creating the window
    window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED, // centered
        SDL_WINDOWPOS_CENTERED, // centered
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
        // SDL_WINDOW_BORDERLESS
        );
    if (!window){
        std::cerr << "Error creating an SDL window" << std::endl;
        return false;
    }
    // full screen
    // if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN)==-1){
    //     std::cerr << "Error going into full screen" << std::endl;
    //     return false;
    // }
    // create a renderer
    renderer = SDL_CreateRenderer(
        window,
        -1, // default display number
        0   // no flags
    );
    if (!renderer){
        std::cerr << "Error creating SDL renderer" << std::endl; 
        return false;
    }
    return true;
}

int main(){

    is_running = initialize_window();
    // std::string greetings = "Hello SDL2";
    // std::cout << greetings << std::endl;
    
    setup();
    while (is_running){
        process_input();
        update();
        draw(renderer);
        std::cout << "number of entities: " << Entity::entity_instances.size() << std::endl;
    }

    destroy_window();
    return EXIT_SUCCESS;
}