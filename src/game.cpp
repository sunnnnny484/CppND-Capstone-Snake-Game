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
    snake = std::make_shared<Snake>(grid_width, grid_height);
    player = std::make_shared<Player>();
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
        controller.HandleInput(*this, *snake);

        // Display the pause menu
        if (state_ == PAUSED)
        {
            renderer.UpdatePauseMenu();
            continue;
        }

        Update();
        renderer.Render(*snake, food);

        frame_end = SDL_GetTicks();

        // Keep track of how long each loop through the input/update/render cycle
        // takes.
        frame_count++;
        frame_duration = frame_end - frame_start;

        // After every second, update the window title.
        if (frame_end - title_timestamp >= 1000)
        {
            renderer.UpdateWindowTitle(score, frame_count);
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
            if (!snake->SnakeCell(x, y))
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
    if (!snake->alive)
        return;

    snake->Update();

    int new_x = static_cast<int>(snake->head_x);
    int new_y = static_cast<int>(snake->head_y);

    // Check if there's food over here
    if (AteFood(new_x, new_y))
    {
        score++;
        // PlaceFood();
        // Grow snake and increase speed.
        snake->GrowBody();
        snake->speed += 0.02;
    }
}

Game::~Game()
{
    player->SetScore(score);
    player->SaveToScoreBoard();
    foodSpawn.join();
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake->size; }

void Game::SetGameState(GameState newState)
{
    state_ = newState;
}

GameState Game::GetCurrentState()
{
    return state_;
}

bool Game::AteFood(int newHeadX, int newHeadY)
{
    for (auto iter = food.begin(); iter < food.end(); iter++)
    {
        if ((*iter).x == newHeadX && (*iter).y == newHeadY)
        {
            food.erase(iter);
            return true;
        }
    }

    return false;
}