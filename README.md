# Capture2Text
Linux CLI port of Capture2Text v4.6.2

sync from:https://sourceforge.net/projects/capture2text/

The OCR results from Capture2Text were generally better than standard Tesseract, so it seemed ideal to make this run on Linux. It turns out the console version can actually run without anything more than build changes.

## Dependencies
Tesseract, Qt 5 (cannot be version 4), Leptonica

Ubuntu

`sudo apt-get install -y libtesseract-dev qtbase5-dev qt5-default libleptonica-dev libqt5texttospeech5-dev qtspeech5-speechd-plugin libspeechd-dev`

Centos

`sudo yum install -y tesseract-devel qt5-qtbase qt5-default leptonica-devel qt5-qtspeech-speechd`

## Build instructions

### Windows:

```
pip install conan
conan install .
```
then use qtcreator to build the project.
after build.
copy `lib/*.dll` to the same folder as Capture2Text.exe file.
in the same Capture2Text.exe folder ,creat a folder called `tessdata` and put the trained data into it.


### Ubuntu

```
QT_SELECT=5 qmake Capture2Text/Capture2Text.pro -d
make
wget "https://github.com/GSam/Capture2Text/releases/download/Prototype/English.zip"
unzip -o "English.zip" -d "tessdata"
rm "English.zip"
```

### Centos:

```
QT_SELECT=5 qmake-qt5 Capture2Text/Capture2Text.pro -d
make
wget "https://github.com/GSam/Capture2Text/releases/download/Prototype/English.zip"
unzip -o "English.zip" -d "tessdata"
rm "English.zip"
```
### known issues:
* can not capture screen on wayland.https://github.com/MaartenBaert/ssr/issues/431
* Compile with Visual Studio on Windows ,should set runtime library to /MD but not /MDd

### only build cli verssion

```
qmake Capture2Text/Capture2Text.pro -d CONFIG+=console
make
```
