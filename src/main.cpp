#include <iostream>
#include "controller.h"
#include "game.h"
#include "renderer.h"

int main()
{
    constexpr std::size_t kFramesPerSecond{60};
    constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
    constexpr std::size_t kScreenWidth{640};
    constexpr std::size_t kScreenHeight{640};
    constexpr std::size_t kGridWidth{32};
    constexpr std::size_t kGridHeight{32};

    Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
    Controller controller;
    Game game(kGridWidth, kGridHeight);
    game.Run(controller, renderer, kMsPerFrame);
    std::cout << "Game has terminated successfully!\n";
    std::cout << "Player 1\n";
    std::cout << "Score: " << game.GetScore(PLAYER_1) << "\n";
    std::cout << "Size: " << game.GetSize(PLAYER_1) << "\n";
    std::cout << "Player 2\n";
    std::cout << "Score: " << game.GetScore(PLAYER_2) << "\n";
    std::cout << "Size: " << game.GetSize(PLAYER_2) << "\n";

    return 0;
}