#pragma once
#include "Player.h"
#include "raylib.h"

struct AnimData {
    Rectangle rect;
    int frame;
    float updateTime;
    float runningTime;
    Rectangle destRec;
    int spriteCount;
    Texture2D texture;
};

enum class AnimEndBehavior {
    LOOP,
    HOLD_LAST,
    RESET
};

struct AnimState {
    Texture2D texture;
    AnimData animData;
    bool wasInAir;
    bool isLanding;
};

struct AnimationParams {
    const char *fileName;
    const int spriteCount;
    const float updateTime;
};

struct CharacterAnimations {
    const AnimationParams idle;
    const AnimationParams jump;
    const AnimationParams land;
};



class Character {
public:
    Character(const CharacterAnimations &animations, float scale);
    ~Character();

    // These can be three inner classes wrapped around tick
    void move(float deltaTime);
    void apply_velocity(PlayerInput input, int ground, float deltaTime);
    void animate(float dt, int ground);
    void draw() const;
    void spawn(Vector2 worldPos);

    // Remove and just take input
    void setPosition(const Vector2 &pos);
    void setVelocity(Vector2 vel);

private:
    AnimData idleAnimation{};
    AnimData jumpAnimation{};
    AnimData landAnimation{};
    AnimState animationState{};
    float scaledWidth{};
    float scaledHeight{};
    Vector2 position{};
    Vector2 velocity{};
    Vector2 lookingAt{1.f, 0.f};
    const float MovementSpeed{400.f}; // pixels/sec;
    const float JumpVelocity{600.f}; // pixels/sec;

    static void loadAnimation(
        const AnimationParams &animParams,
        AnimData &animData
    );

    void update_character_anim_state(
        AnimData &animData,
        float dt,
        AnimEndBehavior behavior
    );

    void update_character_animation_data(
        AnimData &animData,
        float dt,
        AnimEndBehavior behavior
    );

    [[nodiscard]] bool isGrounded(int ground) const;
};
