QT += core

!console {
    QT += gui network texttospeech
    QT += widgets
}

TARGET = Capture2Text

console {
    TARGET = Capture2Text_CLI
}

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

win32{
# Disable warning: C4305: 'initializing': truncation from 'double' to 'l_float32'
 QMAKE_CXXFLAGS += /wd4305

# Disable warning: C4099: 'ETEXT_DESC': type name first seen using 'class' now seen using 'struct'
 QMAKE_CXXFLAGS += /wd4099
}
else{

QMAKE_CXXFLAGS += -std=c++11
}

console {
    DEFINES += CLI_BUILD
}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
    Furigana.cpp \
    BoundingTextRect.cpp \
    RunGuard.cpp \
    CommandLine.cpp \
    UtilsLang.cpp \
    UtilsImg.cpp \
    PostProcess.cpp \
    PreProcess.cpp \
    OcrEngine.cpp \
    UtilsCommon.cpp


HEADERS  += \
    Furigana.h \
    BoundingTextRect.h \
    CommandLine.h \
    UtilsLang.h \
    UtilsImg.h \
    PostProcess.h \
    PreProcess.h \
    PreProcessCommon.h \
    OcrEngine.h \
    UtilsCommon.h

!console {
    SOURCES += \
        MainWindow.cpp \
        SettingsDialog.cpp \
        Settings.cpp \
        PopupDialog.cpp \
        Preview.cpp \
        SampleBox.cpp \
        Translate.cpp \
        Hotkey.cpp \
        HotkeyWidget.cpp \
        CaptureBox.cpp \
        AboutDialog.cpp \
        WelcomeDialog.cpp \
        KeyboardHook.cpp \
        MouseHook.cpp \
        Speech.cpp

    HEADERS  += \
        MainWindow.h \
        RunGuard.h \
        SettingsDialog.h \
        Preview.h \
        Settings.h \
        PopupDialog.h \
        SampleBox.h \
        Translate.h \
        Hotkey.h \
        HotkeyWidget.h \
        CaptureBox.h \
        AboutDialog.h \
        ReplyTimeout.h \
        WelcomeDialog.h \
        KeyboardHook.h \
        MouseHook.h \
        Speech.h

    FORMS    += \
        SettingsDialog.ui \
        PopupDialog.ui \
        HotkeyWidget.ui \
        AboutDialog.ui \
        WelcomeDialog.ui
}


# Linux Paths
unix{
INCLUDEPATH += /usr/include/tesseract/
INCLUDEPATH += /usr/include/leptonica/
}
win32{
 CONFIG += conan_basic_setup
 include ( conanbuildinfo.pri)
}
# Tesseract and Leptonica
unix{
LIBS += -ltesseract
LIBS += -llept
}

!console {
    RESOURCES += \
        resource.qrc
}

# Needed for the icon
RC_FILE = Capture2Text.rc


include (QHotkey/qhotkey.pri)
