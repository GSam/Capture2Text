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

#include <QClipboard>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QTextStream>
#include "CommandLine.h"
#include "UtilsImg.h"
#include "UtilsCommon.h"
#include "UtilsLang.h"
#include "PostProcess.h"

CommandLine::CommandLine()
    : debug(false),
      debugAppendTimestamp(false),
      keepLineBreaks(false),
      outputFilePath(""),
      outputFormat("${capture}${linebreak}")
{
    ocrEngine = new OcrEngine();
}

CommandLine::~CommandLine()
{
    delete ocrEngine;
}

/*
Options:
  -?, -h, --help                     Displays this help.
  -v, --version                      Displays version information.
  -b, --line-breaks                  Do not remove line breaks from OCR text.
  -d, --debug                        Output captured image and pre-processed
                                     image for debugging purposes.
  --debug-timestamp                  Append timestamp to debug images when
                                     using the -d option.
  -f, --images-file <file>           File that contains paths of image files to
                                     OCR. One path per line.
  -i, --image <file>                 Image file to OCR. You may OCR multiple
                                     image files like so: "-i <img1> -i <img2>
                                     -i <img3>"
  -l, --language <language>          OCR language to use. Case-sensitive.
                                     Default is "English". Use the
                                     --show-languages option to list installed
                                     OCR languages.
  -o, --output-file <file>           Output OCR text to this file. If not
                                     specified, stdout will be used.
  --output-file-append               Append to file when using the -o option.
  -s, --screen-rect <"x1 y1 x2 y2">  Coordinates of rectangle that defines area
                                     of screen to OCR.
  -t, --vertical                     OCR vertical text. If not specified,
                                     horizontal text is assumed.
  -w, --show-languages               Show installed languages that may be used
                                     with the "--language" option.
  --output-format <format>           Format to use when outputting OCR text.
                                     You may use these tokens:
                                     ${capture}   : OCR Text.
                                     ${linebreak} : Line break (\r\n).
                                     ${tab}       : Tab character.
                                     ${timestamp} : Time that screen or each
                                     file was processed.
                                     ${file}      : File that was processed or
                                     screen rect.
                                     Default format is "${capture}${linebreak}".
  --whitelist <characters>           Only recognize the provided characters.
                                     Example: "0123456789".
  --blacklist <characters>           Do not recognize the provided characters.
                                     Example: "0123456789".
  --clipboard                        Output OCR text to the clipboard.
  --trim-capture                     During OCR preprocessing, trim captured
                                     image to foreground pixels and add a thin
                                     border.
  --deskew                           During OCR preprocessing, attempt to
                                     compensate for slanted text.
  --scale-factor <factor>            Scale factor to use during pre-processing.
                                     Range: [0.71, 5.0]. Default is 3.5.
  --tess-config-file <file>          (Advanced) Path to Tesseract configuration
                                     file.
  --portable                         Store .ini settings file in same directory
                                     as the .exe file.
*/
bool CommandLine::process(QCoreApplication &app)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
                "Capture2Text may be used to OCR image files or part of the screen.\n"
                "Examples:\n"
                "  Capture2Text_CLI.exe --screen-rect \"400 200 600 300\"\n"
                "  Capture2Text_CLI.exe --vertical -l \"Chinese - Simplified\" -i img1.png\n"
                "  Capture2Text_CLI.exe -i img1.png -i img2.jpg -o result.txt\n"
                "  Capture2Text_CLI.exe -l Japanese -f \"C:\\Temp\\image_files.txt\"\n"
                "  Capture2Text_CLI.exe --show-languages");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption lineBreaksOption(QStringList() << "b" << "line-breaks",
                                        "Do not remove line breaks from OCR text.");
    parser.addOption(lineBreaksOption);

    QCommandLineOption debugOption(QStringList() << "d" << "debug",
                                   "Output captured image and pre-processed image for debugging purposes.");
    parser.addOption(debugOption);

    QCommandLineOption debugAppendTimestampOption("debug-timestamp",
                                                  "Append timestamp to debug images when using the -d option.");
    parser.addOption(debugAppendTimestampOption);

    QCommandLineOption imagesFileOption(QStringList() << "f" << "images-file",
                                        "File that contains paths of image files to OCR. One path per line.",
                                        "file");
    parser.addOption(imagesFileOption);

    QCommandLineOption imagesOption(QStringList() << "i" << "image",
                                    "Image file to OCR. You may OCR multiple image files like so: "
                                    "\"-i <img1> -i <img2> -i <img3>\"",
                                    "file");
    parser.addOption(imagesOption);

    QCommandLineOption langOption(QStringList() << "l" << "language",
                                  "OCR language to use. Case-sensitive. Default is \"English\". "
                                  "Use the --show-languages option to list installed OCR languages.",
                                  "language", "English");
    parser.addOption(langOption);

    QCommandLineOption outputFileOption(QStringList() << "o" << "output-file",
                                        "Output OCR text to this file. If not specified, stdout will be used.",
                                        "file");
    parser.addOption(outputFileOption);

    QCommandLineOption fileAppendOption("output-file-append",
                                        "Append to file when using the -o option.");
    parser.addOption(fileAppendOption);

    QCommandLineOption screenRectOption(QStringList() << "s" << "screen-rect",
                                        "Coordinates of rectangle that defines area of screen to OCR.",
                                        "\"x1 y1 x2 y2\"");
    parser.addOption(screenRectOption);

    QCommandLineOption verticalOption(QStringList() << "t" << "vertical",
                                      "OCR vertical text. If not specified, horizontal text is assumed.");
    parser.addOption(verticalOption);

    QCommandLineOption listLangOption(QStringList() << "w" << "show-languages",
                                      "Show installed languages that may be used with the \"--language\" option.");
    parser.addOption(listLangOption);

    QCommandLineOption outputFormatOption("output-format",
                                          "Format to use when outputting OCR text. You may use these tokens:\n"
                                          "${capture}   : OCR Text.\n"
                                          "${linebreak} : Line break (\\r\\n).\n"
                                          "${tab}       : Tab character.\n"
                                          "${timestamp} : Time that screen or each file was processed.\n"
                                          "${file}      : File that was processed or screen rect.\n"
                                          "Default format is \"${capture}${linebreak}\".",
                                          "format");
    parser.addOption(outputFormatOption);

    QCommandLineOption whitelistOption("whitelist",
                                       "Only recognize the provided characters. Example: \"0123456789\".",
                                       "characters");
    parser.addOption(whitelistOption);

    QCommandLineOption blacklistOption("blacklist",
                                       "Do not recognize the provided characters. Example: \"0123456789\".",
                                       "characters");
    parser.addOption(blacklistOption);

    QCommandLineOption clipboardOption(QStringList() << "clipboard",
                                      "Output OCR text to the clipboard.");
    parser.addOption(clipboardOption);

    QCommandLineOption preprocessTrimOption(QStringList() << "trim-capture",
                                            "During OCR preprocessing, trim captured image to foreground pixels and add a thin border.");
    parser.addOption(preprocessTrimOption);

    QCommandLineOption preprocessDeskewOption(QStringList() << "deskew",
                                            "During OCR preprocessing, attempt to compensate for slanted text.");
    parser.addOption(preprocessDeskewOption);

    QCommandLineOption scaleFactorOption(QStringList() << "scale-factor",
                                            "Scale factor to use during pre-processing. Range: [0.71, 5.0]. Default is 3.5.",
                                            "factor", "3.5");
    parser.addOption(scaleFactorOption);

    QCommandLineOption tessConfigFileOption("tess-config-file",
                                            "(Advanced) Path to Tesseract configuration file.",
                                            "file");
    parser.addOption(tessConfigFileOption);

#ifndef CLI_BUILD
    QCommandLineOption portableOption(QStringList() << "portable",
                                      "Store .ini settings file in same directory as the .exe file.");
    parser.addOption(portableOption);
#endif

    parser.process(app);

    if(app.arguments().length() <= 1)
    {
        parser.showHelp();
        return true;
    }

    if(parser.isSet(listLangOption))
    {
        showInstalledLanguages();
        return true;
    }

#ifndef CLI_BUILD
    portable = parser.isSet(portableOption);

    if(portable)
    {
        return true;
    }
#endif

    debug = parser.isSet(debugOption);
    debugAppendTimestamp = parser.isSet(debugAppendTimestampOption);

    QTextStream errStream(stderr);

    QStringList imagePaths = parser.values(imagesOption);
    QString screenRectStr = parser.value(screenRectOption);
    QString imagesFile = parser.value(imagesFileOption).trimmed();

    if(imagePaths.size() == 0
            && screenRectStr.size() == 0
            && imagesFile.size() == 0)
    {
        errStream << "At least one of the following options must be specified:" << endl
                  << "  -i, --image" << endl
                  << "  -f, --images-file" << endl
                  << "  -s, --screen-rect" << endl;
        return false;
    }

    captureTimestamp = QDateTime::currentDateTime();

    bool verticalOrientation = parser.isSet(verticalOption);
    QString whitelist = parser.value(whitelistOption);
    QString blacklist = parser.value(blacklistOption);
    QString tessConfigFile = parser.value(tessConfigFileOption);
    QString lang = parser.value(langOption);

    imagePreprocessor.setVerticalOrientation(verticalOrientation);
    imagePreprocessor.setRemoveFurigana(UtilsLang::languageSupportsFurigana(lang));

    ocrEngine->setVerticalOrientation(verticalOrientation);
    ocrEngine->setWhitelist(whitelist);
    ocrEngine->setBlacklist(blacklist);
    ocrEngine->setConfigFile(tessConfigFile);

    if(!ocrEngine->setLang(lang))
    {
        errStream << "Error, specified OCR language not found." << endl;
        showInstalledLanguages();
        return false;
    }

    QString outputFormatStr = parser.value(outputFormatOption);

    if(outputFormatStr.length() > 0)
    {
        outputFormat = outputFormatStr;
    }

    keepLineBreaks = parser.isSet(lineBreaksOption);
    copyToClipboard = parser.isSet(clipboardOption);
    preprocessTrim = parser.isSet(preprocessTrimOption);
    preprocessDeskew = parser.isSet(preprocessDeskewOption);
    bool scaleFactorOk = true;
    double scaleFactor = parser.value(scaleFactorOption).toDouble(&scaleFactorOk);

    if(!scaleFactorOk)
    {
        scaleFactor = 3.5;
    }

    imagePreprocessor.setScaleFactor(scaleFactor);

    outputFilePath = parser.value(outputFileOption);
        bool outputFileAppend = parser.isSet(fileAppendOption);

    // If output file specified, open/create it here
    if(outputFilePath.size() > 0)
    {
        QIODevice::OpenMode openMode = QIODevice::WriteOnly | QIODevice::Text;

        if(outputFileAppend)
        {
            openMode |= QIODevice::Append;
        }

        outputFile.setFileName(outputFilePath);
        if(!outputFile.open(openMode))
        {
            QTextStream(stderr) << "Error, unable to create output file:" << endl
                                << "\"" << outputFilePath << "\"" << endl;
            return false;
        }
    }

    if(imagePaths.size() != 0)
    {
        ocrImageFiles(imagePaths);
    }
    else if(screenRectStr.size() != 0)
    {
        QRect rect;
        if(!convertStringToRect(screenRectStr, rect))
        {
            return false;
        }

        currentImageFile = "(" + screenRectStr + ")";
        ocrScreenRectAndOutput(rect);
    }
    else if(imagesFile.size() != 0)
    {
        ocrFileOfImages(imagesFile);
    }

    if(outputFile.isOpen())
    {
        outputFile.close();
    }

    if(copyToClipboard)
    {
        QGuiApplication::clipboard()->setText(allOcrText);
    }

    return true;
}

QString CommandLine::postProcessText(QString ocrText)
{
    PostProcess postProcess(ocrEngine->getLang(), keepLineBreaks);
    return postProcess.postProcessOcrText(ocrText);
}

void CommandLine::ocrScreenRectAndOutput(QRect rect)
{
    QString ocrText = ocrScreenRect(rect);
    ocrText = postProcessText(ocrText);
    outputOcrText(ocrText);
}

QString CommandLine::ocrScreenRect(QRect rect)
{
    QImage img = UtilsImg::takeScreenshot(rect);

    if(img.width() == 0 || img.height() == 0)
    {
        QTextStream(stderr) << "Error, screenshot failure." << endl;
        return QString("<Error>");
    }

    if(debug)
    {
        img.save(getDebugImagePath("debug_capture.png"));
    }

    PIX *inPixs = imagePreprocessor.convertImageToPix(img);
    PIX *pixs = imagePreprocessor.processImage(inPixs, preprocessDeskew, preprocessTrim);
    pixDestroy(&inPixs);

    if(pixs == nullptr)
    {
        QTextStream(stderr) << "Error, pre-processing failure." << endl;
        return QString("<Error>");
    }

    if(debug)
    {
        QString savePath = getDebugImagePath("debug_enhanced.png");
        QByteArray byteArray = savePath.toLocal8Bit();
        pixWriteImpliedFormat(byteArray.constData(), pixs, 0, 0);
    }

    bool singleLine = false;

    if(UtilsLang::languageSupportsFurigana(ocrEngine->getLang()))
    {
        singleLine = (imagePreprocessor.getJapNumTextLines() == 1);
    }

    QString ocrText = ocrEngine->performOcr(pixs, singleLine);

    pixDestroy(&pixs);

    if(ocrText.size() == 0)
    {
        QTextStream(stderr) << "Error, OCR failure." << endl;
        return QString("<Error>");
    }

    return ocrText;
}

bool CommandLine::convertStringToRect(QString str, QRect &rect)
{
    QStringList fields = str.split(" ", QString::SkipEmptyParts);

    if(fields.size() != 4)
    {
        QTextStream(stderr) << "Error, need to specify 4 values for rectangle:" << endl
                            << "\"" << str << "\"" << endl;
        return false;
    }

    bool status = false;
    int coords[4];

    for(int i = 0; i < 4; i++)
    {
        coords[i] = fields[i].toInt(&status);

        if(!status)
        {
            QTextStream(stderr) << "Error, rectangle value must be an integer:" << endl
                                << "\"" << fields[i] << "\"" << endl;
            return false;
        }
    }

    rect.setCoords(coords[0], coords[1], coords[2], coords[3]);

    return true;
}

void CommandLine::ocrFileOfImages(QString imagesFile)
{
    if(!QFile::exists(imagesFile))
    {
        QTextStream(stderr) << "Error, file does not exist:" << endl
                            << "\"" << imagesFile << "\"" << endl;
        return;
    }

    QFile file(imagesFile);

    if(!file.open(QIODevice::ReadOnly))
    {
        QTextStream(stderr) << "Error, could not open file:" << endl
                            << "\"" << imagesFile << "\"" << endl;
        return;
    }

    QTextStream in(&file);
    QStringList imgPaths;

    while(!in.atEnd())
    {
        QString imgPath = in.readLine().trimmed();

        if(imgPath.size() > 0)
        {
            imgPaths.append(imgPath);
        }
    }

    file.close();

    ocrImageFiles(imgPaths);
}

void CommandLine::ocrImageFiles(QStringList &imgList)
{
    for(auto img : imgList)
    {
        ocrImageFileAndOutput(img);
    }
}

void CommandLine::ocrImageFileAndOutput(QString img)
{
    currentImageFile = img;
    captureTimestamp = QDateTime::currentDateTime();
    QString ocrText = ocrImageFile(img);
    ocrText = postProcessText(ocrText);
    outputOcrText(ocrText);
}

QString CommandLine::ocrImageFile(QString img)
{
    if(!QFile::exists(img))
    {
        QTextStream(stderr) << "Error, file does not exist:" << endl
                            << "\"" << img << "\"" << endl;
        return QString("<Error>");
    }

    PIX *inPixs = imagePreprocessor.convertImageToPix(img);
    PIX *pixs = imagePreprocessor.processImage(inPixs, preprocessDeskew, preprocessTrim);
    pixDestroy(&inPixs);

    if(pixs == nullptr)
    {
        QTextStream(stderr) << "Error, pre-processing failure:" << endl
                            << "\"" << img << "\"" << endl;
        return QString("<Error>");
    }

    if(debug)
    {
        QString savePath = getDebugImagePath("debug_enhanced.png");
        QByteArray byteArray = savePath.toLocal8Bit();
        pixWriteImpliedFormat(byteArray.constData(), pixs, 0, 0);
    }

    bool singleLine = false;

    if(UtilsLang::languageSupportsFurigana(ocrEngine->getLang()))
    {
        singleLine = (imagePreprocessor.getJapNumTextLines() == 1);
    }

    QString ocrText = ocrEngine->performOcr(pixs, singleLine);
    pixDestroy(&pixs);

    if(ocrText.size() == 0)
    {
        QTextStream(stderr) << "Error, OCR failure:" << endl
                            << "\"" << img << "\"" << endl;
        return QString("<Error>");
    }

    return ocrText;
}

void CommandLine::outputOcrText(QString ocrText)
{
    QString formattedOcrText = UtilsCommon::formatLogLine(outputFormat, ocrText, captureTimestamp, "", currentImageFile);

    if(outputFilePath.size() > 0)
    {
        outputToFile(formattedOcrText);
    }

    outputToConsole(formattedOcrText);

    if(copyToClipboard)
    {
        allOcrText.append(formattedOcrText);
    }
}

void CommandLine::outputToFile(QString ocrText)
{
    QTextStream stream(&outputFile);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(true);
    stream << ocrText;
    stream.flush();
}

void CommandLine::outputToConsole(QString ocrText)
{
    QTextStream stream(stdout);
    stream.setCodec("UTF-8");
    stream << ocrText;
}

void CommandLine::showInstalledLanguages()
{
    QStringList installedLangs = ocrEngine->getInstalledLangs();
    QTextStream outStream(stdout);

    outStream << "Installed OCR Languages:" << endl;

    for(auto item : installedLangs)
    {
        outStream << item << endl;
    }
}

QString CommandLine::getDebugImagePath(QString filename)
{
    return UtilsImg::getDebugScreenshotPath(filename, debugAppendTimestamp, captureTimestamp);
}




