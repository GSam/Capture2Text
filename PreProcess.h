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

#ifndef PRE_PROCESS_H
#define PRE_PROCESS_H

#include <QImage>
#include "allheaders.h"
#include "PreProcessCommon.h"

class PreProcess
{
public:
    PreProcess();

    bool getVerticalText() const;
    void setVerticalOrientation(bool value);

    bool getRemoveFurigana() const;
    void setRemoveFurigana(bool value);

    int getJapNumTextLines() const;

    QRect getBoundingRect() const;

    float getScaleFactor() const;
    void setScaleFactor(float value);

    PIX *convertImageToPix(QString imageFile);
    PIX *convertImageToPix(QImage &image);

    PIX *processImage(PIX *pixs, bool performDeskew=false, bool trim=false);
    PIX *extractTextBlock(PIX *pixs, int pt_x, int pt_y, int lookahead, int lookbehind, int searchRadius);
    PIX *extractBubbleText(PIX *pixs, int pt_x, int pt_y);

private:
    PIX *makeGray(PIX *pixs);
    PIX *scale(PIX *pixs);
    PIX *unsharpMask(PIX *pixs);
    PIX *binarize(PIX *pixs);
    PIX *scaleUnsharpBinarize(PIX *pixs);
    PIX *deskew(PIX *pixs);
    PIX *addBorder(PIX *pixs);
    PIX *removeNoise(PIX *pixs);
    PIX *eraseFurigana(PIX *pixs);
    void setDPI(PIX *pixs);

    void debugMsg(QString str, bool error=true);
    void debugImg(QString filename, PIX *pixs);

#ifdef QT_DEBUG
    const bool debug = false;
#endif

    // From 0.0 to 1.0, with 0 being all white and 1 being all black
    const float darkBgThreshold = 0.5f;

    // Amount to scale input image to meet OCR engine minimum DPI requirements
    float scaleFactor = 3.5f;

    // Is the text vertical (affects furigana removal)
    bool verticalText;

    // The last bounding rect extracted.
    // Set in extractTextBlock() and extractBubbleText().
    // Can be used for display purposes.
    BOX boundingRect;

    bool removeFurigana;

    // Number of lines detected when last furigana removal was performed
    int japNumTextLines = 0;

    int debugImgCount = 0;
};

#endif // PRE_PROCESS_H
