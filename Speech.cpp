#include <QDebug>
#include "Speech.h"
#include "Settings.h"

bool Speech::isSpeechEnabledForCurrentLang()
{
    if(!Settings::getSpeechEnable())
    {
        return false;
    }

    QString locale;
    QString selectedVoice;
    int rate;
    int pitch;

    Settings::getSpeechInfo(Settings::getOcrLang(), locale, selectedVoice, rate, pitch);

    if(selectedVoice == "<Disabled>")
    {
        return false;
    }

    return true;
}


void Speech::sayText(QString text)
{
    speech->stop();

    if(!Settings::getSpeechEnable())
    {
        return;
    }

    QString locale;
    QString selectedVoice;
    int rate;
    int pitch;

    Settings::getSpeechInfo(Settings::getOcrLang(), locale, selectedVoice, rate, pitch);

    if(selectedVoice == "<Disabled>")
    {
        return;
    }

    speech->setLocale(locale);

    auto voiceList = speech->availableVoices();
    foreach (const QVoice &voice, voiceList)
    {
       if(voice.name() == selectedVoice)
       {
           speech->setVoice(voice);
       }
    }

    speech->setVolume(Settings::getSpeechVolume() / 100.0);
    speech->setRate(rate / 10.0);
    speech->setPitch(pitch / 10.0);

    speech->say(text);
}


void Speech::sayText(QString text, QString locale, QString selectedVoice, int volume, int rate, int pitch)
{
    speech->stop();

    if(selectedVoice == "<Disabled>")
    {
        return;
    }

    speech->setLocale(locale);

    auto voiceList = speech->availableVoices();
    foreach (const QVoice &voice, voiceList)
    {
       if(voice.name() == selectedVoice)
       {
           speech->setVoice(voice);
       }
    }

    speech->setVolume(volume / 100.0);
    speech->setRate(rate / 10.0);
    speech->setPitch(pitch / 10.0);

    speech->say(text);
}
