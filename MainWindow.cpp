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

#include <QDesktopWidget>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>
#include <QMenu>
#include <QDateTime>

#include "MainWindow.h"
#include "CaptureBox.h"
#include "OcrEngine.h"
#include "PreProcess.h"
#include "PostProcess.h"
#include "UtilsImg.h"
#include "UtilsLang.h"
#include "Settings.h"
#include "KeyboardHook.h"
#include "MouseHook.h"
#include "UtilsCommon.h"
#include "Speech.h"


MainWindow::MainWindow(bool portable)
    : pendingPreviewRequest(false)
{
    if(portable)
    {
      QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, UtilsCommon::getAppDir(false));
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);

    createTrayMenu();

    ocrEngine = new OcrEngine();

    if(OcrEngine::isLangInstalled(Settings::getOcrLang()))
    {
        setOcrLang(Settings::getOcrLang());
    }
    else
    {
        QString firstLang = OcrEngine::getFirstInstalledLang();

        if(firstLang == "None")
        {
            QMessageBox::warning(this, "Error", "No OCR languages found!");
            QApplication::exit(1);
        }
        else
        {
            setOcrLang(firstLang);
            Settings::setOcrLang(firstLang);
        }
    }

    captureBox.setBackgroundColor(Settings::getCaptureBoxBackgroundColor());
    captureBox.setBorderColor(Settings::getCaptureBoxBorderColor());
    captureBox.setUseBackgroundColor(true);
    connect(&captureBox, &CaptureBox::captured, this, &MainWindow::captureBoxCaptured);
    connect(&captureBox, &CaptureBox::stoppedMoving, this, &MainWindow::captureBoxStoppedMoving);
    connect(&captureBox, &CaptureBox::cancel, this, &MainWindow::captureBoxCancel);
    connect(&captureBox, &CaptureBox::moved, this, &MainWindow::captureBoxMoved);

    autoCaptureBox.setBorderColor(Settings::getCaptureBoxBorderColor());
    autoCaptureBox.setUseBackgroundColor(false);

    previewBox.setBackgroundColor(Settings::getPreviewBackgroundColor());
    previewBox.setBorderColor(Settings::getPreviewBorderColor());
    previewBox.setTextColor(Settings::getPreviewTextColor());
    previewBox.setTextFont(Settings::getPreviewTextFont());

    infoBox.setBackgroundColor(Settings::getPreviewBackgroundColor());
    infoBox.setBorderColor(Settings::getPreviewBorderColor());
    infoBox.setTextColor(Settings::getPreviewTextColor());
    infoBox.setTextFont(Settings::getPreviewTextFont());

    connect(&watcherPreview, &QFutureWatcher<QString>::finished, this, &MainWindow::ocrPreviewComplete);

    watcherCapture.setPendingResultsLimit(1);
    connect(&watcherCapture, &QFutureWatcher<QString>::finished, this, &MainWindow::ocrCaptureComplete);

    popupDialog.resize(Settings::getOutputPopupWindowSize());
    popupDialog.setTopmost(Settings::getOutputPopupTopmost());

    settingsDialog.init();

    connect(&settingsDialog, &SettingsDialog::accepted, this, &MainWindow::settingsAccepted);

    connect(&translate, &Translate::translationComplete, this, &MainWindow::translationComplete);

    connect(&KeyboardHook::getInstance(), &KeyboardHook::keyPressed, this, &MainWindow::hotkeyPressed);
    registerHotkeys();

    captureTimestamp = QDateTime::currentDateTime();

    if(Settings::getMiscShowWelcome())
    {
        Settings::setMiscShowWelcome(false);
        welcomeDialog.show();
    }

    Settings::setMiscVersion(QCoreApplication::applicationVersion());
}

MainWindow::~MainWindow()
{
    KeyboardHook::getInstance().endThread();
    MouseHook::getInstance().endThread();
    delete actionSaveToClipboard;
    delete actionShowPopupWindow;
    delete actionGroupOcrLang;
    delete actionGroupTextOrientation;
    delete menuTrayIcon;
    delete trayIcon;
    delete ocrEngine;
}

void MainWindow::captureBoxCaptured()
{
    endCaptureBox();
}

void MainWindow::startCaptureBox()
{
    if(Settings::getPreviewEnabled())
    {
        previewBox.move(QPoint(0, 0));
        previewBox.show();
        previewBox.raise();
    }

    captureBox.startCaptureMode();
}

void MainWindow::endCaptureBox()
{
    captureTimestamp = QDateTime::currentDateTime();
    previewBox.hideAndReset();
    captureBox.endCaptureMode();

    QRect captureRect = captureBox.getCaptureRect();
    if(captureRect.width() <= minOcrWidth || captureRect.height() <= minOcrHeight)
    {
        return;
    }

    QFuture<QString> futureCapture = QtConcurrent::run(this, &MainWindow::ocrCaptureBoxArea);
    watcherCapture.setFuture(futureCapture);
}

void MainWindow::ocrCaptureComplete()
{
    QString ocrText = watcherCapture.result();
    ocrText = postProcess(ocrText);
    outputOcrText(ocrText);
}

bool MainWindow::isOrientationVertical()
{
    QString savedTextOrientation = Settings::getOcrTextOrientation();

    if(UtilsLang::languageSupportsVerticalOrientation(Settings::getOcrLang()))
    {
        if(savedTextOrientation == "Auto")
        {
            QRect rect = captureBox.getCaptureRect();
            double aspectRatio = rect.width() / (double)rect.height();

            if(aspectRatio < 2.0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return (savedTextOrientation == "Vertical");
        }
    }
    else
    {
        return false;
    }
}

QString MainWindow::ocrCaptureBoxArea()
{
    bool previewEnabled = Settings::getPreviewEnabled();
    previewEnabled &= !previewBox.isHidden();

    if(previewEnabled)
    {
        QMetaObject::invokeMethod(&captureBox, "turnOffBackground", Qt::BlockingQueuedConnection);
    }

    QRect captureRect = captureBox.getCaptureRect();
    QImage image = UtilsImg::takeScreenshot(captureRect);

    if(previewEnabled)
    {
        QMetaObject::invokeMethod(&captureBox, "turnOnBackground", Qt::BlockingQueuedConnection);

        if(pendingPreviewRequest)
        {
            return previewBox.getText();
        }
    }

    if(image.isNull())
    {
        return "<Error>";
    }

    if(!captureBox.isVisible() && Settings::getDebugSaveCaptureImage())
    {
        image.save(getDebugImagePath("debug_capture.png"));
    }

    preProcess.setVerticalOrientation(isOrientationVertical());
    preProcess.setRemoveFurigana(UtilsLang::languageSupportsFurigana(Settings::getOcrLang()));
    preProcess.setScaleFactor(Settings::getOcrScaleFactor());

    PIX *inPixs = preProcess.convertImageToPix(image);
    PIX *pixs = preProcess.processImage(inPixs, Settings::getOcrDeskew(), Settings::getOcrTrim());
    pixDestroy(&inPixs);

    if(pixs == nullptr)
    {
        return "<Error>";
    }

    if(previewEnabled && pendingPreviewRequest)
    {
        pixDestroy(&pixs);
        return previewBox.getText();
    }

    if(!captureBox.isVisible() && Settings::getDebugSaveEnhancedImage())
    {
        QString savePath = getDebugImagePath("debug_enhanced.png");
        QByteArray byteArray = savePath.toLocal8Bit();
        pixWriteImpliedFormat(byteArray.constData(), pixs, 0, 0);
    }

    ocrEngine->setVerticalOrientation(isOrientationVertical());
    setOcrEngineCommon();

    bool singleLine = false;

    if(UtilsLang::languageSupportsFurigana(Settings::getOcrLang()))
    {
        singleLine = (preProcess.getJapNumTextLines() == 1);
    }

    QString ocrText = ocrEngine->performOcr(pixs, singleLine);
    pixDestroy(&pixs);

    if(previewEnabled && pendingPreviewRequest)
    {
        return previewBox.getText();
    }

    return ocrText;
}

void MainWindow::setOcrEngineCommon()
{
    if(Settings::getOcrEnableWhitelist())
    {
        ocrEngine->setWhitelist(Settings::getOcrWhitelist());
    }
    else
    {
        ocrEngine->setWhitelist("");
    }

    if(Settings::getOcrEnableBlacklist())
    {
        ocrEngine->setBlacklist(Settings::getOcrBlacklist());
    }
    else
    {
        ocrEngine->setBlacklist("");
    }

    ocrEngine->setConfigFile(Settings::getOcrTesseractConfigFile());
}

QString MainWindow::getDebugImagePath(QString filename)
{
    return UtilsImg::getDebugScreenshotPath(filename, Settings::getDebugAppendTimestampToImage(), captureTimestamp);
}

void MainWindow::performForwardTextLineCapture(QPoint pt)
{
    int idealRectWidth = Settings::getForwardTextLineCaptureWidth();
    int idealRectHalfWidth = idealRectWidth / 2;
    int idealRectLength = Settings::getForwardTextLineCaptureLength();
    int idealStartOffset = Settings::getForwardTextLineCaptureStartOffset();

    captureTimestamp = QDateTime::currentDateTime();

    QString savedTextOrientation = Settings::getOcrTextOrientation();

    int totalScreenWidth = 0;

    for(auto item : qApp->screens())
    {
        totalScreenWidth += item->geometry().width();
    }

    QDesktopWidget desktopWidget;
    QRect screenRect = desktopWidget.screenGeometry(pt);

    bool isVertical = false;

    if(UtilsLang::languageSupportsVerticalOrientation(Settings::getOcrLang())
            && (savedTextOrientation == "Auto" || savedTextOrientation == "Vertical"))
    {
        isVertical = true;
    }

    QRect cropRect;

    if(isVertical)
    {
        cropRect.setLeft(qMax(screenRect.x(), pt.x() - idealRectHalfWidth));
        cropRect.setTop(qMax(screenRect.y(), pt.y() - idealStartOffset));
        cropRect.setRight(qMin(totalScreenWidth, pt.x() + idealRectHalfWidth));
        cropRect.setBottom(qMin(screenRect.height(), pt.y() + idealRectLength));
    }
    else
    {
        cropRect.setLeft(qMax(screenRect.x(), pt.x() - idealStartOffset));
        cropRect.setTop(qMax(screenRect.y(), pt.y() - idealRectHalfWidth));
        cropRect.setRight(qMin(totalScreenWidth, pt.x() + idealRectLength));
        cropRect.setBottom(qMin(screenRect.height(), pt.y() + idealRectHalfWidth));
    }

    QImage image = UtilsImg::takeScreenshot(cropRect);

    if(image.isNull())
    {
        return;
    }

    if(Settings::getDebugSaveCaptureImage())
    {
        image.save(getDebugImagePath("debug_capture.png"));
    }

    preProcess.setVerticalOrientation(isVertical);
    preProcess.setRemoveFurigana(UtilsLang::languageSupportsFurigana(Settings::getOcrLang()));
    preProcess.setScaleFactor(Settings::getOcrScaleFactor());

    // Get the click point relative to the cropped area
    Point ptInCropRect(pt.x() - cropRect.left(), pt.y() - cropRect.top());
    PIX *inPixs = preProcess.convertImageToPix(image);

    PIX *pixs = preProcess.extractTextBlock(inPixs,
                                            ptInCropRect.x,
                                            ptInCropRect.y,
                                            Settings::getForwardTextLineCaptureLookahead(),
                                            Settings::getForwardTextLineCaptureLookbehind(),
                                            Settings::getForwardTextLineCaptureSearchRadius());
    pixDestroy(&inPixs);

    if(pixs == nullptr)
    {
        return;
    }

    if(pixs->w <= (unsigned int)minOcrWidth || pixs->h <= (unsigned int)minOcrHeight)
    {
        pixDestroy(&pixs);
        return;
    }

    if(Settings::getDebugSaveEnhancedImage())
    {
        QString savePath = getDebugImagePath("debug_enhanced.png");
        QByteArray byteArray = savePath.toLocal8Bit();
        pixWriteImpliedFormat(byteArray.constData(), pixs, 0, 0);
    }

    ocrEngine->setVerticalOrientation(isVertical);
    setOcrEngineCommon();
    QString ocrText = ocrEngine->performOcr(pixs, true);
    pixDestroy(&pixs);

    if(Settings::getForwardTextLineCaptureFirstWord())
    {
        if(ocrText.contains(' '))
        {
            ocrText = ocrText.left(ocrText.indexOf(' '));
        }

        if(ocrText.length() > 1)
        {
            ocrText = ocrText.trimmed();
            ocrText = ocrText.replace(QRegularExpression("[\\.,!\\?;:]$"), "");
        }
    }

    QRect boundingBox = preProcess.getBoundingRect();

    QRect displayRect;
    displayRect.setLeft(pt.x() - ptInCropRect.x + boundingBox.x() + 1);
    displayRect.setTop(pt.y() - ptInCropRect.y + boundingBox.y() + 1);
    displayRect.setRight(displayRect.left() + boundingBox.width());
    displayRect.setBottom(displayRect.top() + boundingBox.height());

    if(displayRect.width() > minOcrWidth && displayRect.height() > minOcrHeight)
    {
        autoCaptureBox.autoCapture(displayRect);
        ocrText = postProcess(ocrText);
        outputOcrText(ocrText);
    }
}

void MainWindow::performTextLineCapture(QPoint pt)
{
    int idealRectWidth = Settings::getTextLineCaptureWidth();
    int idealRectHalfWidth = idealRectWidth / 2;
    int idealRectLength = Settings::getTextLineCaptureLength();
    int idealRectHalfLength = idealRectLength / 2;

    captureTimestamp = QDateTime::currentDateTime();

    QString savedTextOrientation = Settings::getOcrTextOrientation();

    int totalScreenWidth = 0;

    for(auto item : qApp->screens())
    {
        totalScreenWidth += item->geometry().width();
    }

    QDesktopWidget desktopWidget;
    QRect screenRect = desktopWidget.screenGeometry(pt);

    bool isVertical = false;

    if(UtilsLang::languageSupportsVerticalOrientation(Settings::getOcrLang())
            && (savedTextOrientation == "Auto" || savedTextOrientation == "Vertical"))
    {
        isVertical = true;
    }

    QRect cropRect;

    if(isVertical)
    {
        cropRect.setLeft(qMax(screenRect.x(), pt.x() - idealRectHalfWidth));
        cropRect.setTop(qMax(screenRect.y(), pt.y() - idealRectHalfLength));
        cropRect.setRight(qMin(totalScreenWidth, pt.x() + idealRectHalfWidth));
        cropRect.setBottom(qMin(screenRect.height(), pt.y() + idealRectHalfLength));
    }
    else
    {
        cropRect.setLeft(qMax(screenRect.x(), pt.x() - idealRectHalfLength));
        cropRect.setTop(qMax(screenRect.y(), pt.y() - idealRectHalfWidth));
        cropRect.setRight(qMin(totalScreenWidth, pt.x() + idealRectHalfLength));
        cropRect.setBottom(qMin(screenRect.height(), pt.y() + idealRectHalfWidth));
    }

    QImage image = UtilsImg::takeScreenshot(cropRect);

    if(image.isNull())
    {
        return;
    }

    if(Settings::getDebugSaveCaptureImage())
    {
        image.save(getDebugImagePath("debug_capture.png"));
    }

    preProcess.setVerticalOrientation(isVertical);
    preProcess.setRemoveFurigana(UtilsLang::languageSupportsFurigana(Settings::getOcrLang()));
    preProcess.setScaleFactor(Settings::getOcrScaleFactor());

    // Get the click point relative to the cropped area
    Point ptInCropRect(pt.x() - cropRect.left(), pt.y() - cropRect.top());
    PIX *inPixs = preProcess.convertImageToPix(image);
    PIX *pixs = preProcess.extractTextBlock(inPixs,
                                            ptInCropRect.x,
                                            ptInCropRect.y,
                                            Settings::getTextLineCaptureLookahead(),
                                            Settings::getTextLineCaptureLookbehind(),
                                            Settings::getTextLineCaptureSearchRadius());

    pixDestroy(&inPixs);

    if(pixs == nullptr)
    {
        qDebug() << "performTextLineCapture failed";
        return;
    }

    if(pixs->w <= (unsigned int)minOcrWidth || pixs->h <= (unsigned int)minOcrHeight)
    {
        pixDestroy(&pixs);
        return;
    }

    if(Settings::getDebugSaveEnhancedImage())
    {
        QString savePath = getDebugImagePath("debug_enhanced.png");
        QByteArray byteArray = savePath.toLocal8Bit();
        pixWriteImpliedFormat(byteArray.constData(), pixs, 0, 0);
    }

    ocrEngine->setVerticalOrientation(isVertical);
    setOcrEngineCommon();
    QString ocrText = ocrEngine->performOcr(pixs, true);
    pixDestroy(&pixs);

    QRect boundingBox = preProcess.getBoundingRect();

    QRect displayRect;
    displayRect.setLeft(pt.x() - ptInCropRect.x + boundingBox.x() + 1);
    displayRect.setTop(pt.y() - ptInCropRect.y + boundingBox.y() + 1);
    displayRect.setRight(displayRect.left() + boundingBox.width());
    displayRect.setBottom(displayRect.top() + boundingBox.height());

    if(displayRect.width() > minOcrWidth && displayRect.height() > minOcrHeight)
    {
        autoCaptureBox.autoCapture(displayRect);
        ocrText = postProcess(ocrText);
        outputOcrText(ocrText);
    }
}

void MainWindow::performBubbleCapture(QPoint pt)
{
    int idealRectWidth = Settings::getBubbleCaptureWidth();
    int idealRectHalfWidth = idealRectWidth / 2;
    int idealRectHeight = Settings::getBubbleCaptureHeight();
    int idealRectHalfHeight = idealRectHeight / 2;

    captureTimestamp = QDateTime::currentDateTime();

    QString savedTextOrientation = Settings::getOcrTextOrientation();

    int totalScreenWidth = 0;

    for(auto item : qApp->screens())
    {
        totalScreenWidth += item->geometry().width();
    }

    QDesktopWidget desktopWidget;
    QRect screenRect = desktopWidget.screenGeometry(pt);
    QRect cropRect;

    cropRect.setLeft(qMax(screenRect.x(), pt.x() - idealRectHalfWidth));
    cropRect.setTop(qMax(screenRect.y(), pt.y() - idealRectHalfHeight));
    cropRect.setRight(qMin(totalScreenWidth, pt.x() + idealRectHalfWidth));
    cropRect.setBottom(qMin(screenRect.height(), pt.y() + idealRectHalfHeight));

    QImage image = UtilsImg::takeScreenshot(cropRect);

    if(image.isNull())
    {
        return;
    }

    if(Settings::getDebugSaveCaptureImage())
    {
        image.save(getDebugImagePath("debug_capture.png"));
    }

    bool isVertical = false;

    if(UtilsLang::languageSupportsVerticalOrientation(Settings::getOcrLang())
            && (savedTextOrientation == "Auto" || savedTextOrientation == "Vertical"))
    {
        isVertical = true;
    }

    preProcess.setVerticalOrientation(isVertical);
    preProcess.setRemoveFurigana(UtilsLang::languageSupportsFurigana(Settings::getOcrLang()));
    preProcess.setScaleFactor(Settings::getOcrScaleFactor());

    Point ptInCropRect(pt.x() - cropRect.left(), pt.y() - cropRect.top());
    PIX *inPixs = preProcess.convertImageToPix(image);
    PIX *pixs = preProcess.extractBubbleText(inPixs, ptInCropRect.x, ptInCropRect.y);
    pixDestroy(&inPixs);

    if(pixs == nullptr)
    {
        return;
    }

    if(pixs->w <= (unsigned int)minOcrWidth || pixs->h <= (unsigned int)minOcrHeight)
    {
        pixDestroy(&pixs);
        return;
    }

    if(Settings::getDebugSaveEnhancedImage())
    {
        QString savePath = getDebugImagePath("debug_enhanced.png");
        QByteArray byteArray = savePath.toLocal8Bit();
        pixWriteImpliedFormat(byteArray.constData(), pixs, 0, 0);
    }

    ocrEngine->setVerticalOrientation(isVertical);
    setOcrEngineCommon();

    bool singleLine = false;

    if(UtilsLang::languageSupportsFurigana(Settings::getOcrLang()))
    {
        singleLine = (preProcess.getJapNumTextLines() == 1);
    }

    QString ocrText = ocrEngine->performOcr(pixs, singleLine);
    pixDestroy(&pixs);

    QRect boundingBox = preProcess.getBoundingRect();
    QRect displayRect;
    displayRect.setLeft(cropRect.left() + boundingBox.x() + 1);
    displayRect.setTop(cropRect.top() + boundingBox.y() + 1);
    displayRect.setRight(displayRect.left() + boundingBox.width());
    displayRect.setBottom(displayRect.top() + boundingBox.height());

    if(displayRect.width() > minOcrWidth && displayRect.height() > minOcrHeight)
    {
        autoCaptureBox.autoCapture(displayRect);
        ocrText = postProcess(ocrText);
        outputOcrText(ocrText);
    }
}

void MainWindow::showDocumentation()
{
    QDesktopServices::openUrl(QUrl("http://capture2text.sourceforge.net"));
}

void MainWindow::showAbout()
{
    aboutDialog.show();
}

void MainWindow::showSettingsDialog()
{
    settingsDialog.show();
}

void MainWindow::registerHotkeys()
{
    KeyboardHook::getInstance().addHotkey(CAPTURE_BOX, Hotkey(Settings::getHotkeyCaptureBox()));
    KeyboardHook::getInstance().addHotkey(RE_CAPTURE_LAST, Hotkey(Settings::getHotkeyReCaptureLast()));
    KeyboardHook::getInstance().addHotkey(TEXTLINE_CAPTURE, Hotkey(Settings::getHotkeyTextLineCapture()));
    KeyboardHook::getInstance().addHotkey(FORWARD_TEXTLINE_CAPTURE, Hotkey(Settings::getHotkeyForwardTextLineCapture()));
    KeyboardHook::getInstance().addHotkey(BUBBLE_CAPTURE, Hotkey(Settings::getHotkeyBubbleCapture()));
    KeyboardHook::getInstance().addHotkey(LANG_1, Hotkey(Settings::getHotkeyLang1()));
    KeyboardHook::getInstance().addHotkey(LANG_2, Hotkey(Settings::getHotkeyLang2()));
    KeyboardHook::getInstance().addHotkey(LANG_3, Hotkey(Settings::getHotkeyLang3()));
    KeyboardHook::getInstance().addHotkey(TEXT_ORIENTATION, Hotkey(Settings::getHotkeyTextOrientation()));
    KeyboardHook::getInstance().addHotkey(ENABLE_WHITELIST, Hotkey(Settings::getHotkeyWhitelist()));
    KeyboardHook::getInstance().addHotkey(ENABLE_BLACKLIST, Hotkey(Settings::getHotkeyBlacklist()));
}

void MainWindow::settingsAccepted()
{
    registerHotkeys();

    captureBox.setBackgroundColor(Settings::getCaptureBoxBackgroundColor());
    captureBox.setBorderColor(Settings::getCaptureBoxBorderColor());

    autoCaptureBox.setBorderColor(Settings::getCaptureBoxBorderColor());

    setOcrLang(Settings::getOcrLang());

    checkCurrentTextOrientationInMenu();

    previewBox.setBackgroundColor(Settings::getPreviewBackgroundColor());
    previewBox.setBorderColor(Settings::getPreviewBorderColor());
    previewBox.setTextColor(Settings::getPreviewTextColor());
    previewBox.setTextFont(Settings::getPreviewTextFont());

    infoBox.setBackgroundColor(Settings::getPreviewBackgroundColor());
    infoBox.setBorderColor(Settings::getPreviewBorderColor());
    infoBox.setTextColor(Settings::getPreviewTextColor());
    infoBox.setTextFont(Settings::getPreviewTextFont());

    actionSaveToClipboard->setChecked(Settings::getOutputClipboard());
    actionShowPopupWindow->setChecked(Settings::getOutputShowPopup());
}

void MainWindow::selectOutputClipboardFromMenu()
{
    Settings::setOutputClipboard(actionSaveToClipboard->isChecked());
}

void MainWindow::selectOutputPopupFromMenu()
{
    Settings::setOutputShowPopup(actionShowPopupWindow->isChecked());
}

void MainWindow::setOcrLang(QString lang)
{
    actionGroupOcrLang->checkedAction()->setChecked(false);

    for(auto action : actionGroupOcrLang->actions())
    {
        if(action->text() == lang)
        {
            action->setChecked(true);
            break;
        }
    }

    Settings::setOcrLang(lang);
    QtConcurrent::run(ocrEngine, &OcrEngine::setLang, lang);
}

void MainWindow::captureBoxMoved()
{
    const int gap = 5;

    if(!Settings::getPreviewEnabled())
    {
        return;
    }

    QString savedPreviewPos = Settings::getPreviewPosition();
    QPoint pt(0, 0);
    int previewBoxHeight = previewBox.getBoxHeight();

    if(savedPreviewPos == "Dynamic - Top Left")
    {
        pt = captureBox.getCaptureRect().topLeft();
        pt -= QPoint(0, previewBoxHeight + gap);
    }
    else if(savedPreviewPos == "Dynamic - Bottom Left")
    {
        pt = captureBox.getCaptureRect().bottomLeft();
        pt += QPoint(0, gap);
    }
    else if(savedPreviewPos == "Dynamic - Right Edge Top")
    {
        pt = captureBox.getCaptureRect().topRight();
        pt += QPoint(gap, 0);
    }
    else if(savedPreviewPos == "Dynamic - Right Edge Bottom")
    {
        pt = captureBox.getCaptureRect().bottomRight();
        pt += QPoint(gap, -previewBoxHeight);
    }
    else if(savedPreviewPos == "Dynamic - Right Edge Center")
    {
        pt = QPoint(captureBox.getCaptureRect().right() + gap,
                    captureBox.getCaptureRect().center().y() - previewBoxHeight / 2);
    }
    else if(savedPreviewPos == "Fixed - Bottom Left")
    {
        pt = QApplication::desktop()->availableGeometry().bottomLeft();
        pt -= QPoint(0, previewBoxHeight);
    }
    else // Fixed - Top Left
    {
        pt = QApplication::desktop()->availableGeometry().topLeft();
    }

    previewBox.move(pt);
}

void MainWindow::ocrPreviewComplete()
{
    if(!Settings::getPreviewEnabled() || previewBox.isHidden())
    {
        return;
    }

    QString ocrText = watcherPreview.result();
    ocrText = postProcess(ocrText, true);

    previewBox.setText(ocrText);
    previewBox.update();

    if(pendingPreviewRequest)
    {
        // Capture box was moved while OCR preview was taking place.
        // Now that this preview is complete, perform another preview.
        pendingPreviewRequest = false;
        captureBoxStoppedMoving();
    }
}

void MainWindow::captureBoxStoppedMoving()
{
    if(!Settings::getPreviewEnabled())
    {
        return;
    }

    QRect captureRect = captureBox.getCaptureRect();
    if(captureRect.width() <= minOcrWidth || captureRect.height() <= minOcrHeight)
    {
        return;
    }

    if(watcherPreview.isRunning())
    {
        // OCR preview currently in progress. Set flag to indicate that we need
        // to perform another OCR preview after current OCR preview is done.
        pendingPreviewRequest = true;
        return;
    }

    pendingPreviewRequest = false;

    // Run OCR for preview in separate thread so that capture box moving remains smooth.
    // When OCR is done, the routine connected to watcherPreview's finished() signal will be called.
    QFuture<QString> futurePreview = QtConcurrent::run(this, &MainWindow::ocrCaptureBoxArea);
    watcherPreview.setFuture(futurePreview);
}

void MainWindow::captureBoxCancel()
{
    previewBox.hideAndReset();
}

void MainWindow::selectLangFromMenu()
{
    QAction *action = actionGroupOcrLang->checkedAction();
    QString lang = action->text();
    setOcrLang(lang);
}

void MainWindow::selectTextOrientationAutoFromMenu()
{
    Settings::setOcrTextOrientation("Auto");
}

void MainWindow::selectTextOrientationHorizontalFromMenu()
{
    Settings::setOcrTextOrientation("Horizontal");
}

void MainWindow::selectTextOrientationVerticalAutoFromMenu()
{
    Settings::setOcrTextOrientation("Vertical");
}

QString MainWindow::postProcess(QString text, bool forceRemoveLineBreaks)
{
    PostProcess postProcess(Settings::getOcrLang(),
                            Settings::getOutputKeepLineBreaks() && !forceRemoveLineBreaks);
    postProcess.setReplacementList(Settings::getOcrReplacementList(Settings::getOcrLang()));

    text = postProcess.postProcessOcrText(text);

    if(Settings::getDebugPrependCoords())
    {
        int x1, y1, x2, y2;
        captureBox.getCaptureRect().getCoords(&x1, &y1, &x2, &y2);
        text = QString("(%1 %2 %3 %4) ").arg(x1).arg(y1).arg(x2).arg(y2) + text;
    }

    return text;
}

void MainWindow::translationComplete(QString phrase, QString translation, bool error)
{
    if(error)
    {
        outputOcrTextPhase2(phrase, "<Server Timeout>");
    }
    else
    {
        outputOcrTextPhase2(phrase, translation);
    }
}

void MainWindow::outputOcrText(QString text)
{
    QString ocrLang = OcrEngine::altLangToLang(Settings::getOcrLang());
    QString translateLang = Settings::getTranslateLang(Settings::getOcrLang());

    Speech::getInstance().sayText(text);

    if((Settings::getTranslateAddToClipboard()
           || Settings::getTranslateAddToPopup()
           || (Settings::getOutputLogFileEnable() && Settings::getOutputLogFormat().contains("${translation}")))
        && ocrLang != "None" && translateLang != "<Do Not Translate>")
    {
        if(!translate.startTranslate(text, ocrLang, translateLang, Settings::getTranslateServerTimeout()))
        {
            outputOcrTextPhase2(text, "<Error>");
        }
    }
    else
    {
        outputOcrTextPhase2(text, "");
    }
}

void MainWindow::outputOcrTextPhase2(QString text, QString translation)
{
    QString translateLang = Settings::getTranslateLang(Settings::getOcrLang());
    QString translationSeparator = Settings::separatorToStr(Settings::getTranslateSeparator());

    if(Settings::getOutputClipboard())
    {
        QString clipboardText = text;

        if(Settings::getTranslateAddToClipboard()
                && translateLang != "<Do Not Translate>"
                && translation != "")
        {
            clipboardText += translationSeparator + translation;
        }

        QApplication::clipboard()->setText(clipboardText);
    }

    if(Settings::getOutputShowPopup())
    {
        QString popupText = text;

        if(Settings::getTranslateAddToPopup()
                && translateLang != "<Do Not Translate>"
                && translation != "")
        {
            popupDialog.setText(popupText, translation);
        }
        else
        {
            popupDialog.setText(popupText, "");
        }

        popupDialog.show();
        popupDialog.raise();
    }

    if(Settings::getOutputLogFileEnable())
    {
        QFile logFile(Settings::getOutputLogFile());

        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream stream(&logFile);
            stream.setCodec("UTF-8");
            stream.setGenerateByteOrderMark(true);
            stream << UtilsCommon::formatLogLine(Settings::getOutputLogFormat(), text, captureTimestamp, translation, "");
            logFile.close();
        }
    }
}

void MainWindow::hotkeyPressed(int id)
{
    if(id == FORWARD_TEXTLINE_CAPTURE)
    {
        performForwardTextLineCapture(QCursor::pos());
    }
    else if(id == CAPTURE_BOX)
    {
        if(captureBox.isVisible())
        {
            endCaptureBox();
        }
        else
        {
            startCaptureBox();
        }
    }
    else if(id == RE_CAPTURE_LAST)
    {
        endCaptureBox();
    }
    else if(id == LANG_1)
    {
        QString lang = Settings::getOcrQuickAccessLang1();

        if(OcrEngine::isLangInstalled(lang))
        {
            setOcrLang(lang);
            infoBox.showInfo(lang);
        }
        else
        {
            infoBox.showInfo(lang + " is not installed!");
        }
    }
    else if(id == LANG_2)
    {
        QString lang = Settings::getOcrQuickAccessLang2();

        if(OcrEngine::isLangInstalled(lang))
        {
            setOcrLang(lang);
            infoBox.showInfo(lang);
        }
        else
        {
            infoBox.showInfo(lang + " is not installed!");
        }
    }
    else if(id == LANG_3)
    {
        QString lang = Settings::getOcrQuickAccessLang3();

        if(OcrEngine::isLangInstalled(lang))
        {
            setOcrLang(lang);
            infoBox.showInfo(lang);
        }
        else
        {
            infoBox.showInfo(lang + " is not installed!");
        }
    }
    else if(id == TEXT_ORIENTATION)
    {
        if(Settings::getOcrTextOrientation() == "Auto")
        {
            Settings::setOcrTextOrientation("Horizontal");
        }
        else if(Settings::getOcrTextOrientation() == "Horizontal")
        {
            Settings::setOcrTextOrientation("Vertical");
        }
        else
        {
            Settings::setOcrTextOrientation("Auto");
        }

        checkCurrentTextOrientationInMenu();
        infoBox.showInfo(Settings::getOcrTextOrientation());
    }
    else if(id == BUBBLE_CAPTURE)
    {
        performBubbleCapture(QCursor::pos());
    }
    else if(id == TEXTLINE_CAPTURE)
    {
        performTextLineCapture(QCursor::pos());
    }
    else if(id == ENABLE_WHITELIST)
    {
        bool enabled = !Settings::getOcrEnableWhitelist();
        Settings::setOcrEnableWhitelist(enabled);
        infoBox.showInfo(enabled ? "Whitelist Enabled": "Whitelist Disabled");
    }
    else if(id == ENABLE_BLACKLIST)
    {
        bool enabled = !Settings::getOcrEnableBlacklist();
        Settings::setOcrEnableBlacklist(enabled);
        infoBox.showInfo(enabled ? "Blacklist Enabled": "Blacklist Disabled");
    }
}

void MainWindow::checkCurrentTextOrientationInMenu()
{
    for(auto action : actionGroupTextOrientation->actions())
    {
        action->setChecked(action->text() == Settings::getOcrTextOrientation());
    }
}

void MainWindow::createTrayMenu()
{
    menuTrayIcon = new QMenu(this);

    //
    // Settings
    //

    QAction *actionSettings = menuTrayIcon->addAction(tr("&Settings..."));
    actionSettings->setIcon(QIcon(":/img/img/Settings.png"));
    connect(actionSettings, &QAction::triggered, this, &MainWindow::showSettingsDialog);

    menuTrayIcon->addSeparator();

    //
    // Output options
    //
    actionSaveToClipboard = menuTrayIcon->addAction(tr("Save to &Clipboard"));
    actionSaveToClipboard->setIcon(QIcon(":/img/img/Clipboard.png"));
    actionSaveToClipboard->setCheckable(true);
    connect(actionSaveToClipboard, &QAction::triggered, this, &MainWindow::selectOutputClipboardFromMenu);

    if(Settings::getOutputClipboard())
    {
        actionSaveToClipboard->setChecked(true);
    }

    bool savedOutputPopup = Settings::getOutputShowPopup();
    actionShowPopupWindow = menuTrayIcon->addAction(tr("Show &Popup Window"));
    actionShowPopupWindow->setIcon(QIcon(":/img/img/Window.png"));
    actionShowPopupWindow->setCheckable(true);
    connect(actionShowPopupWindow, &QAction::triggered, this, &MainWindow::selectOutputPopupFromMenu);

    if(savedOutputPopup)
    {
        actionShowPopupWindow->setChecked(true);
    }

    menuTrayIcon->addSeparator();

    //
    // OCR Languages
    //
    QMenu *submenuOcrLang = menuTrayIcon->addMenu(tr("&OCR Language"));
    actionGroupOcrLang = new QActionGroup(this);

    QStringList installedNames = ocrEngine->getInstalledLangs();
    QString savedLang = Settings::getOcrLang();

    for(auto langName : installedNames)
    {
        QAction *actionCurLang = submenuOcrLang->addAction(langName);
        actionCurLang->setCheckable(true);

        if(langName == savedLang)
        {
            actionCurLang->setChecked(true);
        }

        actionCurLang->setData(langName);
        connect(actionCurLang, &QAction::triggered, this, &MainWindow::selectLangFromMenu);
        actionGroupOcrLang->addAction(actionCurLang);
    }

    //
    // Text Orientation
    //
    QMenu *submenuTextOrientation = menuTrayIcon->addMenu(tr("Text &Orientation"));
    QString savedTextOrientation = Settings::getOcrTextOrientation();
    QAction *actionTextOrientationAuto = submenuTextOrientation->addAction(tr("Auto"));
    actionTextOrientationAuto->setIcon(QIcon(":/img/img/Text_Orientation_Auto.png"));
    actionTextOrientationAuto->setCheckable(true);
    connect(actionTextOrientationAuto, &QAction::triggered, this, &MainWindow::selectTextOrientationAutoFromMenu);

    if(savedTextOrientation == "Auto")
    {
        actionTextOrientationAuto->setChecked(true);
    }

    QAction *actionTextOrientationHorizontal = submenuTextOrientation->addAction(tr("Horizontal"));
    actionTextOrientationHorizontal->setIcon(QIcon(":/img/img/Text_Orientation_Horiz.png"));
    actionTextOrientationHorizontal->setCheckable(true);
    connect(actionTextOrientationHorizontal, &QAction::triggered, this, &MainWindow::selectTextOrientationHorizontalFromMenu);

    if(savedTextOrientation == "Horizontal")
    {
        actionTextOrientationHorizontal->setChecked(true);
    }

    QAction *actionTextOrientationVertical = submenuTextOrientation->addAction(tr("Vertical"));
    actionTextOrientationVertical->setIcon(QIcon(":/img/img/Text_Orientation_Vert.png"));
    actionTextOrientationVertical->setCheckable(true);
    connect(actionTextOrientationVertical, &QAction::triggered, this, &MainWindow::selectTextOrientationVerticalAutoFromMenu);

    if(savedTextOrientation == "Vertical")
    {
        actionTextOrientationVertical->setChecked(true);
    }

    // Add the text orientations to an action group so that only one can be checked
    actionGroupTextOrientation = new QActionGroup(this);
    actionGroupTextOrientation->addAction(actionTextOrientationAuto);
    actionGroupTextOrientation->addAction(actionTextOrientationHorizontal);
    actionGroupTextOrientation->addAction(actionTextOrientationVertical);

    menuTrayIcon->addSeparator();

    //
    // Help, Exit
    //

    QMenu *submenuHelp = menuTrayIcon->addMenu(tr("&Help"));
    submenuHelp->setIcon(QIcon(":/img/img/Help.png"));
    QAction *actionDocumentation = submenuHelp->addAction(tr("&Documentation..."));
    connect(actionDocumentation, &QAction::triggered, this, &MainWindow::showDocumentation);

    QAction *actionAbout = submenuHelp->addAction(tr("&About..."));
    connect(actionAbout, &QAction::triggered, this, &MainWindow::showAbout);

    QAction* actionExit = menuTrayIcon->addAction(tr("E&xit"));
    actionExit->setIcon(QIcon(":/img/img/Close.png"));
    connect(actionExit, &QAction::triggered, qApp, &QApplication::quit);

    //
    // Show the tray icon
    //
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/img/img/Logo.png"));
    trayIcon->setContextMenu(menuTrayIcon);
    trayIcon->show();
}


