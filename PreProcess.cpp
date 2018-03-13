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

#include <QBuffer>
#include <QDebug>
#include <QtGlobal>
#include "PreProcess.h"
#include "BoundingTextRect.h"
#include "Furigana.h"

PreProcess::PreProcess()
    : verticalText(false),
      removeFurigana(false)
{

}

QRect PreProcess::getBoundingRect() const
{
    return QRect(boundingRect.x / scaleFactor,
                 boundingRect.y / scaleFactor,
                 boundingRect.w / scaleFactor,
                 boundingRect.h / scaleFactor);
}

bool PreProcess::getRemoveFurigana() const
{
    return removeFurigana;
}

void PreProcess::setRemoveFurigana(bool value)
{
    removeFurigana = value;
}

bool PreProcess::getVerticalText() const
{
    return verticalText;
}

void PreProcess::setVerticalOrientation(bool value)
{
    verticalText = value;
}

// Set DPI so that Tesseract 4.0 doesn't issue this warning:
// "Warning. Invalid resolution 0 dpi. Using 70 instead."
void PreProcess::setDPI(PIX *pixs)
{
    pixs->xres = 300;
    pixs->yres = 300;
}

int PreProcess::getJapNumTextLines() const
{
    return japNumTextLines;
}

float PreProcess::getScaleFactor() const
{
    return scaleFactor;
}

void PreProcess::setScaleFactor(float value)
{
    scaleFactor = qMin(qMax(value, 0.71f), 5.0f);
}

void PreProcess::debugMsg(QString str, bool error)
{
#ifdef QT_DEBUG
    if(debug || error)
    {
        qDebug() << str;
    }
#else
    Q_UNUSED(str);
    Q_UNUSED(error);
#endif

}

void PreProcess::debugImg(QString filename, PIX *pixs)
{
#ifdef QT_DEBUG
    if(debug)
    {
        debugImgCount++;
        QString file = QString("G:\\Temp\\Temp\\c2t_debug\\%1_%2")
                .arg(debugImgCount, 2, 10, QChar('0')).arg(filename);
        QByteArray ba = file.toLocal8Bit();
        pixWriteImpliedFormat(ba.constData(), pixs, 0, 0);
    }
#else
    Q_UNUSED(filename);
    Q_UNUSED(pixs);
#endif
}


// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::convertImageToPix(QImage &image)
{
    // On Windows, pixReadMem() only works with the TIF format,
    // so save the image to memory in TIF format first.
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "TIF");

    PIX *pixs = pixReadMem((const unsigned char*)ba.data(), ba.size());

    if(pixs == nullptr)
    {
        debugMsg("convertImageToPix: failed!");
        return nullptr;
    }

    return pixs;
}

// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::convertImageToPix(QString imageFile)
{
    QByteArray ba = imageFile.toLocal8Bit();
    PIX *pixs = pixRead(ba.constData());

    if(pixs == nullptr)
    {
        debugMsg("convertImageToPix: failed!");
        return nullptr;
    }

    return pixs;
}

// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::makeGray(PIX *pixs)
{
    PIX *pixGray = pixConvertRGBToGray(pixs, 0.0f, 0.0f, 0.0f);

    if(pixGray == nullptr)
    {
        debugMsg("makeGray: failed!");
        return nullptr;
    }

    return pixGray;
}

// pixs must be 8 bpp.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::scale(PIX *pixs)
{
    PIX *scaled_pixs = pixScaleGrayLI(pixs, scaleFactor, scaleFactor);

    if (scaled_pixs == nullptr)
    {
        debugMsg("scale: failed!");
        return nullptr;
    }

    debugImg("scale.png", scaled_pixs);

    return scaled_pixs;
}

// pixs must be 8 bpp.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::unsharpMask(PIX *pixs)
{
    const int usmHalfwidth = 5;
    const float usmFract = 2.5f;

    PIX *unsharp_pixs = pixUnsharpMaskingGray(pixs, usmHalfwidth, usmFract);

    if (unsharp_pixs == nullptr)
    {
        debugMsg("unsharpMask: failed!");
        return nullptr;
    }

    debugImg("unsharpMask.png", unsharp_pixs);

    return unsharp_pixs;
}

// pixs must be 8 bpp.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::binarize(PIX *pixs)
{
    const int otsuSX = 2000;
    const int otsuSY = 2000;
    const int otsuSmoothX = 0;
    const int otsuSmoothY = 0;
    const float otsuScorefract = 0.0f;

    PIX *binarize_pixs = nullptr;

#if 1
    int status = pixOtsuAdaptiveThreshold(pixs, otsuSX, otsuSY, otsuSmoothX, otsuSmoothY, otsuScorefract, nullptr, &binarize_pixs);

    if (status != LEPT_OK)
    {
        debugMsg("binarize: failed!");
        return nullptr;
    }
#else
    binarize_pixs = pixOtsuThreshOnBackgroundNorm(pixs, nullptr, otsuSX, otsuSY, 100, 50, 255, otsuSmoothX, otsuSmoothY, otsuScorefract, nullptr);

    if (binarize_pixs == nullptr)
    {
        debugMsg("binarize: failed!");
        return nullptr;
    }
#endif

    debugImg("binarize.png", binarize_pixs);

    return binarize_pixs;
}

// pixs must be 8 bpp.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::scaleUnsharpBinarize(PIX *pixs)
{
    PIX *scaled_pixs = nullptr;
    PIX *unsharp_pixs = nullptr;
    PIX *binarize_pixs = nullptr;

    scaled_pixs = scale(pixs);

    if (scaled_pixs == nullptr)
    {
        return nullptr;
    }

    unsharp_pixs = unsharpMask(scaled_pixs);
    pixDestroy(&scaled_pixs);

    if (unsharp_pixs == nullptr)
    {
        return nullptr;
    }

    binarize_pixs = binarize(unsharp_pixs);
    pixDestroy(&unsharp_pixs);

    if (binarize_pixs == nullptr)
    {
        return nullptr;
    }

    return binarize_pixs;
}

// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::deskew(PIX *pixs)
{
#if 0
    l_float32 angle;
    l_float32 conf;
    PIX *pixDeskew = pixFindSkewAndDeskew(pixs, 0, &angle, &conf);
    qDebug() << "Angle: " << angle << " Conf: " << conf;
#else
    PIX *pixDeskew = pixFindSkewAndDeskew(pixs, 0, nullptr, nullptr);
#endif

    if (pixDeskew == nullptr)
    {
        debugMsg("deskew: failed!");
        return nullptr;
    }

    debugImg("deskew.png", pixDeskew);

    return pixDeskew;
}

// pixs must be 1 bpp.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::addBorder(PIX *pixs)
{
    const int borderWidth = 10;

    PIX *border_pixs  = pixAddBlackOrWhiteBorder(pixs, borderWidth, borderWidth, borderWidth, borderWidth, L_GET_WHITE_VAL);

    if (border_pixs == nullptr)
    {
        debugMsg("addBorder: failed!");
        return nullptr;
    }

    debugImg("addBorder.png", border_pixs);

    return border_pixs;
}

// pixs must be 1 bpp.
// Remove very small blobs.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::removeNoise(PIX *pixs)
{
    //int minBlobSize = (int)(1.86 * scaleFactor);
    int minBlobSize = 3;

    // Remove noise if both dimensions are less than minBlobSize (yes, L_SELECT_IF_EITHER is correct here).
    PIX *denoisePixs1 = pixSelectBySize(pixs, minBlobSize, minBlobSize, 8,
                                        L_SELECT_IF_EITHER, L_SELECT_IF_GT, nullptr);

    if (denoisePixs1 == nullptr)
    {
        debugMsg("removeNoise 1: failed!");
        return nullptr;
    }

    debugImg("denoisePixs1.png", denoisePixs1);

    return denoisePixs1;

#if 0
    minBlobSize = (int)(1.2 * scaleFactor);

    // Remove noise if either dimension is less than minBlobSize (yes, L_SELECT_IF_BOTH is correct here).
    PIX *denoisePixs2 = pixSelectBySize(denoisePixs1, minBlobSize, minBlobSize, 8,
                                        L_SELECT_IF_BOTH, L_SELECT_IF_GT, nullptr);
    pixDestroy(&denoisePixs1);

    if (denoisePixs2 == nullptr)
    {
        debugMsg("removeNoise 2: failed!");
        return nullptr;
    }

    debugImg("denoisePixs2.png", denoisePixs2);

    return denoisePixs2;
#endif
}

// pixs must be 1 bpp.
PIX *PreProcess::eraseFurigana(PIX *pixs)
{
    PIX *denoisePixs = nullptr;
    bool status = true;

    if(removeFurigana)
    {
        if (verticalText)
        {
            status = Furigana::eraseFuriganaVertical(pixs, scaleFactor, &japNumTextLines);
        }
        else
        {
            status = Furigana::eraseFuriganaHorizontal(pixs, scaleFactor, &japNumTextLines);
        }

        if(status)
        {
            debugImg("eraseFurigana.png", pixs);
            denoisePixs = removeNoise(pixs);
        }
        else
        {
            debugMsg("eraseFurigana: failed!");
            return nullptr;
        }
    }
    else
    {
        denoisePixs = pixClone(pixs);
    }

    return denoisePixs;
}

// Standard pre-process for OCR.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::processImage(PIX *pixs, bool performDeskew, bool trim)
{
    debugImgCount = 0;

    // Convert to grayscale
    PIX *pixGray = makeGray(pixs);

    if(pixGray == nullptr)
    {
        return nullptr;
    }

    // Binarize for negate determination
    PIX *binarizeForNegPixs = binarize(pixGray);

    if (binarizeForNegPixs == nullptr)
    {
        return nullptr;
    }

    float pixelAvg = 0.0f;

    // Get the average intensity of the border pixels,
    // with average of 0.0 being completely white and 1.0 being completely black.
    // Top, bottom, left, right.
    pixelAvg  = pixAverageOnLine(binarizeForNegPixs, 0, 0, binarizeForNegPixs->w - 1, 0, 1);
    pixelAvg += pixAverageOnLine(binarizeForNegPixs, 0, binarizeForNegPixs->h - 1, binarizeForNegPixs->w - 1, binarizeForNegPixs->h - 1, 1);
    pixelAvg += pixAverageOnLine(binarizeForNegPixs, 0, 0, 0, binarizeForNegPixs->h - 1, 1);
    pixelAvg += pixAverageOnLine(binarizeForNegPixs, binarizeForNegPixs->w - 1, 0, binarizeForNegPixs->w - 1, binarizeForNegPixs->h - 1, 1);
    pixelAvg /= 4.0f;

    pixDestroy(&binarizeForNegPixs);

    // If background is dark
    if (pixelAvg > darkBgThreshold)
    {
        // Negate image (yes, input and output can be the same PIX)
        pixInvert(pixGray, pixGray);

        if (pixGray == nullptr)
        {
            return nullptr;
        }
    }

    // Scale, Unsharp Mask, Binarize
    PIX *pixBinarize = scaleUnsharpBinarize(pixGray);
    pixDestroy(&pixGray);

    if (pixBinarize == nullptr)
    {
        return nullptr;
    }

    // Deskew
    if(performDeskew)
    {
        PIX *pixDeskew = deskew(pixBinarize);

        // Deskew isn't critical, ignore on failure
        if (pixDeskew != nullptr)
        {
          pixDestroy(&pixBinarize);
          pixBinarize = pixDeskew;
        }
    }

    // Erase furigana
    PIX *furiganaPixs = eraseFurigana(pixBinarize);
    pixDestroy(&pixBinarize);

    if (furiganaPixs == nullptr)
    {
        return nullptr;
    }

    if(trim)
    {
        PIX *foregroundPixs = nullptr;

        // Remove border
        int status = pixClipToForeground(furiganaPixs, &foregroundPixs, nullptr);
        pixDestroy(&furiganaPixs);

        if (status != LEPT_OK)
        {
            debugMsg("pixClipToForeground failed!");
            return nullptr;
        }

        // Add border
        PIX *borderPixs = addBorder(foregroundPixs);
        pixDestroy(&foregroundPixs);

        if (borderPixs == nullptr)
        {
            return nullptr;
        }

        setDPI(borderPixs);
        return borderPixs;
    }

    setDPI(furiganaPixs);
    return furiganaPixs;
}


// Extract the text block closest to the provided point.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::extractTextBlock(PIX *pixs, int pt_x, int pt_y, int lookahead, int lookbehind, int searchRadius)
{
    debugImgCount = 0;
    int status = LEPT_ERROR;

    // Convert to grayscale
    PIX *pixGray = makeGray(pixs);

    if(pixGray == nullptr)
    {
        return nullptr;
    }

    // Binarize for negate determination
    PIX *binarizeForNegPixs = binarize(pixGray);

    debugImg("binarizeForNegPixs.png", binarizeForNegPixs);

    if (binarizeForNegPixs == nullptr)
    {
        return nullptr;
    }

    // Get the average intensity in the area around the start coordinates
    BOX negRect;
    const int idealNegRectLength = 40;
    negRect.x = qMax(0, pt_x - idealNegRectLength / 2);
    negRect.y = qMax(0, pt_y - idealNegRectLength / 2);
    negRect.w = qMin((int)binarizeForNegPixs->w - negRect.x, idealNegRectLength);
    negRect.h = qMin((int)binarizeForNegPixs->h - negRect.y, idealNegRectLength);

#if 0
    PIX *negRectPixs = pixClipRectangle(binarizeForNegPixs, &negRect, nullptr);
    debugImg("negRectPixs.png", negRectPixs);
    qDebug() << QString("%1, %2, %3, %4")
                .arg(negRect.x).arg(negRect.y).arg(negRect.w).arg(negRect.h);
#endif

    float pixelAvg = 0.0f;
    status = pixAverageInRect(binarizeForNegPixs, &negRect, &pixelAvg);
    pixDestroy(&binarizeForNegPixs);

    // qDebug() << "Pixel Avg: " << pixelAvg;

    if (status != LEPT_OK)
    {
        // Assume white background
        pixelAvg = 0.0;
    }

    // If background is dark
    if (pixelAvg > darkBgThreshold)
    {
        // Negate image (yes, input and output can be the same PIX)
        pixInvert(pixGray, pixGray);

        if (pixGray == nullptr)
        {
            return nullptr;
        }
    }

    // Scale, Unsharp Mark, Binarize
    PIX *binarizePixs = scaleUnsharpBinarize(pixGray);
    pixDestroy(&pixGray);

    if (binarizePixs == nullptr)
    {
        return nullptr;
    }

    // Remove black pixels connected to the border.
    // This eliminates annoying things like text bubbles in manga.
    PIX *connCompsPixs = pixRemoveBorderConnComps(binarizePixs, 8);

    if (connCompsPixs == nullptr)
    {
        debugMsg("pixRemoveBorderConnComps failed!");
        return nullptr;
    }

    debugImg("connCompsPixs.png", connCompsPixs);

    // Remove noise
    PIX *denoisePixs = removeNoise(connCompsPixs);
    pixDestroy(&connCompsPixs);

    if (denoisePixs == nullptr)
    {
        return nullptr;
    }

    // Get rectangle surrounding the text to extract
    boundingRect = BoundingTextRect::getBoundingRect(denoisePixs,
                                                     pt_x * scaleFactor,
                                                     pt_y * scaleFactor,
                                                     verticalText,
                                                     lookahead * scaleFactor,
                                                     lookbehind * scaleFactor,
                                                     searchRadius * scaleFactor);
    pixDestroy(&denoisePixs);

    if(boundingRect.w < 3 && boundingRect.h < 3)
    {
        pixDestroy(&binarizePixs);
        debugMsg("BoundingRect too small!");
        return nullptr;
    }

    PIX *croppedPixs = pixClipRectangle(binarizePixs, &boundingRect, nullptr);
    pixDestroy(&binarizePixs);

    if (croppedPixs == nullptr)
    {
        return nullptr;
    }

    debugImg("croppedPixs.png", croppedPixs);

    // Erase furigana
    PIX *furiganaPixs = eraseFurigana(croppedPixs);
    pixDestroy(&croppedPixs);

    if (furiganaPixs == nullptr)
    {
        return nullptr;
    }

    PIX *foregroundPixs = nullptr;
    BOX *foregroundBox = nullptr;

    // Remove border
    status = pixClipToForeground(furiganaPixs, &foregroundPixs, &foregroundBox);
    pixDestroy(&furiganaPixs);

    if (status != LEPT_OK)
    {
        debugMsg("pixClipToForeground failed!");
        return nullptr;
    }

    // Adjust bounding rect to account for removed border
    boundingRect.x = boundingRect.x + foregroundBox->x;
    boundingRect.y = boundingRect.y + foregroundBox->y;
    boundingRect.w = foregroundBox->w;
    boundingRect.h = foregroundBox->h;

    // Add border
    PIX *borderPixs = addBorder(foregroundPixs);
    pixDestroy(&foregroundPixs);

    if (borderPixs == nullptr)
    {
        return nullptr;
    }

    setDPI(borderPixs);
    return borderPixs;
}


// Extract all text within an enclosed area such as a comic book speech/thought bubble.
// Be sure to call pixDestroy() on the returned PIX pointer to avoid memory leak.
PIX *PreProcess::extractBubbleText(PIX *pixs, int pt_x, int pt_y)
{
    debugImgCount = 0;
    l_int32 status = LEPT_ERROR;

    pt_x = (int)(pt_x * scaleFactor);
    pt_y = (int)(pt_y * scaleFactor);

    // Convert to grayscale
    PIX *grayPixs = makeGray(pixs);

    if (grayPixs == nullptr)
    {
        return nullptr;
    }

    // Scale, Unsharp Mark, Binarize
    PIX *binarizePixs = scaleUnsharpBinarize(grayPixs);
    pixDestroy(&grayPixs);

    if (binarizePixs == nullptr)
    {
        return nullptr;
    }

    // Get color of the start pixel
    l_uint32 startPtIsBlack = LEPT_FALSE;
    status = pixGetPixel(binarizePixs, pt_x, pt_y, &startPtIsBlack);

    if (status != LEPT_OK)
    {
        debugMsg("pixGetPixel failed!");
        return nullptr;
    }

    // Invert if bubble is black background with white text
    if (startPtIsBlack)
    {
        pixInvert(binarizePixs, binarizePixs);

        if (binarizePixs == nullptr)
        {
            debugMsg("pixInvert failed!");
            return nullptr;
        }
    }

    // Create the seed start point
    PIX *seedStartPixs = pixCreateTemplate(binarizePixs);

    if (seedStartPixs == nullptr)
    {
        debugMsg("pixCreateTemplate failed!");
        return nullptr;
    }

    status = pixSetPixel(seedStartPixs, pt_x, pt_y, 1);

    if (status != LEPT_OK)
    {
        debugMsg("pixSetPixel failed!");
        return nullptr;
    }

    // Dilate to thicken lines and connect small gaps in the bubble
    int thickenAmount = (int)(2 * scaleFactor);
    PIX *thickenLinesPixs = pixDilateBrick(nullptr, binarizePixs, thickenAmount, thickenAmount);

    if (thickenLinesPixs == nullptr)
    {
        debugMsg("pixMorphSequence failed!");
        return nullptr;
    }

    debugImg("thickenLinesPixs.png", thickenLinesPixs);

    // Invert for seed fill
    PIX *binarizeNegPixs = pixInvert(nullptr, thickenLinesPixs);
    pixDestroy(&thickenLinesPixs);

    if (binarizeNegPixs == nullptr)
    {
        debugMsg("pixInvert failed!");
        return nullptr;
    }

    // Seed fill
    PIX *seedFillPixs = pixSeedfillBinary(nullptr, seedStartPixs, binarizeNegPixs, 8);
    pixDestroy(&seedStartPixs);
    pixDestroy(&binarizeNegPixs);

    if (seedFillPixs == nullptr)
    {
        debugMsg("pixSeedfillBinary failed!");
        return nullptr;
    }

    debugImg("seedFillPixs.png", seedFillPixs);

    // Negate seed fill
    pixInvert(seedFillPixs, seedFillPixs);

    if (seedFillPixs == nullptr)
    {
        debugMsg("pixInvert 2 failed!");
        return nullptr;
    }

    debugImg("seedFillPixs_Neg.png", seedFillPixs);

    // Remove foreground pixels touching the border
    PIX *noBorderPixs = pixRemoveBorderConnComps(seedFillPixs, 8);
    pixDestroy(&seedFillPixs);

    if (noBorderPixs == nullptr)
    {
        debugMsg("pixRemoveBorderConnComps failed!");
        return nullptr;
    }

    debugImg("noBorderPixs.png", noBorderPixs);

    // AND with original binary image to remove everything except for the text
    PIX *andPixs = pixAnd(nullptr, noBorderPixs, binarizePixs);
    pixDestroy(&binarizePixs);
    pixDestroy(&noBorderPixs);

    if (andPixs == nullptr)
    {
        debugMsg("pixAnd failed!");
        return nullptr;
    }

    debugImg("andPixs.png", andPixs);

    PIX *denoisePixs = removeNoise(andPixs);
    pixDestroy(&andPixs);

    if (denoisePixs == nullptr)
    {
        return nullptr;
    }

    // Erase furigana
    PIX *furiganaPixs = eraseFurigana(denoisePixs);
    pixDestroy(&denoisePixs);

    if (furiganaPixs == nullptr)
    {
        return nullptr;
    }

    // Clip to text
    PIX *clippedPixs = nullptr;
    BOX *foregroundBox = nullptr;
    status = pixClipToForeground(furiganaPixs, &clippedPixs, &foregroundBox);
    pixDestroy(&furiganaPixs);

    if (status != LEPT_OK)
    {
        debugMsg("pixClipToForeground failed!");
        return nullptr;
    }

    debugImg("clippedPixs.png", clippedPixs);

    // Add border
    PIX *borderPixs = addBorder(clippedPixs);
    pixDestroy(&clippedPixs);

    if (borderPixs == nullptr)
    {
        return nullptr;
    }

    boundingRect.x = foregroundBox->x;
    boundingRect.y = foregroundBox->y;
    boundingRect.w = foregroundBox->w;
    boundingRect.h = foregroundBox->h;

    setDPI(borderPixs);
    return borderPixs;
}
