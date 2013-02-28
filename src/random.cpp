/*
    Copyright (C) 2012-2013 by Michael Kristofik <kristo605@gmail.com>
    Part of the libsdl-demos project.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    or at your option any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY.
 
    See the COPYING.txt file for more details.
*/
#include "HexGrid.h"
#include "Minimap.h"
#include "RandomMap.h"
#include "algo.h"
#include "hex_utils.h"
#include "sdl_helper.h"
#include "terrain.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

extern "C" int SDL_main(int, char **)  // 2-arg form is required by SDL
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        std::cerr << "Error initializing SDL: " << SDL_GetError();
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        std::cerr << "Error initializing SDL_image: " << IMG_GetError();
        return EXIT_FAILURE;
    }
    atexit(IMG_Quit);

    // Have to do this prior to SetVideoMode.
    auto icon = make_surface(IMG_Load("../img/icon.png"));
    if (icon != nullptr) {
        SDL_WM_SetIcon(icon.get(), nullptr);
    }
    else {
        std::cerr << "Warning: error loading icon file: " << IMG_GetError();
    }

    screen = SDL_SetVideoMode(1112, 704, 0, SDL_SWSURFACE);
    if (screen == nullptr) {
        std::cerr << "Error setting video mode: " << SDL_GetError();
        return EXIT_FAILURE;    
    }
    SDL_WM_SetCaption("Random Map Test", "");

    // Display area sized to hold 16x9 hexes.
    SDL_Rect mapArea = {10, 10, 882, 684};
    RandomMap m(18, 11, mapArea);
    SDL_Rect minimapArea = {902, 10, 200, 167};
    Minimap mini(m, minimapArea);

    // TODO: unit tests for this would require an SDL main.  These assume the
    // map is drawn in the upper left corner of the screen.
    /*
    assert(str(m.getHexAtS(-1, -1)) == str(hInvalid));
    assert(str(m.getHexAtS(0, 0)) == str({-1, -1}));
    assert(str(m.getHexAtS(36, 36)) == str({0, 0}));
    assert(str(m.getHexAtS(36, 108)) == str({0, 1}));
    assert(str(m.getHexAtS(90, 144)) == str({1, 1}));
    */

    m.draw(0, 0);
    mini.draw();
    SDL_Rect miniBox = mini.drawBoundingBox();
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    bool isDone = false;
    SDL_Event event;
    while (!isDone) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT &&
                    insideRect(event.button.x, event.button.y, minimapArea))
                {
                    // Try to place the center of the bounding box at the mouse
                    // cursor.
                    Sint16 tgtX = event.button.x - miniBox.w / 2;
                    Sint16 tgtY = event.button.y - miniBox.h / 2;
                    auto pct = rectPct(tgtX, tgtY, minimapArea);
                    Sint16 tgtMapX = pct.first * m.pWidth();
                    Sint16 tgtMapY = pct.second * m.pHeight();
                    auto mapLimit = m.maxPixel();
                    tgtMapX = bound(tgtMapX, 0, mapLimit.first);
                    tgtMapY = bound(tgtMapY, 0, mapLimit.second);

                    m.draw(tgtMapX, tgtMapY);
                    mini.draw();
                    mini.drawBoundingBox();
                }
            }
            else if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.state & SDL_BUTTON(1)) {
                    // Try to place the center of the bounding box at the mouse
                    // cursor.
                    Sint16 tgtX = event.button.x - miniBox.w / 2;
                    Sint16 tgtY = event.button.y - miniBox.h / 2;
                    auto pct = rectPct(tgtX, tgtY, minimapArea);
                    Sint16 tgtMapX = pct.first * m.pWidth();
                    Sint16 tgtMapY = pct.second * m.pHeight();
                    auto mapLimit = m.maxPixel();
                    tgtMapX = bound(tgtMapX, 0, mapLimit.first);
                    tgtMapY = bound(tgtMapY, 0, mapLimit.second);

                    m.draw(tgtMapX, tgtMapY);
                    mini.draw();
                    mini.drawBoundingBox();
                }
            }
            /*
            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    std::cout << "Left mouse released.\n";
                }
            }
            */
            else if (event.type == SDL_QUIT) {
                isDone = true;
            }
        }
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        SDL_Delay(1);
    }

    return EXIT_SUCCESS;
}
