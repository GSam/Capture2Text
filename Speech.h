#ifndef SPEECH_H
#define SPEECH_H

#include <QTextToSpeech>

class Speech
{
public:
    static Speech& getInstance()
    {
        static Speech instance;
        return instance;
    }

    static bool isSpeechEnabledForCurrentLang();

    void sayText(QString text);
    void sayText(QString text, QString locale, QString selectedVoice, int volume, int rate, int pitch);

    ~Speech()
    {
        delete speech;
    }

private:
    Speech()
    {
        speech = new QTextToSpeech(nullptr);
    }

    QTextToSpeech *speech;
};

#endif // SPEECH_H
