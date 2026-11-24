#pragma once
#include <SDL.h>
#include <string_view>

SDL_Texture* makeText(SDL_Renderer* renderer, const char* fontPath, std::string_view text, int size, SDL_Color color);

void drawText(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);
