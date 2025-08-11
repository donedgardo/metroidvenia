#pragma once

struct PlayerInput {
    bool jumpButton{};
    float horizontal{};
};

class Player {
public:
    static PlayerInput getInput() ;
};
