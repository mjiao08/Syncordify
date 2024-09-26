#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <string>
#include <iomanip>

#include <QSettings>

using std::string;

class AudioController {
public:
    AudioController() = default;
    ~AudioController();

    void CheckAndAdjustAppVolume();

    float adjustedVolume;
    float activationThreshold;

private:
    string GetProcessPath(DWORD processId);
    string GetProcessName(string processPath);
    void AdjustAppVolume(float targetVolume, IAudioSessionEnumerator* pSessionEnumerator);
    bool IsDiscordActive(IAudioSessionEnumerator* pSessionEnumerator);
};

#endif // AUDIOCONTROLLER_H
