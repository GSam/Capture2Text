/*
Copyright (C) 2010-2017 Christopher Brochtrup

This file is part of Capture2Text.

Capture2Text is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Capture2Text is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Capture2Text.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BoundingTextRect.h"

BoundingTextRect::BoundingTextRect()
{

}

Point BoundingTextRect::findNearestBlackPixel(PIX *pixs, int startX, int startY, int maxDist)
{
    Point pt = { startX, startY };

    for (int dist = 1; dist < maxDist; dist++)
    {
        // Check right one pixel
        pt.x++;

        if (isBlack(pixs, pt.x, pt.y))
        {
            return pt;
        }

        // Check down
        for (int i = 0; i < dist * 2 - 1; i++)
        {
            pt.y++;

            if (isBlack(pixs, pt.x, pt.y))
            {
                return pt;
            }
        }

        // Check left
        for (int i = 0; i < dist * 2; i++)
        {
            pt.x--;

            if (isBlack(pixs, pt.x, pt.y))
            {
                return pt;
            }
        }

        // Check up
        for (int i = 0; i < dist * 2; i++)
        {
            pt.y--;

            if (isBlack(pixs, pt.x, pt.y))
            {
                return pt;
            }
        }

        // Check right
        for (int i = 0; i < dist * 2; i++)
        {
            pt.x++;

            if (isBlack(pixs, pt.x, pt.y))
            {
                return pt;
            }
        }
    }

    pt.x = -1;
    pt.y = -1;

    return pt;
}

bool BoundingTextRect::lineContainBlackHoriz(PIX *pixs, int startX, int startY, int width)
{
    for (int x = startX; x <= startX + width && inRangeX(pixs, x); x++)
    {
        if (isBlack(pixs, x, startY))
        {
            return true;
        }
    }

    return false;
}

bool BoundingTextRect::lineContainBlackVert(PIX *pixs, int startX, int startY, int height)
{
    for (int y = startY; y <= startY + height && inRangeY(pixs, y); y++)
    {
        if (isBlack(pixs, startX, y))
        {
            return true;
        }
    }

    return false;
}

bool BoundingTextRect::isBlack(PIX *pixs, int x, int y)
{
    int status = LEPT_ERROR;
    unsigned int pixelValue = 0;

    if(inRangeX(pixs, x) && inRangeY(pixs, y))
    {
        status = pixGetPixel(pixs, x, y, &pixelValue);

        if (status != LEPT_OK)
        {
            return false;
        }

        return (pixelValue == 1);
    }
    else
    {
        return false;
    }
}

bool BoundingTextRect::inRangeX(PIX *pixs, int x)
{
    return ((x >= 0) && ((unsigned int)x < pixs->w));
}

bool BoundingTextRect::inRangeY(PIX *pixs, int y)
{
    return ((y >= 0) && ((unsigned int)y < pixs->h));
}

bool BoundingTextRect::tryExpandRect(PIX *pixs, BOX *rect, D8 dir, int dist)
{
    if (dir == D8::Top)
    {
        if (lineContainBlackHoriz(pixs, rect->x, rect->y - dist, rect->w))
        {
            rect->y -= dist;
            rect->h += dist;
            return true;
        }
    }
    else if (dir == D8::TopRight)
    {
        if (isBlack(pixs, rect->x + rect->w + dist, rect->y - dist))
        {
            rect->y -= dist;
            rect->h += dist;
            rect->w += dist;
            return true;
        }
    }
    else if (dir == D8::Right)
    {
        if (lineContainBlackVert(pixs, rect->x + rect->w + dist, rect->y, rect->h))
        {
            rect->w += dist;
            return true;
        }
    }
    else if (dir == D8::BottomRight)
    {
        if (isBlack(pixs, rect->x + rect->w + dist, rect->y + rect->h + dist))
        {
            rect->h += dist;
            rect->w += dist;
            return true;
        }
    }
    else if (dir == D8::Bottom)
    {
        if (lineContainBlackHoriz(pixs, rect->x, rect->y + rect->h + dist, rect->w))
        {
            rect->h += dist;
            return true;
        }
    }
    else if (dir == D8::BottomLeft)
    {
        if (isBlack(pixs, rect->x - dist, rect->y + rect->h + dist))
        {
            rect->x -= dist;
            rect->h += dist;
            rect->w += dist;
            return true;
        }
    }
    else if (dir == D8::Left)
    {
        if (lineContainBlackVert(pixs, rect->x - dist, rect->y, rect->h))
        {
            rect->x -= dist;
            rect->w += dist;
            return true;
        }
    }
    else if (dir == D8::TopLeft)
    {
        if (isBlack(pixs, rect->x - dist, rect->y + dist))
        {
            rect->x -= dist;
            rect->y -= dist;
            rect->h += dist;
            rect->w += dist;
            return true;
        }
    }

    return false;
}

void BoundingTextRect::expandRect(PIX *pixs, QList<DirDist> &dirDistList, BOX *rect, bool keepGoing)
{
    int i = 0;

    while (true)
    {
        DirDist dirDist = dirDistList[i];

        // Try to expand rect in correct direction
        bool hasBlack = tryExpandRect(pixs, rect, dirDist.dir, dirDist.dist);

        // If could not expand (ie no black pixel found in current direction)
        if (!hasBlack)
        {
            i++;
        }
        // If caller wants to exit upon first successful expansion
        else if (!keepGoing)
        {
            return;
        }

        // If we went through the entire list, return
        if (i >= dirDistList.size())
        {
            return;
        }
    }
}

BOX BoundingTextRect::getBoundingRect(PIX *pixs, int startX, int startY, bool vertical,
                                      int lookahead, int lookbehind, int maxSearchDist)
{
    Point nearestPt = findNearestBlackPixel(pixs, startX, startY, maxSearchDist);
    BOX rect = { nearestPt.x, nearestPt.y, 0, 0 };
    BOX rectLast = rect;

    QList<DirDist> listD4;

    if (vertical)
    {
        for (int i = 1; i < lookbehind + 1; i++)
        {
            listD4.append(DirDist(D8::Top, i));
        }

        listD4.append(DirDist(D8::Right, 1));
        listD4.append(DirDist(D8::Left, 1));

        for (int i = 1; i < lookahead + 1; i++)
        {
            listD4.append(DirDist(D8::Bottom, i));
        }
    }
    else
    {
        listD4.append(DirDist(D8::Top, 1));

        for (int i = 1; i < lookbehind + 1; i++)
        {
            listD4.append(DirDist(D8::Left, i));
        }

        listD4.append(DirDist(D8::Bottom, 1));

        for (int i = 1; i < lookahead + 1; i++)
        {
            listD4.append(DirDist(D8::Right, i));
        }
    }

    QList<DirDist> listCorners;
    listCorners.append(DirDist(D8::TopRight, 1));
    listCorners.append(DirDist(D8::BottomRight, 1));
    listCorners.append(DirDist(D8::BottomLeft, 1));
    listCorners.append(DirDist(D8::TopLeft, 1));

    // Try a few iterations to form the best bounding rect
    for (int i = 0; i < 10; i++)
    {
        expandRect(pixs, listD4, &rect, true);
        expandRect(pixs, listCorners, &rect, false);

        // No change this iteration, no need to continue
        if (rect.x == rectLast.x
                && rect.y == rectLast.y
                && rect.w == rectLast.w
                && rect.h == rectLast.h)
        {
            break;
        }

        rectLast = rect;
    }

    return rect;
}
