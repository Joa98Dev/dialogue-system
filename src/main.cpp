#include "raylib.h"
#include "GameManager.h"
#include <iostream>
#include <filesystem>

/**
 * @brief Main entry point for the Dialogue System game application
 * 
 * This function initializes the game window, sets up the file system context,
 * creates the GameManager, and runs the main game loop.
 * 
 * Key responsibilities:
 * - Configures working directory for asset loading
 * - Initializes raylib window with resizable flag
 * - Creates and manages the GameManager lifecycle
 * - Runs the main game loop (update → render)
 * - Handles graceful shutdown and cleanup
 * 
 * @return int Exit code (0 = successful execution)
 */
int main () {
    std::cout << "=== Game Starting ===\n";

    // ============================================================================
    // FILE SYSTEM SETUP
    // ============================================================================
    
    /**
     * Adjust working directory to project root for consistent asset loading
     * 
     * This is necessary because when running from different IDEs or build systems,
     * the executable may be located in different subdirectories (e.g., build/, Debug/, etc.)
     * Moving up two levels ensures assets are found regardless of build configuration.
     * 
     * Path navigation: current_path() / ".." / ".." moves up two directory levels
     * Example: /project/build/Debug/ → /project/
     */
    try {
        std::filesystem::current_path(std::filesystem::current_path() / ".." / "..");
        std::cout << "Changed working directory to: " << std::filesystem::current_path() << "\n";
    } 
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to change directory: " << e.what() << "\n";
        std::cout << "Continuing anyway... (assets may not load correctly)\n";
    }

    // ============================================================================
    // WINDOW INITIALIZATION
    // ============================================================================
    
    const int screenWidth = 800;    /**< Initial window width in pixels */
    const int screenHeight = 600;   /**< Initial window height in pixels */

    // Enable window resizing and disable default ESC key exit behavior
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Dialogue System");
    SetExitKey(KEY_NULL);  // Manual exit control only (prevents accidental ESC closes)
    SetTargetFPS(60);      // Lock frame rate for consistent gameplay timing

    // ============================================================================
    // GAME MANAGER SETUP
    // ============================================================================
    
    GameManager gameManager;   /**< Core game state controller */
    gameManager.Init();

    // ============================================================================
    // MAIN GAME LOOP
    // ============================================================================
    
    /**
     * Game loop structure:
     * 1. Calculate delta time (time since last frame)
     * 2. Update game logic (state machine, input, physics, etc.)
     * 3. Check if game should exit
     * 4. Render all graphics
     * 5. Repeat until window close or exit requested
     */
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();  // Time in seconds since last frame
        gameManager.Update(deltaTime);

        // Check if GameManager requested exit (e.g., from EXIT state)
        if (gameManager.WindowShouldClose()) {
            std::cout << "GameManager requested exit\n";
            break;
        }

        // Render all game graphics for this frame
        BeginDrawing();
            gameManager.Draw();
        EndDrawing();
    }
    
    // ============================================================================
    // CLEANUP & SHUTDOWN
    // ============================================================================
    
    gameManager.Cleanup();  // Release game resources
    CloseWindow();          // Close raylib window and OpenGL context

    std::cout << "=== Game Closed ===\n";
    return 0;  // Successful execution
}