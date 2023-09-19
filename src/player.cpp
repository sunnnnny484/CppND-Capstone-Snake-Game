#include "player.h"
#include <fstream>

Player::Player(int id) : id(id)
{
    std::cout << "Enter the player " << id << " name within 20 characters:\n";
    std::cin >> name;
    name = name.substr(0, MAX_NAME_LEN);
}

void Player::SetScore(int point)
{
    score = point;
}

int Player::GetScore() {
    return score;
}

void Player::SaveToScoreBoard()
{
    std::ofstream file;
    file.open("../scoreboard.txt", std::ios::app);
    if (file.is_open())
    {
        file << name << "\t" << score << std::endl;
    }

    file.close();
}
