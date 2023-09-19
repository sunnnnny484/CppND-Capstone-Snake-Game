#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "player.h"
#include <memory>
#include <thread>

#define PLAYER_1 0
#define PLAYER_2 1

typedef enum gameState
{
    STOPPED = 0,
    RUNNING,
    PAUSED,
} GameState;

class Game
{
public:
    Game(std::size_t grid_width, std::size_t grid_height);
    ~Game();
    void Run(Controller const &controller, Renderer &renderer,
             std::size_t target_frame_duration);
    int GetScore(int playerID) const;
    int GetSize(int playerID) const;
    void SetGameState(GameState newState);
    GameState GetCurrentState();

private:
    std::shared_ptr<Snake> snake;
    std::vector<Snake> snakeList;
    std::shared_ptr<Player> player;
    std::vector<Player> playerList;
    std::vector<SDL_Point> food;

    std::random_device dev;
    std::mt19937 engine;
    std::uniform_int_distribution<int> random_w;
    std::uniform_int_distribution<int> random_h;
    std::thread foodSpawn;

    GameState state_{RUNNING};

    void PlaceFood();
    void Update();
    bool EatFood();
};

#endif