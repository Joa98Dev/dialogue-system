#pragma once

#include "MainMenu.h"
#include "DialogueScene.h"
#include <string>

/**
 * @enum GameState
 * @brief Defines all possible states of the game application
 * 
 * The game operates as a finite state machine where each state represents
 * a different screen or mode of operation. Transitions between states
 * occur based on user input or game logic events.
 */
enum GameState {
    MENU,       /**< Main menu screen - initial entry point */
    PLAYING,    /**< Active gameplay state */
    CREDITS,    /**< Credits screen */
    DIALOGUE,   /**< Dialogue interaction state */
    EXIT        /**< Exit state - triggers application shutdown */
};

/**
 * @class GameManager
 * @brief Central controller that manages the game's state machine and overall flow
 * 
 * The GameManager serves as the core orchestrator of the application. It:
 * - Maintains the current game state (MENU, PLAYING, CREDITS, DIALOGUE, EXIT)
 * - Delegates update and draw calls to the appropriate subsystems
 * - Manages transitions between different game screens
 * - Handles window resize events and resource cleanup
 * - Coordinates dialogue system integration
 */
class GameManager {
    private:
        // ====================================================================
        // State Management
        // ====================================================================
        
        GameState currentState;          /**< Current active state of the game */
        bool windowShouldClose;         /**< Flag indicating if the window should terminate */

        // ====================================================================
        // Subsystem References
        // ====================================================================

        MainMenu* mainMenu;             /**< Pointer to the main menu interface */
        DialogueScene* dialogueScene;   /**< Pointer to the dialogue system manager */
        
        // ====================================================================
        // Window Configuration
        // ====================================================================

        int screenWidth;        /**< Current window width in pixels */
        int screenHeight;       /**< Current window height in pixels */

    public:
        // ====================================================================
        // Constructor & Destructor
        // ====================================================================

        /**
         * @brief Constructs a new GameManager instance
         * 
         * Initializes member variables to default values but does NOT
         * allocate resources or create subsystems. Call Init() separately.
         */
        GameManager();

                /**
         * @brief Destroys the GameManager and cleans up resources
         * 
         * Automatically calls Cleanup() to ensure proper deallocation
         * of dynamically allocated subsystems.
         */
        ~GameManager();

        // ====================================================================
        // Lifecycle Methods
        // ====================================================================
        
        /**
         * @brief Initializes all game subsystems and prepares for execution
         * 
         * Allocates memory for subsystems (MainMenu, DialogueScene),
         * sets up initial game state, and performs any one-time setup operations.
         * Must be called before Update() or Draw().
         */
        void Init();

        /**
         * @brief Updates the current game state based on elapsed time
         * @param deltaTime Time elapsed since the last frame in seconds
         * 
         * Delegates update logic to the appropriate state handler based on
         * currentState. Should be called once per frame.
         */
        void Update(float deltaTime);

        /**
         * @brief Renders all visual elements for the current game state
         * 
         * Delegates drawing calls to the appropriate state renderer.
         * Should be called after Update() each frame.
         */
        void Draw();

        /**
         * @brief Releases all allocated resources and performs shutdown cleanup
         * 
         * Deletes dynamically allocated objects, saves persistent data,
         * and prepares the application for termination.
         */
        void Cleanup();

        // ====================================================================
        // Public Accessors & Utilities
        // ====================================================================
        
        /**
         * @brief Checks if the window should close
         * @return true if application should terminate, false otherwise
         * 
         * Used by the main game loop to determine when to exit.
         */
        bool WindowShouldClose();

        /**
         * @brief Triggers the dialogue system with content from a JSON file
         * @param jsonFile Path to the JSON file containing dialogue data
         * 
         * Loads and parses the specified JSON file, initializes the dialogue
         * scene, and transitions the game to DIALOGUE state.
         */
        void StartDialogueFromFile(const std::string& jsonFile);

        /**
         * @brief Detects and handles window resize events
         * 
         * Should be called each frame to check for resolution changes
         * and propagate new dimensions to all active subsystems.
         */
        void CheckWindowResize();
    
    private:

        // ====================================================================
        // State Update Handlers
        // ====================================================================
        
        /**
         * @brief Updates logic for the MENU state
         * 
         * Processes menu navigation, button hover states, and state transitions
         * based on user selection (Play, Credits, Exit).
         */
        void UpdateMenu();

        /**
         * @brief Updates logic for the PLAYING state
         * 
         * Handles core gameplay mechanics, player input, collision detection,
         * and game world updates.
         */
        void UpdatePlaying();

                /**
         * @brief Updates logic for the DIALOGUE state
         * 
         * Advances dialogue progression, processes player choices,
         * and detects when dialogue sequences complete.
         */
        void UpdateDialogue();

        /**
         * @brief Updates logic for the CREDITS state
         * 
         * Handles scrolling credits animation or waits for user input
         * to return to the main menu.
         */
        void UpdateCredits();


       // ====================================================================
        // State Render Handlers
        // ====================================================================
        
        /**
         * @brief Renders the main menu interface
         */
        void DrawMenu();
        
        /**
         * @brief Renders the active gameplay screen
         */
        void DrawPlaying();
        
        /**
         * @brief Renders the credits screen
         */
        void DrawCredits();
        
        /**
         * @brief Renders the dialogue/cutscene interface
         */
        void DrawDialogue();
        
        // ====================================================================
        // Event Handlers
        // ====================================================================
        
        /**
         * @brief Callback invoked when a dialogue sequence ends
         * 
         * Handles post-dialogue logic such as returning to previous state,
         * triggering rewards, or advancing the game narrative.
         */
        void OnDialogueEnd();
        
        /**
         * @brief Configures and initializes all dialogue scenes
         * 
         * Sets up dialogue triggers, character data, and scene transitions
         * for the entire game. Called once during initialization.
         */
        void SetupDialogueScenes();
};