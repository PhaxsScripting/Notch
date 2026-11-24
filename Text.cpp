#include "Text.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>
#include <iostream>

SDL_Texture* makeText(SDL_Renderer* renderer, const char* fontPath, std::string_view text, int size, SDL_Color color) {
    if (renderer == nullptr || size <= 0) {
        return nullptr;
    }

    FT_Library library = nullptr;
    if (FT_Init_FreeType(&library) != 0) {
        std::cerr << "Could not init FreeType.\n";
        return nullptr;
    }

    FT_Face face = nullptr;
    if (FT_New_Face(library, fontPath, 0, &face) != 0) {
        std::cerr << "Could not open font: " << fontPath << '\n';
        FT_Done_FreeType(library);
        return nullptr;
    }

    auto finishFont = [&]() {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
    };

    if (FT_Set_Pixel_Sizes(face, 0, size) != 0) {
        finishFont();
        return nullptr;
    }

    // This First version supports one line of printable ASCII text
    int width = size;
    for (unsigned char character : text) {
        if (character < 32 || character > 126 || FT_Load_Char(face, character, FT_LOAD_RENDER) != 0) {
            std::cerr << "Could not render character.\n";
            finishFont();
            return nullptr;
        }
        width += face->glyph->advance.x >> 6;
    }

    const int height = std::max(1, int(face->size->metrics.height >> 6));
    const int baseline = face->size->metrics.ascender >> 6;
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(
        0, width, height, 32, SDL_PIXELFORMAT_RGBA32
    );

    if (surface == nullptr) {
        finishFont();
        return nullptr;
    }

    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    if (SDL_LockSurface(surface) != 0) {
        SDL_FreeSurface(surface);
        finishFont();
        return nullptr;
    }

    int penX = 0;
    for (unsigned char character : text) {
        if (FT_Load_Char(face, character, FT_LOAD_RENDER) != 0) {
            SDL_UnlockSurface(surface);
            SDL_FreeSurface(surface);
            finishFont();
            return nullptr;
        }

        const auto glyph = face->glyph;
        const auto& bitmap = glyph->bitmap;
        for (unsigned int row = 0; row < bitmap.rows; ++row) {
            const auto* source = bitmap.buffer + int(row) * bitmap.pitch;
            for (unsigned int column = 0; column < bitmap.width; ++column) {
                const int x = penX + glyph->bitmap_left + int(column);
                const int y = baseline - glyph->bitmap_top + int(row);
                if (x < 0 || x >= width || y < 0 || y >= height) {
                    continue;
                }
                auto* pixels = reinterpret_cast<Uint32*>(
                    static_cast<Uint8*>(surface->pixels) + y * surface->pitch
                );

                const Uint8 alpha = source[column] * color.a / 255;
                pixels[x] = SDL_MapRGBA(surface->format, color.r, color.g, color.b, alpha);

            }
        }
        penX += glyph->advance.x >> 6;
    }

    SDL_UnlockSurface(surface);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    finishFont();
    if (texture != nullptr) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }
    return texture;
}

void drawText(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y) {
    if (texture == nullptr) {
        return;
    }
    SDL_Rect destination{x, y, 0, 0};
    if (SDL_QueryTexture(texture, nullptr, nullptr, &destination.w, &destination.h) == 0) {
        SDL_RenderCopy(renderer, texture, nullptr, &destination);
    }
}
