#include "audiocontroller.h"

#include <psapi.h>
#include <comdef.h>
#include <functiondiscoverykeys_devpkey.h>
#include <stdio.h>

#define CHECK_HR(hr, msg) \
    if (FAILED(hr)) { \
        printf("%s failed: %lu\n", msg, GetLastError()); \
        return; \
    }

__CRT_UUID_DECL(IAudioMeterInformation, 0xC02216F6, 0x8C67, 0x4B5B, 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64);

MIDL_INTERFACE("C02216F6-8C67-4B5B-9D00-D008E73E0064")
IAudioMeterInformation : public IUnknown
{
public:
    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPeakValue(
        /* [out] */ float *pfPeak) = 0;

    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(
        /* [out] */
        UINT *pnChannelCount) = 0;

    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(
        /* [in] */ UINT32 u32ChannelCount,
        /* [size_is][out] */ float *afPeakValues) = 0;

    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE QueryHardwareSupport(
        /* [out] */
        DWORD *pdwHardwareSupportMask) = 0;
};

AudioController::AudioController() {
    adjustedVolume = 0.5f;
    activationThreshold = 0.05f;
}

string AudioController::GetProcessPath(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

    string path;
    char buffer[1024];
    DWORD bufferSize = sizeof(buffer);

    if (hProcess) {
        if (QueryFullProcessImageNameA(hProcess, 0, buffer, &bufferSize)) {
            path = buffer;
        } else {
            printf("QueryFullProcessImageNameA failed: %lu\n", GetLastError());
        }

        CloseHandle(hProcess);
    } else {
        printf("OpenProcess failed: %lu\n", GetLastError());
    }

    return path;
}

string AudioController::GetProcessName(string processPath) {
    size_t pos = processPath.find_last_of('\\');
    return (pos != string::npos) ? processPath.substr(pos + 1) : "";
}

void AudioController::AdjustAppVolume(float targetVolume, IAudioSessionEnumerator* pSessionEnumerator) {
    int sessionCount;
    HRESULT hr = pSessionEnumerator->GetCount(&sessionCount);
    CHECK_HR(hr, "IAudioSessionEnumerator::GetCount");

    for (int i = 0; i < sessionCount; i++) {
        IAudioSessionControl* pSessionControl = nullptr;
        hr = pSessionEnumerator->GetSession(i, &pSessionControl);
        CHECK_HR(hr, "IAudioSessionEnumerator::GetSession");

        IAudioSessionControl2* pSessionControl2 = nullptr;
        hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
        if (FAILED(hr)) {
            pSessionControl->Release();
            continue;
        }

        DWORD processId;
        hr = pSessionControl2->GetProcessId(&processId);
        if (FAILED(hr)) {
            pSessionControl->Release();
            continue;
        }

        string processPath = GetProcessPath(processId);
        string processName = GetProcessName(processPath);

        if (processName == "Spotify.exe") {
            ISimpleAudioVolume* pVolumeControl = nullptr;
            hr = pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pVolumeControl);
            if (FAILED(hr)) {
                pSessionControl2->Release();
                pSessionControl->Release();
                continue;
            }

            printf("Adjusting Spotify volume to %f\n", targetVolume);
            pVolumeControl->SetMasterVolume(targetVolume, nullptr);
            pVolumeControl->Release();
        }

        pSessionControl2->Release();
        pSessionControl->Release();
    }
}

bool AudioController::IsDiscordActive(IAudioSessionEnumerator* pSessionEnumerator) {
    int sessionCount;
    HRESULT hr = pSessionEnumerator->GetCount(&sessionCount);
    //CHECK_HR(hr, "IAudioSessionEnumerator::GetCount");

    for (int i = 0; i < sessionCount; i++) {
        IAudioSessionControl* pSessionControl = nullptr;
        hr = pSessionEnumerator->GetSession(i, &pSessionControl);
        //CHECK_HR(hr, "IAudioSessionEnumerator::GetSession");

        IAudioSessionControl2* pSessionControl2 = nullptr;
        hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
        if (FAILED(hr)) {
            pSessionControl->Release();
            continue;
        }

        DWORD processId;
        hr = pSessionControl2->GetProcessId(&processId);
        if (FAILED(hr)) {
            pSessionControl->Release();
            continue;
        }

        string processPath = GetProcessPath(processId);
        string processName = GetProcessName(processPath);

        if (processName == "Discord.exe") {
            IAudioMeterInformation* pMeterInfo = nullptr;
            hr = pSessionControl->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&pMeterInfo);
            if (FAILED(hr)) {
                pSessionControl2->Release();
                pSessionControl->Release();
                continue;
            }

            float peakLevel = 0.0f;
            pMeterInfo->GetPeakValue(&peakLevel);
            printf("Discord Peak Audio Level: %f\n", peakLevel);
            pMeterInfo->Release();

            if (peakLevel > activationThreshold) {
                pSessionControl2->Release();
                pSessionControl->Release();
                return true;
            }
        }

        pSessionControl2->Release();
        pSessionControl->Release();
    }

    return false;
}

void AudioController::CheckAndAdjustAppVolume() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    CHECK_HR(hr, "CoInitializeEx");

    IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
    CHECK_HR(hr, "CoCreateInstance");

    IMMDevice* pDevice = nullptr;
    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    CHECK_HR(hr, "IMMDeviceEnumerator::GetDefaultAudioEndpoint");

    IAudioSessionManager2* pSessionManager = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&pSessionManager);
    CHECK_HR(hr, "IMMDevice::Activate");

    IAudioSessionEnumerator* pSessionEnumerator = nullptr;
    hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
    CHECK_HR(hr, "IAudioSessionManager2::GetSessionEnumerator");

    if (IsDiscordActive(pSessionEnumerator)) {
        AdjustAppVolume(adjustedVolume, pSessionEnumerator);
    } else {
        AdjustAppVolume(1.0f, pSessionEnumerator);
    }

    pSessionEnumerator->Release();
    pSessionManager->Release();
    pDevice->Release();
    pDeviceEnumerator->Release();
    CoUninitialize();
}
