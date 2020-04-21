// Copyright (C) Microsoft Corporation. All rights reserved.
// 
// WMCConsole_winrtcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "apphost.h"

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <Mferror.h>
#include <winrt\base.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Media.Capture.h>
#include <winrt\Windows.Media.Core.h>
#include <winrt\Windows.Media.Devices.h>
#include <winrt\Windows.Media.MediaProperties.h>
#include <winrt\Windows.Media.Capture.Frames.h>

#include <winrt\Windows.Devices.Enumeration.h>
#include <winrt\Windows.Graphics.Imaging.h>
#include <winrt\Windows.Storage.h>
#include <winrt\Windows.Storage.Streams.h>



using namespace winrt;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Storage;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Core;



class CameraHelper
{

    // We need to hold the reference to sourcegroups. The sourceinfos are holding weak references to this.
    IVectorView<MediaFrameSourceGroup> m_SourceGroups;

    // This can be used as a mock preview
    MediaFrameReader m_PreviewFrameReader;

    //
    // Iterates through source groups and filter-out the frame sources which have IR, depth and other sources which we cannot consume in this app
    //
    IVector<MediaFrameSourceInfo> GetFilteredSourceGroupList()
    {
        auto filteredSourceInfos = single_threaded_vector<MediaFrameSourceInfo>();
        m_SourceGroups = MediaFrameSourceGroup::FindAllAsync().get();
        auto sourceGroupIter = m_SourceGroups.First();
        while (sourceGroupIter.HasCurrent())
        {
            auto sourceInfos = sourceGroupIter.Current().SourceInfos();
            auto sourceInfoIter = sourceInfos.First();
            // Iterate through sources and filter-out the IR, depth and other sources which we cannot consume in this app
            while (sourceInfoIter.HasCurrent())
            {
                if ((sourceInfoIter.Current().MediaStreamType() == MediaStreamType::Photo
                    || sourceInfoIter.Current().MediaStreamType() == MediaStreamType::VideoPreview
                    || sourceInfoIter.Current().MediaStreamType() == MediaStreamType::VideoRecord)
                    && sourceInfoIter.Current().SourceKind() == MediaFrameSourceKind::Color)
                {
                    filteredSourceInfos.Append(sourceInfoIter.Current());
                }
                sourceInfoIter.MoveNext();
            }
            sourceGroupIter.MoveNext();
        }
        return filteredSourceInfos;
    }

    //
    // Prints information of all sources from the list in input argument 'filteredSources' and returns user selected index of the source to use
    //
    int GetSGSelection(IVector<MediaFrameSourceInfo> filteredSources)
    {
        unsigned int idx = 0;
        auto source = filteredSources.First();
        while (source.HasCurrent())
        {
            idx++;
            auto currSource = source.Current();

            // Add new entries to the following map initializations if new enum entries are added in the future
            std::map<MediaStreamType, std::wstring> streamTypes =
            {
                {MediaStreamType::VideoPreview, L"VideoPreview"},
                {MediaStreamType::VideoRecord,  L"VideoRecord" },
                {MediaStreamType::Audio,        L"Audio"       },
                {MediaStreamType::Photo,        L"Photo"       }
            };

            std::map<Panel, std::wstring>  panelTypes =
            {
                {Panel::Unknown, L"Unknown"},
                {Panel::Front,   L"Front"  },
                {Panel::Back,    L"Back"   },
                {Panel::Top,     L"Top"    },
                {Panel::Bottom,  L"Bottom" },
                {Panel::Left,    L"Left"   },
                {Panel::Right,   L"Right"  }
            };
            auto enclosureLocation = currSource.DeviceInformation().EnclosureLocation();
            std::wstring panelLocation;
            if (enclosureLocation)
            {
                panelLocation = L"\\" + panelTypes[enclosureLocation.Panel()];
            }

            std::wcout << idx << L":" << currSource.SourceGroup().DisplayName().c_str() << "-->" << currSource.DeviceInformation().Name().c_str() << panelLocation.c_str() << ":" << streamTypes[currSource.MediaStreamType()].c_str() << std::endl;
            source.MoveNext();
        }
        do
        {
            std::wcout << L"Enter selection:";
            std::cin >> idx;
        } while ((idx > filteredSources.Size()) || (idx < 1));
        return idx - 1; // Because selection idx is from 1 to N and indexes for from 0 to N-1
    }

    //
    // Prints information of all available Mediatypes for the selected source and returns user selected index of the MediaType to use
    //
    int GetMediaTypeSelection(MediaFrameSourceInfo selectedSource)
    {
        auto mediaDescriptionIter = selectedSource.VideoProfileMediaDescription().First();
        int idx = 0;

        while (mediaDescriptionIter.HasCurrent())
        {

            auto format = mediaDescriptionIter.Current();
            int frameRateInt = (int)format.FrameRate();
            int frameRateDec = ((int)(format.FrameRate() * 100) % 100);
            idx++;
            auto formatStr = std::to_wstring(idx) + L". " + std::wstring(format.Subtype().c_str()) + L": " +
                std::to_wstring(format.Width()) + L"x" + std::to_wstring(format.Height()) + L" @ " + std::to_wstring(frameRateInt) + L"." + std::to_wstring(frameRateDec) + L" fps";
            std::wcout << formatStr << std::endl;
            mediaDescriptionIter.MoveNext();
        }
        int selection;
        do
        {
            std::wcout << L"Enter selection:";
            std::cin >> selection;
        } while ((selection > idx) || (selection < 1));
        return selection - 1; // Because selection idx is from 1 to N and indexes for from 0 to N-1
    }

    //
    // Create a Frame reader for Mock preview and drain samples from preview/record pin for auto-exposure conververgence
    //
    void StartMockPreview(MediaCapture mediaCapture)
    {
        // start Preview to get the 3A running and wait for convergence.
        MediaFrameSource previewframeSource(nullptr);
        MediaStreamType streamTypelookup = MediaStreamType::VideoPreview;

        // Try to find the suitable pin where 3A will be running.
        // Start by looking for a preview pin , if not found look for record pin 
        // However exit the loop when preview and record pins are not available as just running the photo pin cannot converge 3A
        while ((previewframeSource == nullptr) && (streamTypelookup != MediaStreamType::Photo))
        {
            auto frameSourceIter = mediaCapture.FrameSources().First();
            // If there is no preview pin, find a record pin
            while (frameSourceIter.HasCurrent())
            {
                auto frameSource = frameSourceIter.Current().Value();
                if (frameSource.Info().MediaStreamType() == streamTypelookup)
                {
                    previewframeSource = frameSource;
                    break;
                }
                frameSourceIter.MoveNext();
            }
            streamTypelookup = (streamTypelookup == MediaStreamType::VideoPreview) ? MediaStreamType::VideoRecord : MediaStreamType::Photo;
        }

        m_PreviewFrameReader = mediaCapture.CreateFrameReaderAsync(previewframeSource).get();
        m_PreviewFrameReader.AcquisitionMode(MediaFrameReaderAcquisitionMode::Realtime);
        m_PreviewFrameReader.StartAsync().get();
        std::cout << "\nDraining samples for one second from the pipeline to enable 3A convergence\n";
        Sleep(1000);
    }

public:
    //
    // Constructs the CameraHelper class and initializes the members
    //
    CameraHelper() :m_PreviewFrameReader(nullptr), m_SourceGroups(nullptr)
    {

    }

    //
    // Initialized MediaCapture with appropriate settings, frame source and mediatype selection
    //
    MediaCapture InitCamera()
    {
        MediaCapture mediaCapture;
        auto settings = MediaCaptureInitializationSettings();
        auto filteredGroups = GetFilteredSourceGroupList();

        if (filteredGroups.Size() < 1)
        {
            std::cout << "Error No suitable capture sources found";
            throw_hresult(MF_E_NO_CAPTURE_DEVICES_AVAILABLE);
        }

        auto selectedSGidx = GetSGSelection(filteredGroups);

        auto selectedSrc = filteredGroups.GetAt(selectedSGidx);

        if (selectedSrc == nullptr)
        {
            throw_hresult(MF_E_OUT_OF_RANGE);
        }

        settings.SourceGroup(selectedSrc.SourceGroup());

        // If user explicitly selected a non-photo pin to take the photo
        if (selectedSrc.MediaStreamType() != MediaStreamType::Photo)
        {
            settings.PhotoCaptureSource(PhotoCaptureSource::VideoPreview);
        }
        else
        {
            // We hope that auto will select the best photo pin options
            settings.PhotoCaptureSource(PhotoCaptureSource::Auto);
        }

        settings.StreamingCaptureMode(StreamingCaptureMode::Video);

        // Set format on the medicapture frame source
        auto formatIdx = GetMediaTypeSelection(selectedSrc);

        mediaCapture.InitializeAsync(settings).get();
        auto frameSourceIter = mediaCapture.FrameSources().First();

        auto frameSource = mediaCapture.FrameSources().Lookup(selectedSrc.Id());
        frameSource.SetFormatAsync(frameSource.SupportedFormats().GetAt(formatIdx)).get();

        if (mediaCapture.VideoDeviceController().ExposureControl().Supported())
        {
            std::cout << (mediaCapture.VideoDeviceController().ExposureControl().Auto() ? "Exposure Control AutoMode already Set" : "Exposure Control AutoMode not Set..setting it now to true");
            mediaCapture.VideoDeviceController().ExposureControl().SetAutoAsync(true);
        }
        else if (mediaCapture.VideoDeviceController().Exposure().Capabilities().Supported())
        {
            if (mediaCapture.VideoDeviceController().Exposure().Capabilities().AutoModeSupported())
            {
                bool autoexp = false;
                mediaCapture.VideoDeviceController().Exposure().TryGetAuto(autoexp);
                std::cout << (autoexp ? "Exposure AutoMode already Set" : "Exposure AutoMode not Set..setting it now to true");
                mediaCapture.VideoDeviceController().Exposure().TrySetAuto(true);
            }
            else
            {
                std::cout << "Exposure AutoMode not supported";
            }
        }

        StartMockPreview(mediaCapture);

        return mediaCapture;
    }

    //
    // Stops and closes frame reader, closes Mediacapture 
    //
    void DeInitCamera(MediaCapture& mediaCapture)
    {
        m_SourceGroups = nullptr;
        m_PreviewFrameReader.StopAsync().get();
        m_PreviewFrameReader.Close();
        m_PreviewFrameReader = nullptr;
        mediaCapture.Close();
        mediaCapture = nullptr;
    }
};

//
// Takes two photos and processes them using low light fusion API. Saves all photos to files
//
void TakePhotosAndProcess(MediaCapture mediaCapture, uint32_t photoIndex)
{
    WCHAR ExePath[MAX_PATH] = { 0 };
    if (GetModuleFileName(NULL, ExePath, _countof(ExePath)) == 0)
    {
        std::cout << "\nError getting the path to executable, defaulting output folder to C:\\test";
        wcscpy_s(ExePath, L"C:\\");
    }

    auto file = Windows::Storage::StorageFile::GetFileFromPathAsync(ExePath).get();
    auto folderRoot = file.GetParentAsync().get();
    auto folder = folderRoot.CreateFolderAsync(L"test\\", CreationCollisionOption::OpenIfExists).get();
    auto file1 = folder.CreateFileAsync(to_hstring(photoIndex) + L"_1.png", CreationCollisionOption::GenerateUniqueName).get();
    auto file2 = folder.CreateFileAsync(to_hstring(photoIndex) + L"_2.png", CreationCollisionOption::GenerateUniqueName).get();
    std::wcout << L"\nCapturing two photos:\n" << file1.Path().c_str() << L"\n" << file2.Path().c_str();
    try
    {
        // Capture and save two photos
        mediaCapture.CapturePhotoToStorageFileAsync(ImageEncodingProperties::CreatePng(), file1).get();
        mediaCapture.CapturePhotoToStorageFileAsync(ImageEncodingProperties::CreatePng(), file2).get();
/*
        auto sbList = single_threaded_vector<SoftwareBitmap>();
        // Open photos and decode them into NV12 format for processing
        // Photo1
        {
            auto strm = file1.OpenReadAsync().get();
            auto decoder = BitmapDecoder::CreateAsync(strm).get();
            auto sb1 = decoder.GetSoftwareBitmapAsync().get();
            sbList.Append(SoftwareBitmap::Convert(sb1, BitmapPixelFormat::Nv12));
        }
        // Photo2
        {
            auto strm = file2.OpenReadAsync().get();
            auto decoder = BitmapDecoder::CreateAsync(strm).get();
            auto sb1 = decoder.GetSoftwareBitmapAsync().get();
            sbList.Append(SoftwareBitmap::Convert(sb1, BitmapPixelFormat::Nv12));
        }
*/
/*
        // Do some processing like low light fusion in this example
        auto op = LowLightFusion::FuseAsync(sbList).get();

        // Save the output
        auto fname = to_hstring(photoIndex) + L"_LLF.png";
        auto fileOp = folder.CreateFileAsync(fname, CreationCollisionOption::GenerateUniqueName).get();
        {
            auto strm = fileOp.OpenAsync(FileAccessMode::ReadWrite).get();
            auto encoder = BitmapEncoder::CreateAsync(BitmapEncoder::PngEncoderId(), strm).get();
            encoder.SetSoftwareBitmap(SoftwareBitmap::Convert(op.Frame(), BitmapPixelFormat::Bgra8));
            encoder.FlushAsync().get();
            std::wcout << L"\nSaved LowLightFusion output to:\n " << fileOp.Path().c_str();
        }
*/
    }
    catch (hresult_error const& ex)
    {
        std::cout << "\nError during photo capture and process";
        throw(ex);
    }
}

CameraHelper cameraHelper;
auto mediaCapture = cameraHelper.InitCamera();
char key = 0;
uint32_t photoCounter = 0;


void draw()
{
    std::cout << "\nTaking photos and processing: " << ++photoCounter;
    TakePhotosAndProcess(mediaCapture, photoCounter);
}


//
// Main entry point of the application
//
void setup()
{
    std::cout << "Windows Media Capture in Win32 desktop Console app!";
    try
    {
        //CameraHelper cameraHelper;
        //auto mediaCapture = cameraHelper.InitCamera();

        // Main processing part of photos
        //char key = 0;
        //uint32_t photoCounter = 0;


        //cameraHelper.DeInitCamera(mediaCapture);
    }
    catch (hresult_error const& ex)
    {
        std::wcout << L"\nError: " << std::hex << ex.code() << L":" << ex.message().c_str();
    }

    setFrameRate(5);
}
