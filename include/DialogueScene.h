#pragma once

#include "raylib.h"
#include <string>
#include <vector>

/**
 * @struct DialogueLine
 * @brief Represents a single line of dialogue in a conversation or cutscene
 * 
 * Contains all necessary data for displaying one dialogue entry including
 * character information, visuals, and timing controls.
 */
struct DialogueLine {
    std::string text;              /**< The dialogue text to display */
    std::string characterName;     /**< Name of the speaking character */
    std::string bgImage;           /**@< File path to background image (empty = keep current) */
    std::string characterImage;    /**< File path to character portrait/emote */
    float autoAdvance;             /**< Seconds before auto-advancing (0 = wait for input) */
};

/**
 * @class DialogueScene
 * @brief Manages visual novel-style dialogue sequences with text rendering and character portraits
 * 
 * Features:
 * - Typewriter text animation with configurable speed
 * - Dynamic background and character portrait loading
 * - Auto-advance timing for automatic progression
 * - Smooth fade transitions between scenes
 * - JSON-based dialogue loading
 */
class DialogueScene {
    private:
        // ====================================================================
        // Dialogue State
        // ====================================================================
        
        std::vector<DialogueLine> dialogueLine;  /**< All lines in current conversation */
        int currentLineIndex;                    /**< Index of currently displayed line */
        bool isActive;                           /**< Whether dialogue is currently playing */
        float autoAdvanceTimer;                  /**< Countdown timer for auto-advance (seconds) */
        
        // ====================================================================
        // Visual Assets
        // ====================================================================
        
        Texture2D currentBackground;     /**< Currently loaded background texture */
        Texture2D currentCharacter;      /**< Currently loaded character portrait */
        std::string currentBackgroundPath; /**< Cache of current background file path */
        std::string currentCharacterPath;  /**< Cache of current portrait file path */
        
        // ====================================================================
        // UI Layout
        // ====================================================================
        
        Rectangle textBox;      /**< Screen area where dialogue text appears */
        Rectangle portraitBox;  /**< Screen area where character portrait appears */
        float screenWidth;      /**< Current screen width in pixels */
        float screenHeight;     /**< Current screen height in pixels */
        
        // ====================================================================
        // Text Animation (Typewriter Effect)
        // ====================================================================
        
        float letterTimer;          /**< Accumulator for letter-by-letter timing */
        int visibleCharCount;       /**< Number of characters currently visible */
        bool isPrintingText;        /**< Whether text is currently animating */
        float textSpeed;            /**< Seconds per character (e.g., 0.03 = fast, 0.08 = slow) */
        
        // ====================================================================
        // Text Styling
        // ====================================================================
        
        int fontSize;               /**< Dialogue text size in pixels */
        Color textColor;            /**< Color of dialogue text */
        Color boxColor;             /**< Background color of dialogue text box */
        
        // ====================================================================
        // Scene Transition Effects
        // ====================================================================
        
        float fadeAlpha;            /**< Current fade opacity (0 = transparent, 1 = opaque) */
        float fadeSpeed;            /**< Fade in/out speed (alpha change per second) */
        bool isFadingIn;            /**< Whether currently fading from black */
        bool isFadingOut;           /**< Whether currently fading to black */
        
        // ====================================================================
        // Private Methods
        // ====================================================================
        
        void StartFadeIn();         /**< Begin fade-from-black transition */
        void StartFadeOut();        /**< Begin fade-to-black transition */
        void UpdateFade(float deltaTime); /**< Process fading animation */
        void DrawFade();            /**< Render fade overlay */
        
        // ====================================================================
        // Rendering Helpers
        // ====================================================================
        
        void DrawTextBox();         /**< Render dialogue background panel */
        void DrawPortrait();        /**@< Render character portrait in portraitBox */
        void DrawCharacterName();   /**< Render speaking character's name */
        
        /**
         * @brief Splits long text into multiple lines that fit within maxWidth pixels
         * @param text Original text to wrap
         * @param wrappedLines Output vector of wrapped text lines
         * @param maxWidth Maximum pixel width before wrapping
         */
        void WrapText(const std::string& text, std::vector<std::string>& wrappedLines, int maxWidth);
        
        /**
         * @brief Loads and caches a background texture from file
         * @param path File path to background image
         * @note Skips loading if already using the same background
         */
        void LoadBackground(const std::string& path);
        
        /**
         * @brief Loads and caches a character portrait from file
         * @param path File path to portrait image
         * @note Skips loading if already using the same portrait
         */
        void LoadCharacterPortrait(const std::string& path);
        
        // ====================================================================
        // Callbacks
        // ====================================================================
        
        void (*onDialogueEnd)();    /**< Function pointer called when dialogue completes */
    
    public:
        // ====================================================================
        // Constructor & Destructor
        // ====================================================================
        
        /**
         * @brief Creates a dialogue scene with screen dimensions
         * @param width Screen width in pixels (used for UI layout)
         * @param height Screen height in pixels
         */
        DialogueScene(int width, int height);
        
        ~DialogueScene();           /**< Automatically unloads textures */
        
        // ====================================================================
        // Public Interface
        // ====================================================================
        
        /**
         * @brief Recalculates UI element positions based on current screen size
         * @note Call this after window resize events
         */
        void UpdateUIPositions();
        
        /**
         * @brief Loads dialogue sequence from a JSON file
         * @param filename Path to JSON dialogue file
         * @return true if file loaded and parsed successfully
         */
        bool LoadFromJSON(const std::string& filename);
        
        /**
         * @brief Manually sets dialogue lines from a vector
         * @param lines Vector of DialogueLine structs to play
         */
        void LoadDialogue(const std::vector<DialogueLine>& lines);
        
        void Start();               /**< Begins playing the loaded dialogue */
        void End();                 /**< Immediately stops dialogue and triggers callback */
        void Update(float deltaTime); /**< Processes animations and auto-advance timing */
        void Draw();                /**< Renders all dialogue UI elements */
        
        bool IsActive() const { return isActive; }  /**< Returns true if dialogue is playing */
        
        /**
         * @brief Registers a callback function for when dialogue ends
         * @param callback Function to call (takes no parameters, returns void)
         */
        void SetOnDialogueEnd(void (*callback)()) { onDialogueEnd = callback; }
        
        void AdvanceToNextLine();   /**< Skips to next line (or ends dialogue if last line) */
        std::string GetCurrentSceneName();  /**< @todo Implement scene naming system */
        
        // ====================================================================
        // Fade Effect Controls
        // ====================================================================
        
        void SetFadeSpeed(float speed) { fadeSpeed = speed; }
        bool IsFading() const { return isFadingIn || isFadingOut; }
        
        /**
         * @brief Releases all loaded textures from VRAM
         * @note Called automatically by destructor
         */
        void Unload();
};