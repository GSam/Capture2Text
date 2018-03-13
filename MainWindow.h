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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QActionGroup>
#include <QFutureWatcher>

#include "AboutDialog.h"
#include "CaptureBox.h"
#include "OcrEngine.h"
#include "PopupDialog.h"
#include "PreProcess.h"
#include "Preview.h"
#include "SettingsDialog.h"
#include "Translate.h"
#include "WelcomeDialog.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(bool portable);
    ~MainWindow();

private slots:
    void hotkeyPressed(int id);

private:
    const int minOcrWidth = 3;
    const int minOcrHeight = 3;

    void captureBoxCaptured();
    void startCaptureBox();
    void endCaptureBox();
    void createTrayMenu();
    void populateOcrLangMap();
    void showDocumentation();
    void showAbout();
    void showSettingsDialog();
    void selectOutputClipboardFromMenu();
    void selectOutputPopupFromMenu();
    void selectModeCaptureBoxFromMenu();
    void selectModeTriggerFromMenu();
    void selectLangFromMenu();
    void selectTextOrientationAutoFromMenu();
    void selectTextOrientationHorizontalFromMenu();
    void selectTextOrientationVerticalAutoFromMenu();
    void outputOcrText(QString text);
    void performForwardTextLineCapture(QPoint pt);
    void performTextLineCapture(QPoint pt);
    void performBubbleCapture(QPoint pt);
    QImage takeScreenshot(QRect &rect);
    bool isOrientationVertical();
    void setOcrLang(QString lang);
    void captureBoxMoved();
    void captureBoxStoppedMoving();
    void captureBoxCancel();
    QString ocrCaptureBoxArea();
    void ocrPreviewComplete();
    void ocrCaptureComplete();
    void settingsAccepted();
    QString postProcess(QString text, bool forceRemoveLineBreaks=false);
    void registerHotkeys();
    void checkCurrentTextOrientationInMenu();
    void outputOcrTextPhase2(QString text, QString translation);
    void translationComplete(QString phrase, QString translation, bool error);
    void setOcrEngineCommon();
    QString getDebugImagePath(QString filename);

    enum HotkeyAction
    {
        CAPTURE_BOX = 1000,
        RE_CAPTURE_LAST,
        TEXTLINE_CAPTURE,
        FORWARD_TEXTLINE_CAPTURE,
        BUBBLE_CAPTURE,
        LANG_1,
        LANG_2,
        LANG_3,
        TEXT_ORIENTATION,
        ENABLE_WHITELIST,
        ENABLE_BLACKLIST,
    };

    CaptureBox captureBox;
    CaptureBox autoCaptureBox;
    Preview previewBox;
    Preview infoBox;
    OcrEngine *ocrEngine;
    PreProcess preProcess;
    QSystemTrayIcon *trayIcon;
    QDateTime captureTimestamp;

    QMenu *menuTrayIcon;
    QAction *actionSaveToClipboard;
    QAction *actionShowPopupWindow;
    QActionGroup *actionGroupOcrLang;
    QActionGroup *actionGroupTextOrientation;

    QFutureWatcher<QString> watcherPreview;
    bool pendingPreviewRequest;

    QFutureWatcher<QString> watcherCapture;
    PopupDialog popupDialog;
    SettingsDialog settingsDialog;
    AboutDialog aboutDialog;
    WelcomeDialog welcomeDialog;

    Translate translate;
};

#endif // MAINWINDOW_H
