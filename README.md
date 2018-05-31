# Capture2Text
Linux CLI port of Capture2Text v4.5.1

The OCR results from Capture2Text were generally better than standard Tesseract, so it seemed ideal to make this run on Linux. It turns out the console version can actually run without anything more than build changes.

## TODO
* Make the build file more generic, so as to be able to build on Windows still
* Add some additional instructions on packages to install and how to build
* Figure out how to avoid `-platform offscreen` for the command line (over SSH)
