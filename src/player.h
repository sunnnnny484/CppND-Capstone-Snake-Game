#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string.h>

#define MAX_NAME_LEN 20

class Player
{
public:
    Player(int id);
    void SetScore(int point);
    int GetScore() const;
    void SaveToScoreBoard();

private:
    std::string name{MAX_NAME_LEN, '\0'};
    int score{0};
    int id{0};
};

#endif /* PLAYER_H */