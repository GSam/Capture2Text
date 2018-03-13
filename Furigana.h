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

#ifndef FURIGANA_H
#define FURIGANA_H

#include "allheaders.h"

class Furigana
{
public:

    static bool eraseFuriganaVertical(PIX *pixs, float scaleFactor, int *numTextLines);
    static bool eraseFuriganaHorizontal(PIX *pixs, float scaleFactor, int *numTextLines);

private:
    // Span of lines that contain foreground text. Used during furigana removal.
    struct FuriganaSpan
    {
        int start;
        int end;

        int getLength() { return abs(end - start) + 1; }

        FuriganaSpan(int s, int e)
        {
          start = s;
          end = e;
        }
    };

    Furigana();

    // Minimum number of foreground pixels that a line must contain for it to be part of a span.
    // Needed because sometimes furigana does not have a perfect gap between the text and itself.
    static const float FURIGANA_MIN_FG_PIX_PER_LINE;

    // Minimum width of a span (in pixels) for it to be included in the span list.
    static const float FURIGANA_MIN_WIDTH;

    static bool eraseAreaLeftToRight(PIX *pixs, int x, int width);
    static bool eraseAreaTopToBottom(PIX *pixs, int y, int height);

    static bool averageLargestSpans(PIX *pixs, float scaleFactor);


};

#endif // FURIGANA_H
