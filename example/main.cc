#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common_audio/vad/include/vad.h"
#include "modules/audio_processing/vad/voice_activity_detector.h"

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"

typedef struct {
  mp3dec_t *mp3d;
  mp3dec_file_info_t *info;
  size_t allocated;
} FramesIterateData;

static int mp3FramesIterateCallback(void *user_data, const uint8_t *frame,
                                    int frame_size, int free_format_bytes,
                                    size_t buf_size, uint64_t offset,
                                    mp3dec_frame_info_t *info) {
  (void)buf_size;
  (void)offset;
  (void)free_format_bytes;
  FramesIterateData *d = (FramesIterateData *)user_data;
  d->info->channels = info->channels;
  d->info->hz = info->hz;
  d->info->layer = info->layer;

  if ((d->allocated - d->info->samples * sizeof(mp3d_sample_t)) <
      MINIMP3_MAX_SAMPLES_PER_FRAME * sizeof(mp3d_sample_t)) {
    if (!d->allocated) {
      d->allocated = 1024 * 1024;
    } else {
      d->allocated *= 2;
    }

    mp3d_sample_t *buf =
        (mp3d_sample_t *)realloc(d->info->buffer, d->allocated);
    if (!buf) {
      return MP3D_E_MEMORY;
    }
    d->info->buffer = buf;
  }
  int samples = mp3dec_decode_frame(d->mp3d, frame, frame_size,
                                    d->info->buffer + d->info->samples, info);
  if (samples) {
    d->info->samples += samples * info->channels;
  }

  return 0;
}

std::vector<mp3d_sample_t> loadMp3(const std::string &path) {
  std::vector<mp3d_sample_t> pcm;
  std::ifstream ifs(path, std::ios::binary | std::ios::in);
  if (!ifs.is_open()) {
    std::cerr << "Failed to open input file: " << path << std::endl;
    return pcm;
  }

  ifs.seekg(0, std::ios::end);
  size_t fileLength = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(fileLength);
  ifs.read(reinterpret_cast<char *>(buffer.data()), fileLength);
  ifs.close();

  mp3dec_t mp3d;
  mp3dec_init(&mp3d);

  mp3dec_file_info_t info;
  memset(&info, 0, sizeof(info));
  FramesIterateData d = {&mp3d, &info, 0};
  mp3dec_iterate_buf(&buffer[0], buffer.size(), mp3FramesIterateCallback, &d);

  // copy pcm data from channel 0
  pcm.resize(info.samples / info.channels);
  for (size_t i = 0; i < info.samples / info.channels; i++) {
    pcm[i] = info.buffer[i * info.channels];
  }
  return pcm;
}

void checkVoiceActivity(const std::vector<mp3d_sample_t> &pcm) {
  auto vad = webrtc::CreateVad(webrtc::Vad::kVadVeryAggressive);
  int sampleRate = 16000;
  int frameLength = 160;
  int numFrames = pcm.size() / frameLength;
  int lastActivity = 0;
  webrtc::Vad::Activity last = webrtc::Vad::kError;
  for (int i = 0; i < numFrames; i++) {
    auto activity = vad->VoiceActivity(&pcm[i * frameLength], frameLength, sampleRate);
    if (activity != last) {
      // convert frame index to seconds
      auto ms = i * frameLength * 1000 / sampleRate;
      // print activity with timestamp in MM:SS.sss format with activity
      auto millis = ms % 1000;
      auto seconds = (ms / 1000) % 60;
      auto minutes = ms / 60000;
      std::cout << minutes << ":" << seconds << "." << millis << " activity=" << activity
                << " duration=" << ms - lastActivity << "ms" << std::endl;
      last = activity;
      lastActivity = ms;
    }
  }
  std::cout << "Done" << std::endl;
}

void testCommonAudioVad() {
  auto pcm = loadMp3(PROJECT_ROOT "/samples/airport-broadcast-2.mp3");
  checkVoiceActivity(pcm);
}

void testAudioProcessingVad() {
    webrtc::VoiceActivityDetector vad;

}

int main() {
  testAudioProcessingVad();
  return 0;
}
