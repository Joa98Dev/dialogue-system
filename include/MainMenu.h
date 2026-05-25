#pragma once

#include "raylib.h"

/**
 * @class MainMenu
 * @brief Manage the main menu interface and button interactions
 * 
 * This class handles the creation, rendering, and interaction logic for the game's
 * main menu, including dynamic button positioning and hover effects
 */

class MainMenu {
private:
    // ==========================================
    // UI Element Definitions
    // ==========================================

    /** Button clickable areas (position and size) */
    Rectangle playButton;       /**< Play game button area */
    Rectangle creditsButton;    /**< Credits button area */
    Rectangle quitButton;       /**< Quit button area */
    
    /** Button display text labels */
    const char* playText = "PLAY";          /**< Label for play button */
    const char* creditsText = "CREDITS";    /**< Label for credits button */
    const char* quitText = "QUIT";          /**< Label for quit button */
    
    // ========================================================================
    // Visual Properties
    // ========================================================================

    Color buttonColor = BLUE;               /**< Default button background color */
    Color buttonHoverColor = LIGHTGRAY;     /**< Button color when mouse hovers over it */
    Color textColor = WHITE;                /**< Color of button text labels */
    int fontSize = 30;                      /**< Text size in pixels */
    
    // ========================================================================
    // Layout Configuration
    // ========================================================================

    int screenWidth;    /**< Current screen width in pixels */
    int screenHeight;   /**< Current screen height in pixels */
    
    /** Button dimensions as relative percentages (0.0 to 1.0) */
    float buttonWidthPercent = 0.25f;   /**< Button width = 25% of screen width */
    float buttonHeightPercent = 0.08f;  /**< Button height = 8% of screen height */
    float buttonSpacingPercent = 0.03f; /**< Vertical spacing = 3% of screen height */
    
    // ========================================================================
    // Private Methods
    // ========================================================================

      /**
     * @brief Checks if the mouse cursor is hovering over a specified button area
     * @param button The button rectangle to check against
     * @param mousePoint Current mouse cursor position
     * @return true if mouse is inside button bounds, false otherwise
     */

    bool isHovering(const Rectangle& button, Vector2 mousePoint);

    /**
     * @brief Recalculates button positions based on current screen dimensions
     * 
     * This method should be called whenever the window is resized or during
     * initialization to ensure buttons are properly positioned relative to
     * screen size.
     */

    void UpdateButtonPositions();  // Method to recalculate positions
    
public:
    // ========================================================================
    // Constructor & Lifecycle Methods
    // ========================================================================
    
    /**
     * @brief Constructs the main menu with specified screen dimensions
     * @param width Initial screen width in pixels
     * @param height Initial screen height in pixels
     */
    MainMenu(int width, int height);

        /**
     * @brief Initializes menu resources and sets up initial button positions
     */
    void Init();

    /**
     * @brief Updates menu logic (input handling, button states)
     * 
     * Should be called once per frame to process user interactions
     * and update hover states.
     */
    void Update();

    /**
     * @brief Renders the main menu to the screen
     * 
     * Draws all buttons, text labels, and visual effects.
     * Should be called after Update() each frame.
     */
    void Draw();

        /**
     * @brief Gets the currently selected menu option
     * @return Integer representing selected option (0=Play, 1=Credits, 2=Quit)
     */
    int GetSelectedOption();

    /**
     * @brief Cleans up any allocated resources
     * 
     * Should be called before destroying the menu or when exiting the game.
     */
    void Cleanup();

    /**
     * @brief Handles window resize events to reposition UI elements
     * @param newWidth New screen width in pixels
     * @param newHeight New screen height in pixels
     * 
     * Updates internal screen dimensions and recalculates button positions.
     */
    void Resize(int newWidth, int newHeight);  // Method for window resize
};