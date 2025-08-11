/*******************************************************************************************
*
*   raylib gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 5.0
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <iterator>
#include "Character.h"
#include "Player.h"

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for: 

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

// TODO: Define your custom data types here

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static constexpr int windowWidth{16 * 80};
static constexpr int windowHeight{9 * 80};
constexpr float scale{4.0f};
static RenderTexture2D target = { 0 };  // Render texture to render our game

// TODO: Define global variables here, recommended to make them static

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    SetWindowState(FLAG_WINDOW_HIGHDPI); // Enable HiDPI if needed
    InitWindow(windowWidth, windowHeight, "raylib gamejam template");

    
    // TODO: Load resources / Initialize variables at this point
    auto character = Character(
      CharacterAnimations{
          {"resources/character/Idle/Player Idle 48x48.png", 10, 1.f / 10.f},
          {"resources/character/Jump/player new jump 48x48.png", 6, 1.f / 10.f},
          {"resources/character/Land/player land 48x48.png", 9, 1.f / 10.f}
      },
      scale);
    constexpr int ground = windowHeight;
    character.spawn({(windowWidth - 36 * 4.f) / 4.f, ground});
    
    // Render texture to draw full screen, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(windowWidth, windowHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        const PlayerInput input = Player::getInput();
        const float dt = {GetFrameTime()};
        // character tick
        character.apply_velocity(input, ground, dt);
        character.move(dt);
        character.animate(dt, ground);
        character.draw();
        EndDrawing();
    }
#endif
    CloseWindow();
    return 0;
}