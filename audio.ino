// Includes from ESP8266audio
#include "AudioFileSourceBuffer.h"    //input buffer
#include "AudioFileSourceICYStream.h" //input stream
#include "AudioGeneratorMP3.h"        //decoder
#include "AudioGeneratorAAC.h"        // AAC decoder
#include "AudioOutputI2S.h"           //output stream

// We now use dynamic allocation to prevent AAC SBR crashes and optimize buffer sizes!
// MP3 uses a large buffer (65KB) because the decoder is small.
// AAC uses a small buffer (25KB) because the decoder is huge (85KB).

// instances for audio components
AudioGenerator *decoder = NULL;
AudioFileSourceICYStream *file = NULL;
AudioFileSourceBuffer *buff = NULL;
AudioOutputI2S *out;

// callback function will be called if meta data were found in input stream
void MDCallback(void *cbData, const char *type, bool isUnicode,
                const char *string) {
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void)isUnicode; 

  char s1[32] = "unknown";
  char s2[64] = "unknown";

  if (type != NULL) {
    strncpy_P(s1, type, sizeof(s1));
    s1[sizeof(s1) - 1] = 0;
  }
  if (string != NULL) {
    strncpy_P(s2, string, sizeof(s2));
    s2[sizeof(s2) - 1] = 0;
  }
  
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr ? ptr : "NULL", s1, s2);
  Serial.flush();
}

// stop playing the input stream release memory, delete instances
void stopPlaying() {
  if (decoder) {
    decoder->stop();
    delete decoder;
    decoder = NULL;
  }
  if (buff) {
    buff->close();
    delete buff;
    buff = NULL;
  }
  if (file) {
    file->close();
    delete file;
    file = NULL;
  }
}

// start playing a stream from current active station
void startUrl() {
  stopPlaying(); // first close existing streams
  // open input file for selected url
  Serial.printf("Active station %s\n", stationlist[actStation].url);
  file = new AudioFileSourceICYStream(stationlist[actStation].url);
  // register callback for meta data
  file->RegisterMetadataCB(MDCallback, NULL);
  String urlStr = String(stationlist[actStation].url);
  int dynamicBuffSize = 75 * 1024; // Default to 75KB for MP3 (~4.7 seconds of buffer at 128kbps)
  if (urlStr.indexOf("aac") >= 0 || urlStr.indexOf("AAC") >= 0) {
    dynamicBuffSize = 25 * 1024; // Reduce buffer to 25KB for AAC (~3.2 seconds at 64kbps) to leave enough RAM for the 85KB SBR decoder
  }
  
  // The buffer is MANDATORY for ICY streams! Without it, network latency freezes the decoder.
  buff = new AudioFileSourceBuffer(file, dynamicBuffSize);
  Serial.printf("sourcebuffer created (size=%d) - Free mem=%d\n", dynamicBuffSize, ESP.getFreeHeap());
  
  if (!file->isOpen()) {
    Serial.println("Failed to open stream!");
    return;
  }

  // create and start a new decoder with dynamic allocation depending on format
  if (urlStr.indexOf("aac") >= 0 || urlStr.indexOf("AAC") >= 0) {
    decoder = (AudioGenerator *)new AudioGeneratorAAC();
  } else {
    decoder = (AudioGenerator *)new AudioGeneratorMP3();
  }
  Serial.println("created decoder");
  Serial.println("Decoder start...");
  Serial.flush();
  decoder->begin(buff, out); // Stream through buffer!
}

void setup_audio() {
  // create I2S output for external DAC (e.g. PCM5102)
  // parameters: port=0, output_mode=0 (EXTERNAL_I2S), dma_buf_count=32 (default is 8), use_apll=0
  out = new AudioOutputI2S(0, 0, 32, 0);
}

// to be called in 'loop()'
int loop_audio() {
  if (decoder && decoder->isRunning()) {
    if (!decoder->loop()) {
      decoder->stop();
    }
    return (true);
  }
  return (false);
}
