#include <iterator>
#include "Character.h"
#include "raymath.h"

Character::Character(const CharacterAnimations &animations, const float scale) {
    const AnimationParams animationParams[] = {
        animations.idle,
        animations.jump,
        animations.land
    };
    AnimData *animDataPointers[] = {
        &idleAnimation,
        &jumpAnimation,
        &landAnimation
    };
    for (size_t i = 0; i < std::size(animationParams); i++) {
        loadAnimation(animationParams[i], *animDataPointers[i]);
    }
    scaledWidth = idleAnimation.rect.width * scale;
    scaledHeight = idleAnimation.rect.height * scale;
    animationState = {
        idleAnimation.texture,
        idleAnimation,
        false,
        false
    };
}

void Character::setPosition(const Vector2 &pos) {
    position = pos;
}

void Character::setVelocity(const Vector2 vel) {
    velocity = vel;
}

void Character::move(const float deltaTime) {
    lookingAt.x = velocity.x >= 0 ? 1 : -1;
    position = position + Vector2Scale(velocity, deltaTime);
}

void Character::apply_velocity(const PlayerInput input, const int ground, const float deltaTime) {
    if (!isGrounded(ground)) {
        constexpr float gravity{1'000.f};
        setVelocity(Vector2 {input.horizontal * MovementSpeed, velocity.y + gravity * deltaTime});
    } else if (input.jumpButton) {
        velocity.y -= JumpVelocity;
    } else {
        setVelocity(Vector2{input.horizontal * MovementSpeed, 0.f});
    }
}

bool Character::isGrounded(const int ground) const {
    return position.y >= static_cast<float>(ground) - scaledHeight;
}

void Character::animate(const float dt, const int ground) {
    if (isGrounded(ground)) {
        //flip sprite on looking at
        animationState.animData.rect.width = abs(animationState.animData.rect.width) * lookingAt.x;
        // reset jump anim data
        jumpAnimation.frame = 0;
        jumpAnimation.runningTime = 0;
        // Landing
        if (animationState.wasInAir || animationState.isLanding) {
            animationState.wasInAir = false;
            animationState.isLanding = true;
            update_character_anim_state(landAnimation, dt,
                                        AnimEndBehavior::RESET);
        } else {
            // Idle
            update_character_anim_state(idleAnimation, dt,
                                        AnimEndBehavior::LOOP);
        }
    } else {
        animationState.wasInAir = true;
    }
    // Jumping
    if (velocity.y < 0.f) {
        // reset landing data
        landAnimation.frame = 0;
        landAnimation.runningTime = 0;
        // Jumping
        update_character_anim_state(jumpAnimation, dt,
                                    AnimEndBehavior::HOLD_LAST);
    }
};

Character::~Character() {
    UnloadTexture(idleAnimation.texture);
    UnloadTexture(jumpAnimation.texture);
    UnloadTexture(landAnimation.texture);
}

void Character::draw() const {
    DrawTexturePro(
        animationState.texture,
        animationState.animData.rect,
        {
            position.x,
            position.y,
            scaledWidth,
           scaledHeight
        },
        {},
        0.0f,
        WHITE
    );
}

void Character::spawn(const Vector2 worldPos) {
    setPosition({
       worldPos.x,
       (worldPos.y - scaledHeight)
   });
}


void Character::loadAnimation(const AnimationParams &animParams, AnimData &animData) {
    animData.texture = LoadTexture(animParams.fileName);
    SetTextureFilter(animData.texture, TEXTURE_FILTER_POINT);
    animData.rect = {
        0, 0,
        static_cast<float>(animData.texture.width) / static_cast<float>(animParams.spriteCount),
        static_cast<float>(animData.texture.height - 8)
    };
    animData.updateTime = animParams.updateTime;
    animData.spriteCount = animParams.spriteCount;
}

void Character::update_character_anim_state(
    AnimData &animData,
    const float dt,
    const AnimEndBehavior behavior
) {
    update_character_animation_data(animData, dt, behavior);
    animationState.texture = animData.texture;
    animationState.animData = animData;
}

void Character::update_character_animation_data(
    AnimData &animData,
    const float dt,
    const AnimEndBehavior behavior
) {
    animData.runningTime += dt;
    if (animData.runningTime < animData.updateTime) {
        return;
    }
    animData.runningTime = 0.0;
    animData.rect.x = animData.rect.width * static_cast<float>(
                          animData.frame);
    animData.frame++;
    if (animData.frame < animData.spriteCount) {
        return;
    }
    switch (behavior) {
        case AnimEndBehavior::LOOP:
            animData.frame = 0;
            break;

        case AnimEndBehavior::HOLD_LAST:
            animData.frame = animData.spriteCount - 1;
            break;

        case AnimEndBehavior::RESET:
            animData.frame = 0;
            animData.runningTime = 0;
            animationState.isLanding = false;
            animationState.wasInAir = false;
            break;
    }
}
