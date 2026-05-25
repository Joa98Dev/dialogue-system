#include "GameManager.h"
#include "DialogueScene.h"
#include "raylib.h"
#include <iostream>

// ============================================================================
// Constructor & Destructor
// ============================================================================

/**
 * @brief Constructs the GameManager with default values
 * 
 * Initializes state to MENU, creates null subsystem pointers,
 * and sets default screen resolution (800x600).
 */
GameManager::GameManager()
        : currentState(MENU), mainMenu(nullptr), dialogueScene(nullptr),
        screenWidth(800), screenHeight(600), windowShouldClose(false) {
            std::cout << "GameManager Constructor - Initial state: MENU\n";
        }

/**
 * @brief Destructor - logs cleanup for debugging
 * @note Actual resource cleanup happens in Cleanup()
 */
GameManager::~GameManager() {
    std::cout << "GameManager Destructor\n";
}

// ============================================================================
// Initialization & Lifecycle
// ============================================================================

/**
 * @brief Initializes all game subsystems and prepares for execution
 * 
 * Creates and initializes the main menu and dialogue scene.
 * Must be called before any Update() or Draw() operations.
 */
void GameManager::Init() {
    std::cout << "GameManager Init called\n";
    mainMenu = new MainMenu(screenWidth, screenHeight);
    mainMenu->Init();

    // Initialize the dialogue scene
    dialogueScene = new DialogueScene(screenWidth, screenHeight);
    dialogueScene->SetOnDialogueEnd(nullptr);
    currentState = MENU;
    std::cout << "GameManager Init complete! - State " << currentState << "\n";
}

/**
 * @brief Callback function invoked when a dialogue sequence ends
 * 
 * Transitions the game state back to the main menu.
 * This function is intended to be registered with DialogueScene.
 */
void GameManager::OnDialogueEnd() {
    std::cout << "Dialogue ended, returning to menu\n";
    currentState = MENU;
}

/**
 * @brief Loads and starts a dialogue sequence from a JSON file
 * @param jsonFile Path to the JSON dialogue file (currently ignored, uses hardcoded path)
 * @todo Use the jsonFile parameter instead of hardcoded "scene1.json"
 * 
 * Loads the dialogue data, starts the scene, and transitions to DIALOGUE state.
 * Currently uses a hardcoded path for testing purposes.
 */
void GameManager::StartDialogueFromFile(const std::string& jsonFile) {
    // FIXME: Use the jsonFile parameter instead of hardcoded path
    std::string dialoguePath = "scene1.json";

    if (!dialogueScene) {
        std::cerr << "ERROR: dialogueScene is NULL!\n";
        return;
    }

    if (dialogueScene->LoadFromJSON(dialoguePath)) {
        dialogueScene->Start();
        currentState = DIALOGUE;
        std::cout << "SUCCESS: Dialogue loaded, state changed to DIALOGUE\n";
    } else {
        std::cerr << "ERROR: Failed to load dialogue\n";
    }
}

// ============================================================================
// Window Management
// ============================================================================

/**
 * @brief Detects and handles window resize events
 * 
 * Compares current screen dimensions with cached values.
 * If changed, updates all subsystems and propagates new dimensions.
 * Should be called once per frame.
 */
void GameManager::CheckWindowResize() {
    static int lastWidth = screenWidth;
    static int lastHeight = screenHeight;

    int newWidth = GetScreenWidth();
    int newHeight = GetScreenHeight();

    if (newWidth != lastWidth || newHeight != lastHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;

        // Update main menu button positions
        if (mainMenu) {
            mainMenu->Resize(screenWidth, screenHeight);
        }

        // Update dialogue scene UI layout
        if (dialogueScene) {
            dialogueScene->UpdateUIPositions();
        }

        lastWidth = screenWidth;
        lastHeight = screenHeight;
        std::cout << "Window resized to: " << screenWidth << "x" << screenHeight << "\n";
    }
}

// ============================================================================
// Main Update Loop (State Machine)
// ============================================================================

/**
 * @brief Core update loop - processes current game state
 * @param deltaTime Time elapsed since last frame in seconds
 * 
 * Checks for window resizing, then delegates to the appropriate
 * state handler based on currentState.
 */
void GameManager::Update(float deltaTime) {
    CheckWindowResize();
    if (windowShouldClose) return;

    // Debug logging every 60 frames (~1 second at 60 FPS)
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 60 == 0) {
        std::cout << "Current State: " << "(0=Menu, 1=Playing, 2=Credits, 3=Dialogue, 4=Exit)\n" << currentState;
    }

    switch (currentState)
    {
    case MENU:
        UpdateMenu();
        break;
    case PLAYING:
        UpdatePlaying();
        break;
    case CREDITS:
        UpdateCredits();
        break;
    case DIALOGUE:
        UpdateDialogue();
        break;
    case EXIT:
        std::cout << "Exit state reached\n";
        windowShouldClose = true;
        break;
    }
}

// ============================================================================
// State-Specific Update Handlers
// ============================================================================

/**
 * @brief Updates the DIALOGUE state logic
 * 
 * Processes dialogue animations, input, and auto-advance timing.
 * Returns to MENU state when dialogue completes.
 */
void GameManager::UpdateDialogue() {
    if (dialogueScene) {
        dialogueScene->Update(GetFrameTime());

        if (!dialogueScene->IsActive()) {
            std::cout << "Dialogue ended, returning to menu\n";
            currentState = MENU;
        }
    }
}

/**
 * @brief Updates the MENU state logic
 * 
 * Checks for button selections and performs state transitions:
 * - Play button (0) -> Load dialogue and switch to DIALOGUE
 * - Credits button (1) -> Switch to CREDITS
 * - Quit button (2) -> Switch to EXIT
 */
void GameManager::UpdateMenu() {
    int selected = mainMenu->GetSelectedOption();

    if (selected == 0) {
        std::cout << "Starting game - loading scene1.json\n";
        StartDialogueFromFile("assets/dialogues/scene1.json");
    } else if (selected == 1) {
        std::cout << "Changing state: MENU -> CREDITS\n";
        currentState = CREDITS;
    } else if (selected == 2) {
        std::cout << "Changing state: MENU -> EXIT\n";
        currentState = EXIT;
    }
}

/**
 * @brief Updates the PLAYING state logic (placeholder gameplay)
 * @todo Replace with actual gameplay systems
 * 
 * Currently provides test functionality:
 * - SPACE key returns to menu
 * - '1' key triggers dialogue test
 */
void GameManager::UpdatePlaying() {
    if (IsKeyPressed(KEY_SPACE)) {
        std::cout << "SPACE pressed - Returning to MENU\n";
        currentState = MENU;
    }

    if (IsKeyPressed(KEY_ONE)) {
        StartDialogueFromFile("assets/dialogues/scene1.json");
    }
}

/**
 * @brief Updates the CREDITS state logic
 * 
 * Waits for user input (ESC or SPACE) to return to the main menu.
 */
void GameManager::UpdateCredits() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
        std::cout << "ESC/SPACE pressed - Returning to MENU\n";
        currentState = MENU;
    }
}

// ============================================================================
// Rendering (Draw Methods)
// ============================================================================

/**
 * @brief Main draw loop - renders current game state
 * 
 * Clears the screen and delegates drawing to the appropriate
 * state handler based on currentState.
 */
void GameManager::Draw() {
    ClearBackground(BLACK);
    
    switch (currentState) {
        case MENU:
            DrawMenu();
            break;
        case PLAYING:
            DrawPlaying();
            break;
        case CREDITS:
            DrawCredits();
            break;
        case DIALOGUE:
            DrawDialogue();
            break;
        case EXIT:
            break;  // Nothing to draw when exiting
    }
}

// ============================================================================
// State-Specific Draw Handlers
// ============================================================================

/**
 * @brief Renders the main menu interface
 */
void GameManager::DrawMenu() {
    if (mainMenu) {
        mainMenu->Draw();
    } else {
        DrawText("MainMenu is NULL!", 10, 10, 20, RED);
    }
}

/**
 * @brief Renders the placeholder gameplay screen
 * 
 * Displays instructional text for testing navigation and dialogue triggers.
 * @todo Replace with actual gameplay rendering
 */
void GameManager::DrawPlaying() {
    DrawText("GAMEPLAY MODE", screenWidth/2 - MeasureText("GAMEPLAY MODE", 30)/2, 100, 30, GREEN);
    DrawText("Press SPACE to return to menu", screenWidth/2 - MeasureText("Press SPACE to return to menu", 20)/2, 200, 20, YELLOW);
    DrawText("Press '1' for dialogue test", screenWidth/2 - MeasureText("Press '1' for dialogue test", 20)/2, 300, 20, LIGHTGRAY);
}

/**
 * @brief Renders the credits screen with scrolling text (static version)
 * 
 * Displays a semi-transparent black background with centered credits text.
 * @todo Consider adding scrolling animation for longer credit sequences
 */
void GameManager::DrawCredits() {
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.95f));
    
    const char* creditsText[] = {
        "=== GAME CREDITS ===",
        "",
        "Developed by: Joa98",
        "Powered by: Raylib 5.5",
        "",
        "Special Thanks to:",
        "- The Raylib community",
        "",
        "Press SPACE or ESC to return"
    };
    
    int lineHeight = 35;
    int startY = 120;
    
    for (int i = 0; i < 9; i++) {
        DrawText(creditsText[i], 
                 screenWidth/2 - MeasureText(creditsText[i], 20)/2, 
                 startY + (i * lineHeight), 
                 20, WHITE);
    }
}

/**
 * @brief Renders the dialogue scene
 */
void GameManager::DrawDialogue() {
    if (dialogueScene) {
        dialogueScene->Draw();
    }
}

// ============================================================================
// Cleanup & Resource Management
// ============================================================================

/**
 * @brief Releases all allocated resources and performs shutdown cleanup
 * 
 * Deletes mainMenu and dialogueScene, sets pointers to nullptr,
 * and prepares the application for termination.
 */
void GameManager::Cleanup() {
    std::cout << "GameManager Cleanup called\n";
    if (mainMenu) {
        mainMenu->Cleanup();
        delete mainMenu;
        mainMenu = nullptr;
    }
    if (dialogueScene) {
        dialogueScene->Unload();
        delete dialogueScene;
        dialogueScene = nullptr;
    }
}

/**
 * @brief Checks if the window should close
 * @return true if application should terminate, false otherwise
 */
bool GameManager::WindowShouldClose() {
    return windowShouldClose;
}
