#include "Player.h"
#include "raylib.h"

PlayerInput Player::getInput() {
    auto input = PlayerInput{};
    if (IsKeyDown(KEY_A)) {
        input.horizontal = -1.f;
    }
    if (IsKeyDown(KEY_D)) {
        input.horizontal = 1.f;
    }
    if (IsKeyPressed(KEY_SPACE)) {
        input.jumpButton = true;
    }
    return input;
}