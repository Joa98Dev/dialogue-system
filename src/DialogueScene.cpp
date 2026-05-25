#include "DialogueScene.h"
#include "../libs/json.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <limits.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

// ============================================================================
// Constructor & Destructor
// ============================================================================

/**
 * @brief Constructs a dialogue scene with specified screen dimensions
 * @param width Initial screen width in pixels
 * @param height Initial screen height in pixels
 */
DialogueScene::DialogueScene(int width, int height)
    : currentLineIndex(0), isActive(false), autoAdvanceTimer(0),
      currentBackground({0}), currentCharacter({0}), 
      screenWidth(width), screenHeight(height),
      letterTimer(0), visibleCharCount(0), isPrintingText(false),
      textSpeed(30.0f), fontSize(24), textColor(WHITE), 
      boxColor(ColorAlpha(BLACK, 0.85f)), onDialogueEnd(nullptr),
      fadeAlpha(0.0f), fadeSpeed(2.0f), isFadingIn(false), isFadingOut(false) {
    
    UpdateUIPositions();
}

/**
 * @brief Destructor - automatically unloads textures
 */
DialogueScene::~DialogueScene() {
    Unload();
}

// ============================================================================
// UI Layout Management
// ============================================================================

/**
 * @brief Recalculates UI element positions based on current screen dimensions
 * 
 * Updates text box, portrait box, and font size dynamically when the window
 * is resized or during initialization.
 */
void DialogueScene::UpdateUIPositions() {
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    
    // Text box occupies 80% of screen width, 20% of screen height
    float boxWidth = (float)screenWidth * 0.8f;
    float boxHeight = (float)screenHeight * 0.2f;
    float boxX = ((float)screenWidth - boxWidth) / 2.0f;
    float boxY = (float)screenHeight - boxHeight - 20.0f;
    
    textBox = {boxX, boxY, boxWidth, boxHeight};
    
    // Portrait is 70% of text box height, positioned at top-left of text box
    float portraitSize = boxHeight * 0.7f;
    portraitBox = {textBox.x + 10.0f, textBox.y + 10.0f, portraitSize, portraitSize};
    
    // Font size scales with screen height (clamped between 16-32 pixels)
    fontSize = (int)std::clamp((float)screenHeight / 25.0f, 16.0f, 32.0f);
}

// ============================================================================
// JSON Loading & Parsing
// ============================================================================

/**
 * @brief Loads dialogue sequence from a JSON file
 * @param filename Name of the JSON file in assets/dialogues/ directory
 * @return true if file loaded and parsed successfully, false otherwise
 * 
 * Automatically resolves the executable path to construct absolute paths,
 * making the game portable across different machines and operating systems.
 */
bool DialogueScene::LoadFromJSON(const std::string& filename) {
    try {
        // Get executable path (works across different computers/OS)
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        fs::path executablePath;
        
        if (count != -1) {
            executablePath = fs::path(std::string(result, count)).parent_path();
        } else {
            // Fallback for Windows or if readlink fails
            executablePath = fs::current_path();
        }
        
        // Build RELATIVE path to executable (NOT absolute system path)
        fs::path fullPath = executablePath / "assets" / "dialogues" / filename;
        fullPath = fullPath.lexically_normal();
        
        std::cout << "Loading: " << fullPath << std::endl;
        
        // Validate file exists before attempting to open
        if (!fs::exists(fullPath)) {
            std::cerr << "ERROR: File not found: " << fullPath << std::endl;
            return false;
        }
        
        // Open and parse the JSON file
        std::ifstream file(fullPath);
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not open file: " << fullPath << std::endl;
            return false;
        }
        
        json data;
        file >> data;
        
        // Clear existing dialogue and load new lines from JSON
        dialogueLine.clear();
        for (const auto& item : data["lines"]) {
            DialogueLine line;
            line.text = item.value("text", "");
            line.characterName = item.value("character_name", "");
            line.bgImage = item.value("background", "");
            line.characterImage = item.value("portrait", "");
            line.autoAdvance = item.value("auto_advance", 0.0f);
            dialogueLine.push_back(line);
        }
        
        std::cout << "SUCCESS: Loaded " << dialogueLine.size() << " lines\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// Dialogue Flow Control
// ============================================================================

/**
 * @brief Begins playing the loaded dialogue sequence
 * 
 * Resets all animation states, loads the first line's assets,
 * and initiates fade-in transition.
 */
void DialogueScene::Start() {
    if (dialogueLine.empty()) return;
    
    isActive = true;
    currentLineIndex = 0;
    visibleCharCount = 0;
    isPrintingText = true;
    letterTimer = 0;
    
    StartFadeIn();

    // Load initial background and character portrait if specified
    if (!dialogueLine[0].bgImage.empty()) LoadBackground(dialogueLine[0].bgImage);
    if (!dialogueLine[0].characterImage.empty()) LoadCharacterPortrait(dialogueLine[0].characterImage);
}

/**
 * @brief Updates dialogue logic, animations, and input handling
 * @param deltaTime Time elapsed since last frame in seconds
 */
void DialogueScene::Update(float deltaTime) {
    UpdateFade(deltaTime);

    // Don't process input during transitions
    if (!isActive && !isFadingOut && fadeAlpha >= 1.0f) return;
    if (isFadingIn || isFadingOut || !isActive) return;
    
    // Typewriter text animation
    if (isPrintingText) {
        const std::string& currentText = dialogueLine[currentLineIndex].text;
        if (visibleCharCount < (int)currentText.length()) {
            letterTimer += deltaTime;
            if (letterTimer >= 1.0f / textSpeed) {
                letterTimer = 0;
                visibleCharCount++;
            }
        } else {
            isPrintingText = false;  // Text fully revealed
        }
    } else {
        // Wait for user input to advance after text is complete
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AdvanceToNextLine();
        }
    }
}

// ============================================================================
// Rendering
// ============================================================================

/**
 * @brief Renders all dialogue UI elements
 * 
 * Draws background, text box, character portrait, character name,
 * and typewriter text. Updates UI positioning each frame to handle resizing.
 */
void DialogueScene::Draw() {
    if (!isActive && !isFadingOut) return;

    UpdateUIPositions();
    
    // Draw background image if loaded
    if (currentBackground.id != 0) {
        DrawTexturePro(currentBackground, 
            {0, 0, (float)currentBackground.width, (float)currentBackground.height},
            {0, 0, (float)screenWidth, (float)screenHeight}, {0, 0}, 0, WHITE);
    }

    // Draw dialogue text box with gold border
    DrawRectangleRec(textBox, boxColor);
    DrawRectangleLinesEx(textBox, 2, GOLD);
    
    DrawPortrait();
    DrawCharacterName();
    
    // Render typewriter text with line wrapping
    if (currentLineIndex < dialogueLine.size()) {
        std::string displayText = dialogueLine[currentLineIndex].text.substr(0, visibleCharCount);
        std::vector<std::string> wrappedLines;
        WrapText(displayText, wrappedLines, (int)(textBox.width - portraitBox.width - 30));
        
        // Display up to 4 wrapped lines
        for (size_t i = 0; i < wrappedLines.size() && i < 4; i++) {
            DrawText(wrappedLines[i].c_str(), 
                     (int)(portraitBox.x + portraitBox.width + 15), 
                     (int)(textBox.y + 15 + (i * (fontSize + 8))), 
                     fontSize, textColor);
        }
    }
    DrawFade();
}

// ============================================================================
// Text Utilities
// ============================================================================

/**
 * @brief Wraps text to fit within a maximum pixel width
 * @param text Input text to wrap
 * @param wrappedLines Output vector of wrapped line strings
 * @param maxWidth Maximum pixel width before wrapping
 * 
 * Uses word-boundary wrapping (doesn't cut words in half).
 */
void DialogueScene::WrapText(const std::string& text, std::vector<std::string>& wrappedLines, int maxWidth) {
    wrappedLines.clear();
    std::string currentLine, word;
    
    for (char c : text) {
        if (c == ' ') {
            std::string test = currentLine.empty() ? word : currentLine + " " + word;
            if (MeasureText(test.c_str(), fontSize) <= maxWidth) {
                currentLine = test;
            } else {
                if (!currentLine.empty()) wrappedLines.push_back(currentLine);
                currentLine = word;
            }
            word.clear();
        } else {
            word += c;
        }
    }
    
    // Handle remaining text after last space
    if (!word.empty()) {
        if (MeasureText((currentLine + " " + word).c_str(), fontSize) <= maxWidth) {
            currentLine += (currentLine.empty() ? "" : " ") + word;
        } else {
            wrappedLines.push_back(currentLine);
            currentLine = word;
        }
    }
    
    if (!currentLine.empty()) wrappedLines.push_back(currentLine);
}

// ============================================================================
// UI Component Rendering
// ============================================================================

/**
 * @brief Draws the character portrait scaled to fit portraitBox
 * 
 * Maintains aspect ratio while fitting within the portrait box area.
 */
void DialogueScene::DrawPortrait() {
    if (currentCharacter.id != 0) {
        float scale = std::min(portraitBox.width / currentCharacter.width, 
                               portraitBox.height / currentCharacter.height);
        DrawTexturePro(currentCharacter, 
            {0, 0, (float)currentCharacter.width, (float)currentCharacter.height},
            {portraitBox.x + (portraitBox.width - currentCharacter.width * scale) / 2, 
             portraitBox.y + (portraitBox.height - currentCharacter.height * scale) / 2, 
             currentCharacter.width * scale, currentCharacter.height * scale}, 
            {0,0}, 0, WHITE);
    }
}

/**
 * @brief Draws the character name tag above the text box
 */
void DialogueScene::DrawCharacterName() {
    if (currentLineIndex < dialogueLine.size()) {
        const std::string& name = dialogueLine[currentLineIndex].characterName;
        if (!name.empty()) {
            int nameWidth = MeasureText(name.c_str(), fontSize + 4);
            DrawRectangleRec({textBox.x + 120, textBox.y - 30, (float)nameWidth + 20, 30}, 
                            ColorAlpha(BLACK, 0.85f));
            DrawText(name.c_str(), (int)textBox.x + 130, (int)textBox.y - 25, 
                    fontSize + 4, GOLD);
        }
    }
}

// ============================================================================
// Asset Management
// ============================================================================

/**
 * @brief Loads and resizes background texture to fit screen
 * @param path File path to background image
 */
void DialogueScene::LoadBackground(const std::string& path) {
    if (currentBackground.id != 0) UnloadTexture(currentBackground);
    Image img = LoadImage(path.c_str());
    if (img.data) {
        ImageResize(&img, screenWidth, screenHeight);
        currentBackground = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

/**
 * @brief Loads character portrait texture (maintains original size)
 * @param path File path to portrait image
 */
void DialogueScene::LoadCharacterPortrait(const std::string& path) {
    if (currentCharacter.id != 0) UnloadTexture(currentCharacter);
    Image img = LoadImage(path.c_str());
    if (img.data) {
        currentCharacter = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

/**
 * @brief Advances to the next dialogue line or ends the scene
 * 
 * Moves to next line if available, otherwise triggers fade-out
 * and calls the end callback.
 */
void DialogueScene::AdvanceToNextLine() {
    currentLineIndex++;
    if (currentLineIndex >= dialogueLine.size()) {
        // End of dialogue
        isActive = false;
        if (onDialogueEnd) onDialogueEnd();
        StartFadeOut();
    } else {
        // Load next line's assets
        visibleCharCount = 0;
        isPrintingText = true;
        const auto& next = dialogueLine[currentLineIndex];
        if (!next.bgImage.empty()) LoadBackground(next.bgImage);
        if (!next.characterImage.empty()) LoadCharacterPortrait(next.characterImage);
    }
}

/**
 * @brief Releases all loaded textures from video memory
 */
void DialogueScene::Unload() {
    if (currentBackground.id != 0) UnloadTexture(currentBackground);
    if (currentCharacter.id != 0) UnloadTexture(currentCharacter);
}

// ============================================================================
// Fade Effect Management
// ============================================================================

void DialogueScene::StartFadeIn() { 
    isFadingIn = true; 
    isFadingOut = false; 
    fadeAlpha = 1.0f; 
}

void DialogueScene::StartFadeOut() { 
    isFadingOut = true; 
    isFadingIn = false; 
    fadeAlpha = 0.0f; 
}

/**
 * @brief Updates fade transition alpha values
 * @param deltaTime Time elapsed since last frame
 */
void DialogueScene::UpdateFade(float deltaTime) {
    if (isFadingIn) { 
        fadeAlpha -= deltaTime * fadeSpeed; 
        if (fadeAlpha <= 0) { 
            fadeAlpha = 0; 
            isFadingIn = false; 
        } 
    }
    if (isFadingOut) { 
        fadeAlpha += deltaTime * fadeSpeed; 
        if (fadeAlpha >= 1) { 
            fadeAlpha = 1; 
            isFadingOut = false; 
        } 
    }
}

/**
 * @brief Draws semi-transparent black overlay for fade effects
 */
void DialogueScene::DrawFade() {
    if (fadeAlpha > 0) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), 
                     ColorAlpha(BLACK, fadeAlpha));
    }
}