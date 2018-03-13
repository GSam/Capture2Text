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

#ifndef BOUNDING_TEXT_RECT_H
#define BOUNDING_TEXT_RECT_H

#include <QList>
#include "allheaders.h"
#include "PreProcessCommon.h"


class BoundingTextRect
{
public:
    enum class D8
    {
      Top,
      TopRight,
      Right,
      BottomRight,
      Bottom,
      BottomLeft,
      Left,
      TopLeft
    };

    struct DirDist
    {
      D8 dir;
      int dist;

      DirDist(D8 _dir, int _dist)
          : dir(_dir), dist(_dist) { }
    };

    static BOX getBoundingRect(PIX *pixs, int startX, int startY, bool vertical,
                               int lookahead, int lookbehind, int maxSearchDist);

    static Point findNearestBlackPixel(PIX *pixs, int startX, int startY, int max_dist);
    static bool isBlack(PIX *pixs, int x, int y);
    static bool inRangeX(PIX *pixs, int x);
    static bool inRangeY(PIX *pixs, int y);
    static bool lineContainBlackHoriz(PIX *pixs, int startX, int startY, int width);
    static bool lineContainBlackVert(PIX *pixs, int startX, int startY, int height);

private:
    BoundingTextRect();

    static bool tryExpandRect(PIX *pixs, BOX *rect, D8 dir, int dist);
    static void expandRect(PIX *pixs, QList<DirDist> &dirDistList, BOX *rect, bool keep_going);
};

#endif // BOUNDING_TEXT_RECT_H
