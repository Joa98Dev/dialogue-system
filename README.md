# Dialogue System - Documentation

A flexible, visual novel-style dialogue system built with Raylib. Features include typewriter text animation, character portraits, dynamic background loading, JSON-based dialogue management, and responsive UI that adapts to window resizing.

## Project Structure

```
DialogueSystem/
├── assets/ # Game assets (images, audio)
│ ├── dialogues/ # JSON dialogue files
│ └── sprites/ # Images (backgrounds, portraits)
│ ├── backgrounds/ # Scene backgrounds
│ └── characters/ # Character portraits
├── include/ # Header files (.h)
│ ├── DialogueScene.h
│ ├── GameManager.h
│ └── MainMenu.h
├── src/ # Source files (.cpp)
│ ├── main.cpp
│ ├── DialogueScene.cpp
│ ├── GameManager.cpp
│ └── MainMenu.cpp
├── libs/ # Third-party libraries
│ └── json.hpp # JSON parser (nlohmann/json)
├── Makefile # Build system
└── build/
└── bin/
├── assets/ # Copied assets at build time
└── DialogueSystem # Compiled executable
```

## Game States

The game uses a state machine pattern to track the current screen:
```
enum GameState {
    MENU,       // Main menu screen
    PLAYING,    // Active gameplay (placeholder)
    CREDITS,    // Credits/Information screen
    DIALOGUE,   // Active dialogue/cutscene
    EXIT        // Graceful shutdown
};
```

# Dialogue System

## Creating a Dialogue File

Create a JSON file in assets/dialogues/ with the following structure:

```
{
    "scene_name": "First Encounter",
    "lines": [
        {
            "text": "Welcome to the Dialogue System!",
            "character_name": "Narrator",
            "background": "assets/sprites/backgrounds/forest.jpg",
            "portrait": "assets/sprites/characters/narrator.png",
            "auto_advance": 0
        },
        {
            "text": "This system supports typewriter text animation.",
            "character_name": "System",
            "background": "",
            "portrait": "",
            "auto_advance": 3.5
        },
        {
            "text": "Press ENTER or click to continue!",
            "character_name": "Tutorial",
            "background": "",
            "portrait": "",
            "auto_advance": 0
        }
    ]
}
```

## Dialogue Fields Explained

| Field | Type | Description |
| :--- | :--- | :--- |
| `text` | string | The dialogue text to display (supports typewriter animation) |
| `character_name` | string | Name displayed above the text box |
| `background` | string | Image path for scene background (leave empty to keep current) |
| `portrait` | string | Character portrait image path (leave empty to keep current) |
| `auto_advance` | float | Seconds until next line (0 = wait for user input) |

## Using Dialogue in Code

```
// Load and start a dialogue scene
StartDialogueFromFile("assets/dialogues/scene1.json");

// Check if dialogue is currently active
if (dialogueScene->IsActive()) {
    // Dialogue is playing - handle game pause, etc.
}

// Register a callback for when dialogue ends
dialogueScene->SetOnDialogueEnd(YourCallbackFunction);
```

# Main Menu System

## How It Works

   1. Dynamic Positioning - Buttons are positioned based on screen dimensions using percentage values

    2. Hover Detection - Buttons change color when the mouse cursor hovers over them

    3. Click Detection - Returns button identifier when clicked

    4. Responsive Design - Automatically adapts to window resize events


### Button Return Values

`GetSelectedOption()` returns:

| Return Value | Button |
| :--- | :--- |
| `0` | PLAY button |
| `1` | CREDITS button |
| `2` | QUIT button |
| `-1` | No button clicked |

## Build Commands

```
# Clean previous build artifacts
make clean

# Build the game
make

# Build and run the game
make run

# Run the compiled executable manually
./build/bin/DialogueSystem
```

## Common Tasks

### Adding a New Dialogue Scene

    1. Create a JSON file in assets/dialogues/ (e.g., chapter1.json)

    2. Structure your dialogue lines following the JSON format above

    3. Call from code:

```
    StartDialogueFromFile("assets/dialogues/chapter1.json");
```

### Adding Images

Place your images in the appropriate directories:

    Backgrounds: `assets/sprites/backgrounds/`

    Character Portraits: `assets/sprites/characters/`

Supported formats: `.png`, `.jpg`, `.jpeg`

### Customizing Button Colors

In `MainMenu.cpp`, modify these color constants:

```
buttonColor = BLUE;           // Default button color
buttonHoverColor = RED;       // Color when hovering
textColor = WHITE;            // Button text color
buttonBorderColor = DARKBLUE; // Border color
```

### Adjusting Text Speed

In `DialogueScene.cpp` constructor, modify the text speed:

```
textSpeed = 30.0f;  // Characters per second (higher = faster)
```


## Common Issues & Solutions

| Problem | Likely Cause | Solution |
| :--- | :--- | :--- |
| Images not loading | Incorrect file paths in JSON | Use relative paths from executable: `assets/sprites/backgrounds/image.jpg` |
| Dialogue not starting | Invalid JSON format | Validate JSON syntax with online validator |
| Window resize breaks UI | Missing update call | Ensure `UpdateUIPositions()` is called on resize |
| Compilation errors | Outdated build artifacts | Run `make clean` then `make` |
| Text not wrapping correctly | Font size too large | Reduce `fontSize` or increase text box width |
| Fade effects not showing | Alpha blending issues | Check that `BeginDrawing()` is called before draw operations |

## Features

    * Typewriter Text Animation - Letters appear one by one

    * Responsive UI - Adapts to any window size

    * JSON Dialogue Loading - Easy content creation without recompiling

    * Smooth Fade Transitions - Fade in/out between scenes

    * Auto-Advance Support - Configurable delay between lines

    * Dynamic Asset Loading - Load backgrounds/portraits on-the-fly

    * Hover Effects - Visual feedback on button interaction

    * Portable Build - Uses relative paths for cross-platform compatibility

## Dependencies

    * Raylib 5.5 - Graphics, input, and window management

    * nlohmann/json - JSON parsing (header-only)

    * C++17 or later - Required for filesystem library


## Credits

Built with [Raylib](https://www.raylib.com/)
    
JSON parsing by [nlohmann/json](https://github.com/nlohmann/json)
