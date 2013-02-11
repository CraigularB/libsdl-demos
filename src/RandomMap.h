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
#ifndef RANDOM_MAP_H
#define RANDOM_MAP_H

#include "HexGrid.h"
#include "SDL_stdinc.h"
#include "SDL_video.h"
#include "hex_utils.h"
#include "terrain.h"
#include <vector>

class RandomMap
{
public:
    // Create a map and define the visible portion on the screen.  Minimum size
    // is 2x1.
    RandomMap(Sint16 hWidth, Sint16 hHeight, SDL_Rect pDisplayArea);

    // Draw the map with the given map coordinates in the upper-left corner.
    void draw(Sint16 mpx, Sint16 mpy);

    // Return the hex currently drawn at the given screen pixel.
    Point getHexAt(Sint16 spx, Sint16 spy) const;
    Point getHexAt(const Point &sp) const;

    // Return the screen coordinates of the given hex.
    Point sPixelFromHex(Sint16 hx, Sint16 hy) const;
    Point sPixelFromHex(const Point &hex) const;

private:
    // Use a Voronoi diagram to generate a random set of regions.
    void generateRegions();
    void recalcHexCenters();

    // Construct an adjacency list for each region.
    void buildRegionGraph();

    void assignTerrain();
    void drawTile(Sint16 hx, Sint16 hy);

    // The terrain grid extends from (-1,-1) to (hWidth,hHeight) inclusive on
    // the main grid.  These conversions let us always refer to the map in main
    // grid coordinates.  Return -1 if the result is outside the terrain grid.
    int tIndex(int mIndex) const;
    int tIndex(const Point &mHex) const;
    int tIndex(Sint16 hx, Sint16 hy) const;

    // Convert between screen coordinates and map coordinates.
    Point mPixel(const Point &sp) const;
    Point mPixel(Sint16 spx, Sint16 spy) const;
    Point sPixel(const Point &mp) const;
    Point sPixel(Sint16 mpx, Sint16 mpy) const;

    HexGrid mgrid_;
    int numRegions_;
    std::vector<int> regions_;  // assign each tile to a region [0,numRegions)
    std::vector<Point> centers_;  // center hex of each region
    AdjacencyList regionGraph_;

    // To help make the edges of the map look nice, we extend the grid by one
    // hex in every direction.
    HexGrid tgrid_;
    std::vector<int> terrain_;

    // Visible portion of the map.  Max pixel is defined so that the display
    // area is always filled.
    SDL_Rect pDisplayArea_;
    Sint16 mMaxX_;
    Sint16 mMaxY_;

    // Current upper-left pixel in map coordinates.
    Sint16 px_;
    Sint16 py_;
};

#endif