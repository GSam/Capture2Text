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

#include <QList>
#include <QDebug>
#include "Furigana.h"
#include "PreProcessCommon.h"

//const float Furigana::FURIGANA_MIN_FG_PIX_PER_LINE = 2.0f;
const float Furigana::FURIGANA_MIN_FG_PIX_PER_LINE = 1.0f;
const float Furigana::FURIGANA_MIN_WIDTH = 5.0f;

Furigana::Furigana()
{

}


// Erase the furigana from the provided binary PIX. Works by finding spans of foreground
// text and removing the spans that are too narrow and are likely furigana.
// Use this version for vertical text.
bool Furigana::eraseFuriganaVertical(PIX *pixs, float scaleFactor, int *numTextLines)
{
    int minFgPixPerLine = (int)(FURIGANA_MIN_FG_PIX_PER_LINE * scaleFactor);
    int minSpanWidth = (int)(FURIGANA_MIN_WIDTH * scaleFactor);
    unsigned int x = 0;
    int numFgPixelsOnLine = 0;
    int goodLine = LEPT_FALSE;
    int numGoodLinesInCurSpan = 0;
    int totalGoodLines = 0;
    unsigned int pixelValue = 0;
    FuriganaSpan span(NO_VALUE, NO_VALUE);
    QList<FuriganaSpan> spanList;

    // Get list of spans that contain fg pixels
    for (x = 0; x < pixs->w; x++)
    {
        numFgPixelsOnLine = 0;
        goodLine = LEPT_FALSE;

        for (int y = 0; y < (int)pixs->h; y++)
        {
            int status = pixGetPixel(pixs, x, y, &pixelValue);

            if (status != LEPT_OK)
            {
                return false;
            }

            // If this is a foreground pixel
            if (pixelValue == 1)
            {
                numFgPixelsOnLine++;

                // If this line has already meet the minimum number of fg pixels, stop scanning it
                if (numFgPixelsOnLine >= minFgPixPerLine)
                {
                    goodLine = LEPT_TRUE;
                    break;
                }
            }
        }

        // If last line is good, set it bad in order to close the span
        if (goodLine && (x == pixs->w - 1))
        {
            goodLine = LEPT_FALSE;
            numGoodLinesInCurSpan++;
        }

        // If this line has the minimum number of fg pixels
        if (goodLine)
        {
            // Start a new span
            if (span.start == NO_VALUE)
            {
                span.start = x;
            }

            numGoodLinesInCurSpan++;
        }
        else // Line doesn't have enough fg pixels to consider as part of a span
        {
            // If a span has already been started, then end it
            if (span.start != NO_VALUE)
            {
                // If this span isn't too small (needed so that the average isn't skewed)
                if (numGoodLinesInCurSpan >= minSpanWidth)
                {
                    span.end = x;

                    totalGoodLines += numGoodLinesInCurSpan;

                    // Add span to the list
                    spanList.append(FuriganaSpan(span.start, span.end));
                }
            }

            // Reset span
            span.start = NO_VALUE;
            span.end = NO_VALUE;
            numGoodLinesInCurSpan = 0;
        }
    }

    if (spanList.size() == 0)
    {
        return true;
    }

    // Get mean width of the largest 50% of spans
    QList<double> spanLengths;

    for(auto span : spanList)
    {
      spanLengths.append(span.getLength());
    }

    qSort(spanLengths);

    double meanUpperHalf = 0.0;
    for(int i = spanLengths.size() / 2; i < spanLengths.size(); i++)
    {
        meanUpperHalf += spanLengths[i];
    }

    meanUpperHalf /= (spanLengths.size() - spanLengths.size() / 2);

    x = 0;
    int numMinorSpans = 0;

    // Erase areas of the PIX where either no span exists or where a span is too narrow
    for (int spanIdx = 0; spanIdx < spanList.size(); spanIdx++)
    {
        span = spanList.at(spanIdx);

        // If span is at least X% of average width, erase area between the previous span and this span
        if (span.getLength() >= (int)(meanUpperHalf * 0.6))
        {
            bool status = eraseAreaLeftToRight(pixs, x, span.start - x);

            if (!status)
            {
                return false;
            }

            x = span.end + 1;
        }
        else
        {
            numMinorSpans++;
        }
    }

    *numTextLines = qMax(spanList.size() - numMinorSpans, 1);

    // Clear area between the end of the right-most span and the right edge of the PIX
    if ((x != 0) && (x < (pixs->w - 1)))
    {
        bool status = eraseAreaLeftToRight(pixs, x, pixs->w - x);

        if (!status)
        {
            return false;
        }
    }

    return true;
}

// Erase the furigana from the provided binary PIX. Works by finding spans of foreground
// text and removing the spans that are too narrow and are likely furigana.
// Use this version for horizontal text.
bool Furigana::eraseFuriganaHorizontal(PIX *pixs, float scaleFactor, int *numTextLines)
{
    int minFgPixPerLine = (int)(FURIGANA_MIN_FG_PIX_PER_LINE * scaleFactor);
    int minSpanWidth = (int)(FURIGANA_MIN_WIDTH * scaleFactor);
    unsigned int y = 0;
    int numFgPixelsOnLine = 0;
    int goodLine = LEPT_FALSE;
    int numGoodLinesInCurSpan = 0;
    int totalGoodLines = 0;
    unsigned int pixelValue = 0;
    FuriganaSpan span(NO_VALUE, NO_VALUE);
    QList<FuriganaSpan> spanList;

    // Get list of spans that contain fg pixels
    for (y = 0; y < pixs->h; y++)
    {
        numFgPixelsOnLine = 0;
        goodLine = LEPT_FALSE;

        for (int x = 0; x < (int)pixs->w; x++)
        {
            int status = pixGetPixel(pixs, x, y, &pixelValue);

            if (status != LEPT_OK)
            {
                return false;
            }

            // If this is a foreground pixel
            if (pixelValue == 1)
            {
                numFgPixelsOnLine++;

                // If this line has already meet the minimum number of fg pixels, stop scanning it
                if (numFgPixelsOnLine >= minFgPixPerLine)
                {
                    goodLine = LEPT_TRUE;
                    break;
                }
            }
        }

        // If last line is good, set it bad in order to close the span
        if (goodLine && (y == pixs->h - 1))
        {
            goodLine = LEPT_FALSE;
            numGoodLinesInCurSpan++;
        }

        // If this line has the minimum number of fg pixels
        if (goodLine)
        {
            // Start a new span
            if (span.start == NO_VALUE)
            {
                span.start = y;
            }

            numGoodLinesInCurSpan++;
        }
        else // Line doesn't have enough fg pixels to consider as part of a span
        {
            // If a span has already been started, then end it
            if (span.start != NO_VALUE)
            {
                // If this span isn't too small (needed so that the average isn't skewed)
                if (numGoodLinesInCurSpan >= minSpanWidth)
                {
                    span.end = y;

                    totalGoodLines += numGoodLinesInCurSpan;

                    // Add span to the list
                    spanList.append(FuriganaSpan(span.start, span.end));
                }
            }

            // Reset span
            span.start = NO_VALUE;
            span.end = NO_VALUE;
            numGoodLinesInCurSpan = 0;
        }
    }

    if (spanList.size() == 0)
    {
        return true;
    }

    // Get mean width of the largest 50% of spans
    QList<double> spanLengths;

    for(auto span : spanList)
    {
      spanLengths.append(span.getLength());
    }

    qSort(spanLengths);

    double meanUpperHalf = 0.0;
    for(int i = spanLengths.size() / 2; i < spanLengths.size(); i++)
    {
        meanUpperHalf += spanLengths[i];
    }

    meanUpperHalf /= (spanLengths.size() - spanLengths.size() / 2);

    y = 0;
    int numMinorSpans = 0;

    // Erase areas of the PIX where either no span exists or where a span is too narrow
    for (int spanIdx = 0; spanIdx < spanList.size(); spanIdx++)
    {
        span = spanList.at(spanIdx);

        // If span is at least X% of average width, erase area between the previous span and this span
        if (span.getLength() >= (int)(meanUpperHalf * 0.6))
        {
            bool status = eraseAreaTopToBottom(pixs, y, span.start - y);

            if (!status)
            {
                return false;
            }

            y = span.end + 1;
        }
        else
        {
            numMinorSpans++;
        }
    }

    *numTextLines = qMax(spanList.size() - numMinorSpans, 1);

    // Clear area between the end of the right-most span and the right edge of the PIX
    if ((y != 0) && (y < (pixs->h - 1)))
    {
        bool status = eraseAreaTopToBottom(pixs, y, pixs->h - y);

        if (!status)
        {
            return false;
        }
    }

    return true;
}

// Clear/erase a left-to-right section of the provided binary PIX.
bool Furigana::eraseAreaLeftToRight(PIX *pixs, int x, int width)
{
    int status = LEPT_ERROR;
    BOX box;

    box.x = x;
    box.y = 0;
    box.w = width;
    box.h = pixs->h;

    status = pixClearInRect(pixs, &box);

    return (status == LEPT_OK);

}

// Clear/erase a top-to-bottom section of the provided binary PIX.
bool Furigana::eraseAreaTopToBottom(PIX *pixs, int y, int height)
{
    int status = LEPT_ERROR;
    BOX box;

    box.x = 0;
    box.y = y;
    box.w = pixs->w;
    box.h = height;

    status = pixClearInRect(pixs, &box);

    return (status == LEPT_OK) ;

}
