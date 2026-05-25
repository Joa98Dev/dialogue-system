#include "MainMenu.h"
#include <iostream>

// ============================================================================
// Constructor & Initialization
// ============================================================================

/**
 * @brief Constructs the main menu with initial screen dimensions
 * @param width Initial screen width in pixels
 * @param height Initial screen height in pixels
 * 
 * Stores screen dimensions for later button positioning.
 * Does NOT calculate button positions - call Init() separately.
 */
MainMenu::MainMenu(int width, int height) 
    : screenWidth(width), screenHeight(height) {
    std::cout << "MainMenu constructor called\n";
}

/**
 * @brief Initializes the main menu and calculates button positions
 * 
 * Must be called after construction and before any Draw() operations.
 * Calculates button positions based on the current screen dimensions.
 */
void MainMenu::Init() {
    UpdateButtonPositions();
    std::cout << "MainMenu Init called\n";
}

// ============================================================================
// Layout & Positioning
// ============================================================================

/**
 * @brief Recalculates all button positions based on current screen size
 * 
 * Uses percentage-based dimensions (buttonWidthPercent, buttonHeightPercent,
 * buttonSpacingPercent) to calculate button rectangles that scale with screen.
 * Also adjusts font size proportionally to screen height.
 * 
 * Button layout:
 * - All buttons centered horizontally
 * - Vertically stacked with equal spacing
 * - Entire button group centered on screen
 */
void MainMenu::UpdateButtonPositions() {
    // Calculate dynamic button sizes based on screen dimensions
    float buttonWidth = screenWidth * buttonWidthPercent;
    float buttonHeight = screenHeight * buttonHeightPercent;
    float buttonSpacing = screenHeight * buttonSpacingPercent;
    
    // Center buttons horizontally
    float centerX = (screenWidth - buttonWidth) / 2;
    
    // Calculate Y positions (centered vertically with spacing)
    float totalHeight = (buttonHeight * 3) + (buttonSpacing * 2);
    float startY = (screenHeight - totalHeight) / 2;
    
    // Update button rectangles
    playButton = {centerX, startY, buttonWidth, buttonHeight};
    creditsButton = {centerX, startY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight};
    quitButton = {centerX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight};
    
    // Adjust font size based on screen height (clamped between 20-50 pixels)
    fontSize = screenHeight / 20;
    if (fontSize < 20) fontSize = 20;
    if (fontSize > 50) fontSize = 50;
    
    std::cout << "Button positions updated: " 
        << screenWidth << "x" << screenHeight 
        << " screen, font size: " << fontSize << "\n";
}

/**
 * @brief Handles window resize events
 * @param newWidth New screen width in pixels
 * @param newHeight New screen height in pixels
 * 
 * Updates internal screen dimensions and recalculates button positions.
 */
void MainMenu::Resize(int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
    UpdateButtonPositions();
}

// ============================================================================
// Update & Input Handling
// ============================================================================

/**
 * @brief Updates menu logic (currently empty)
 * 
 * Input handling is done in GetSelectedOption() rather than Update()
 * because menu doesn't require continuous update logic.
 */
void MainMenu::Update() {
    // No continuous update needed - GetSelectedOption handles input polling
}

/**
 * @brief Checks if mouse cursor is hovering over a button
 * @param button The button rectangle to test
 * @param mousePoint Current mouse cursor position
 * @return true if mouse is inside button bounds, false otherwise
 */
bool MainMenu::isHovering(const Rectangle& button, Vector2 mousePoint) {
    return CheckCollisionPointRec(mousePoint, button);
}

/**
 * @brief Processes button clicks and returns selected option
 * @return int Selected option: 0=Play, 1=Credits, 2=Quit, -1=None
 * 
 * Called each frame to detect mouse clicks on menu buttons.
 * Returns -1 when no button is clicked, allowing the GameManager
 * to only react when a valid selection is made.
 */
int MainMenu::GetSelectedOption() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        
        if (isHovering(playButton, mousePoint)) {
            std::cout << "Play button clicked!\n";
            return 0;
        }
        if (isHovering(creditsButton, mousePoint)) {
            std::cout << "Credits button clicked!\n";
            return 1;
        }
        if (isHovering(quitButton, mousePoint)) {
            std::cout << "Quit button clicked!\n";
            return 2;
        }
    }
    return -1;  // No button clicked this frame
}

// ============================================================================
// Rendering
// ============================================================================

/**
 * @brief Renders the entire main menu interface
 * 
 * Draws in order:
 * 1. Game title (responsive font size)
 * 2. Play button (with hover color change)
 * 3. Credits button (with hover color change)
 * 4. Quit button (with hover color change)
 * 
 * Button text is automatically centered both horizontally and vertically.
 */
void MainMenu::Draw() {
    // Draw title with responsive font size (clamped between 30-80 pixels)
    const char* title = "Dialogue System";
    int titleFontSize = screenHeight / 12;
    if (titleFontSize < 30) titleFontSize = 30;
    if (titleFontSize > 80) titleFontSize = 80;
    
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (screenWidth - titleWidth) / 2, screenHeight * 0.1f, titleFontSize, WHITE);
    
    // Get mouse position once for all hover checks
    Vector2 mousePoint = GetMousePosition();
    
    // ===== PLAY BUTTON =====
    Color currentPlayColor = isHovering(playButton, mousePoint) ? RED : BLUE;
    DrawRectangleRec(playButton, currentPlayColor);
    DrawRectangleLinesEx(playButton, 3, DARKBLUE);
    
    // Center text within button
    int playTextWidth = MeasureText(playText, fontSize);
    DrawText(playText, 
             playButton.x + (playButton.width - playTextWidth) / 2,
             playButton.y + (playButton.height - fontSize) / 2,
             fontSize, WHITE);
    
    // ===== CREDITS BUTTON =====
    Color currentCreditsColor = isHovering(creditsButton, mousePoint) ? RED : BLUE;
    DrawRectangleRec(creditsButton, currentCreditsColor);
    DrawRectangleLinesEx(creditsButton, 3, DARKBLUE);
    
    int creditsTextWidth = MeasureText(creditsText, fontSize);
    DrawText(creditsText,
             creditsButton.x + (creditsButton.width - creditsTextWidth) / 2,
             creditsButton.y + (creditsButton.height - fontSize) / 2,
             fontSize, WHITE);
    
    // ===== QUIT BUTTON =====
    Color currentQuitColor = isHovering(quitButton, mousePoint) ? RED : BLUE;
    DrawRectangleRec(quitButton, currentQuitColor);
    DrawRectangleLinesEx(quitButton, 3, DARKBLUE);
    
    int quitTextWidth = MeasureText(quitText, fontSize);
    DrawText(quitText,
             quitButton.x + (quitButton.width - quitTextWidth) / 2,
             quitButton.y + (quitButton.height - fontSize) / 2,
             fontSize, WHITE);
}

// ============================================================================
// Cleanup
// ============================================================================

/**
 * @brief Releases any allocated resources
 * 
 * MainMenu doesn't allocate any dynamic resources, so this is primarily
 * a placeholder for consistency with other game systems.
 */
void MainMenu::Cleanup() {
    std::cout << "MainMenu Cleanup called\n";
}