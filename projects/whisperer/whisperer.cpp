#include "apphost.h"

#include <mmeapi.h>

#include "whisper/whisper.h"


struct whisper_context* ctx = nullptr;


WAVEFORMATEX wfx{};
HWAVEIN hWaveIn;

auto rgb = [](int r, int g, int b) { return maths::vec4b{ (uint8_t)b, (uint8_t)g, (uint8_t)r, 255 }; };

// setup wav audio input
bool setupAudio()
{
	// https://cplusplus.com/forum/general/152940/
	auto nDevices = waveInGetNumDevs();
	if (nDevices == 0)
	{
		printf("No audio input devices found\n");
		return false;
	}
	


	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;

	// calculated fields
	wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	wfx.cbSize = 0;
	
	// Open the first available input device
	auto res = waveInOpen(&hWaveIn,
		WAVE_MAPPER,
		&wfx,
		NULL, NULL,
		CALLBACK_NULL | WAVE_FORMAT_DIRECT
	);

	if (res != MMSYSERR_NOERROR)
	{
		printf("Error opening input device!\n");
		return false;
	}
	
	// At this point, we have our device, now we need to give it buffers (with headers) that it can
	//  put the recorded audio somewhere
	char buffers[2][44100 * 2 * 2 / 2];    // 2 buffers, each half of a second long
	WAVEHDR headers[2] = { {},{} };           // initialize them to zeros
	for (int i = 0; i < 2; ++i)
	{
		headers[i].lpData = buffers[i];             // give it a pointer to our buffer
		headers[i].dwBufferLength = 44100 * 2 * 2 / 2;      // tell it the size of that buffer in bytes
		// the other parts of the header we don't really care about for this example, and can be left at zero

		// Prepare each header
		waveInPrepareHeader(hWaveIn, &headers[i], sizeof(headers[i]));

		// And add it to the queue
		//  Once we start recording, queued buffers will get filled with audio data
		waveInAddBuffer(hWaveIn, &headers[i], sizeof(headers[i]));
	}

	// In this example, I'm just going to dump the audio data to a binary file
	//std::ofstream outfile("my_recorded_audio.bin", std::ios_base::out | std::ios_base::binary);

	// Print some simple directions to the user
	//std::cout << "Now recording audio.  Press Escape to stop and exit." << std::endl;

	// start recording!
	waveInStart(hWaveIn);

	// Now that we are recording, keep polling our buffers to see if they have been filled.
	//   If they have been, dump their contents to the file and re-add them to the queue so they
	//   can get filled again, and again, and again
	while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))  // keep looping until the user hits escape
	{
		for (auto& h : headers)      // check each header
		{
			if (h.dwFlags & WHDR_DONE)           // is this header done?
			{
				// if yes, dump it to our file
				//outfile.write(h.lpData, h.dwBufferLength);

				// then re-add it to the queue
				h.dwFlags = 0;          // clear the 'done' flag
				h.dwBytesRecorded = 0;  // tell it no bytes have been recorded

				// re-add it  (I don't know why you need to prepare it again though...)
				waveInPrepareHeader(hWaveIn, &h, sizeof(h));
				waveInAddBuffer(hWaveIn, &h, sizeof(h));
			}
		}
	}

	// Once the user hits escape, stop recording, and clean up
	waveInStop(hWaveIn);
	for (auto& h : headers)
	{
		waveInUnprepareHeader(hWaveIn, &h, sizeof(h));
	}
	waveInClose(hWaveIn);

}


void setupWhisper()
{
	ctx = whisper_init("d:\\models\\ggml-base.en.bin");

	//if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0) 
	//{
	//	fprintf(stderr, "failed to process audio\n");
	//	return;
	//}

	const int n_segments = whisper_full_n_segments(ctx);
	for (int i = 0; i < n_segments; ++i) {
		const char* text = whisper_full_get_segment_text(ctx, i);
		printf("%s", text);
	}

	//whisper_free(ctx);
}

void onLoad()
{
	appFrameBuffer().setAllPixels(rgb(255,0,0));

}