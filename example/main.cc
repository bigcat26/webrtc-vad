#include "common_audio/vad/include/vad.h"

int main() {
    auto vad = webrtc::CreateVad(webrtc::Vad::kVadNormal);
    return 0;
}
