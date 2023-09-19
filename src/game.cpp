#include "game.h"
#include <iostream>
#include "SDL.h"
#include <chrono>

using namespace std::chrono;

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1))
{
    Snake snake = Snake(grid_width, grid_height, PLAYER_1);
    snakeList.emplace_back(snake);
    snake = Snake(grid_width, grid_height, PLAYER_2);
    snakeList.emplace_back(snake);

    Player player = Player(PLAYER_1);
    playerList.emplace_back(player);
    player = Player(PLAYER_2);
    playerList.emplace_back(player);
    foodSpawn = std::thread(&Game::PlaceFood, this);
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration)
{
    Uint32 title_timestamp = SDL_GetTicks();
    Uint32 frame_start;
    Uint32 frame_end;
    Uint32 frame_duration;
    int frame_count = 0;

    while ((state_ == RUNNING) || (state_ == PAUSED))
    {
        frame_start = SDL_GetTicks();

        // Input, Update, Render - the main game loop.
        controller.HandleInput(*this, snakeList);

        // Display the pause menu
        if (state_ == PAUSED)
        {
            renderer.UpdatePauseMenu();
            continue;
        }

        Update();
        renderer.Render(snakeList, food);

        frame_end = SDL_GetTicks();

        // Keep track of how long each loop through the input/update/render cycle
        // takes.
        frame_count++;
        frame_duration = frame_end - frame_start;

        // After every second, update the window title.
        if (frame_end - title_timestamp >= 1000)
        {
            renderer.UpdateWindowTitle(frame_count);
            frame_count = 0;
            title_timestamp = frame_end;
        }

        // If the time for this frame is too small (i.e. frame_duration is
        // smaller than the target ms_per_frame), delay the loop to
        // achieve the correct frame rate.
        if (frame_duration < target_frame_duration)
        {
            SDL_Delay(target_frame_duration - frame_duration);
        }
    }
}

#define MAX_FOOD 5
void Game::PlaceFood()
{
    SDL_Point newFood;
    int x, y;

    while (true)
    {
        if (food.size() < MAX_FOOD)
        {
            x = random_w(engine);
            y = random_h(engine);
            // Check that the location is not occupied by a snake item before placing
            // food.
            if (!snakeList[0].SnakeCell(x, y) && !snakeList[1].SnakeCell(x, y))
            {
                newFood.x = x;
                newFood.y = y;
                food.emplace_back(newFood);
            }
        }

        std::this_thread::sleep_for(milliseconds(400));
        if (state_ == STOPPED)
            return;
    }
}

void Game::Update()
{
    if (!snakeList[0].alive && !snakeList[1].alive)
        return;

    snakeList[0].Update(snakeList[1]);
    snakeList[1].Update(snakeList[0]);

    // Check if there's food over here
    EatFood();
}

Game::~Game()
{
    playerList[0].SaveToScoreBoard();
    playerList[1].SaveToScoreBoard();
    foodSpawn.join();
}

int Game::GetScore(int playerID) const { return playerList[playerID].GetScore(); }
int Game::GetSize(int playerID) const { return snakeList[playerID].size; }

void Game::SetGameState(GameState newState)
{
    state_ = newState;
}

GameState Game::GetCurrentState()
{
    return state_;
}

bool Game::EatFood()
{
    for (auto iter = food.begin(); iter < food.end(); iter++)
    {
        /* Player 1 gets the food */
        if ((*iter).x == static_cast<int>(snakeList[0].head_x) &&
            (*iter).y == static_cast<int>(snakeList[0].head_y))
        {
            snakeList[PLAYER_1].GrowBody();
            snakeList[PLAYER_1].speed += 0.02;
            food.erase(iter);
            playerList[0].SetScore(playerList[0].GetScore() + 1);
            return true;
        }
        /* Player 2 gets the food */
        else if ((*iter).x == static_cast<int>(snakeList[1].head_x) &&
                 (*iter).y == static_cast<int>(snakeList[1].head_y))
        {
            snakeList[PLAYER_2].GrowBody();
            snakeList[PLAYER_2].speed += 0.02;
            food.erase(iter);
            playerList[1].SetScore(playerList[1].GetScore() + 1);
            return true;
        }
    }

    return false;
}