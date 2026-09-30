// Includes from ESP8266audio
#include "AudioFileSourceBuffer.h"    //input buffer
#include "AudioFileSourceICYStream.h" //input stream
#include "AudioGeneratorMP3.h"        //decoder
#include "AudioOutputI2S.h"           //output stream

// buffer size for stream buffering
//  Increased from 30KB to 60KB. 30KB causes buffer underruns (stuttering).
//  120KB caused malloc crashes. 60KB is the sweet spot.
const int preallocateBufferSize = 60 * 1024; // was 120*1024, then 30*1024
const int preallocateCodecSize = 29192; // 29192 is the exact max mem needed for
                                        // MP3 codec, saving 10KB RAM over 40000
// pointer to preallocated memory
void *preallocateBuffer = NULL;
void *preallocateCodec = NULL;

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
  // create a new buffer which uses the preallocated memory
  buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
  Serial.printf_P(PSTR("sourcebuffer created - Free mem=%d\n"),
                  ESP.getFreeHeap());
  // create and start a new decoder with preallocation
  decoder = (AudioGenerator *)new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
  Serial.printf_P(PSTR("created decoder\n"));
  Serial.printf_P("Decoder start...\n");
  decoder->begin(buff, out);
}

void setup_audio() {
  // reserve buffer for decoder and stream (size is now safely set to 60KB)
  preallocateBuffer = malloc(preallocateBufferSize); // Stream-file-buffer
  preallocateCodec = malloc(preallocateCodecSize);   // Decoder-buffer
  
  if (!preallocateBuffer || !preallocateCodec) {
    Serial.printf_P(
        PSTR("FATAL ERROR:  Unable to preallocate %d bytes for app\n"),
        preallocateBufferSize + preallocateCodecSize);
    while (1) {
      yield(); // Infinite halt
    }
  }

  // create I2S output for external DAC (e.g. PCM5102)
  out = new AudioOutputI2S();
}

// to be called in 'loop()'
int loop_audio() {
  if (decoder->isRunning()) {
    if (!decoder->loop()) {
      decoder->stop();
    }
    return (true);
  }
  return (false);
}
