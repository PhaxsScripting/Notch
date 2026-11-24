#include <SDL.h>
#include <iostream>
#include "Editor.h"
#include "Text.h"
#include "TextBuffer.h"

int runEditor() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Notch",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1100, 700,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (window == nullptr) {
        std::cerr << "Window creation failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == nullptr) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;  
    }

    SDL_RenderSetLogicalSize(renderer, 1100, 700);
    const char* fontPath = "fonts/JetBrainsMono-Regular.ttf";
    SDL_Texture* title = makeText(renderer, fontPath, "Notch", 42, {126, 231, 193, 255});
    SDL_Texture* welcome = makeText(renderer, fontPath, "Type something. Backspace deletes, obv.", 22, {190, 199, 213, 255});
    SDL_Texture* line = makeText(renderer, fontPath "", 22, {230, 233, 240, 255});

    if (title == nullptr || welcome == nullptr || line == nullptr) {
        std::cerr << "Text creation failed. Check the font path.\n";
        SDL_DestroyTexture(line);
        SDL_DestroyTexture(welcome);
        SDL_DestroyTexture(title);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TextBuffer buffer;
    bool dirty = false;
    Uint64 lastEdit = SDL_GetTicks64();
    SDL_StartTextInput();

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }

            if (event.type == SDL_TEXTINPUT && buffer.insert(event.text.text)) {
                dirty = true;
                lastEdit = SDL_GetTicks64();
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && buffer.backspace()) {
                dirty = true;
                lastEdit = SDL_GetTicks64();
            }
        }

        if (dirty) {
            SDL_Texture* updated = makeText(renderer, fontPath, buffer.text(), 22, {230, 233, 240, 255});
            if (updated == nullptr) {
                std::cerr << "Could not update the text.\n";
                break;
            }
            SDL_DestroyTexture(line);
            line = updated;
            dirty = false;
        }

        SDL_SetRenderDrawColor(renderer, 18, 20, 27, 255);
        SDL_RenderClear(renderer);
        drawText(renderer, title, 64, 64);
        drawText(renderer, welcome, 64, 130);
        drawText(renderer, line, 64, 220);

        if (((SDL_GetTicks64() - lastEdit) / 500) % 2 == 0) {
            int textWidth = 0;
            SDL_QueryTexture(line, nullptr, nullptr, &textWidth, nullptr);
            SDL_Rect caret{64 + textWidth - 22 + 2, 224, 2, 26};
            SDL_SetRenderDrawColor(renderer, 126, 231, 193, 255);
            SDL_RenderFillRect(renderer, &caret);
        }

        SDL_RenderPresent(renderer);


        SDL_Delay(16);
    }

    SDL_StopTextInput();
    SDL_DestroyTexture(line);
    SDL_DestroyTexture(welcome);
    SDL_DestroyTexture(title);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
