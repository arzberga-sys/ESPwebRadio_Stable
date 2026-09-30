// Includes from ESP8266audio
#include "AudioFileSourceBuffer.h"    //input buffer
#include "AudioFileSourceICYStream.h" //input stream
#include "AudioGeneratorMP3.h"        //decoder
#include "AudioGeneratorAAC.h"        // AAC decoder
#include "AudioOutputI2S.h"           //output stream

// buffer size for stream buffering
//  Increased to 90KB. 60KB was slightly too small for 192kbps streams (Nius Radio).
const int preallocateBufferSize = 90 * 1024;
// AAC with SBR (HE-AAC) requires ~85KB. MP3 only needs 29KB.
const int preallocateCodecSize = 85000;
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
  // The buffer is MANDATORY for ICY streams! Without it, network latency freezes the decoder.
  buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
  Serial.printf("sourcebuffer created - Free mem=%d\n", ESP.getFreeHeap());
  
  if (!file->isOpen()) {
    Serial.println("Failed to open stream!");
    return;
  }

  // create and start a new decoder with preallocation depending on format
  String urlStr = String(stationlist[actStation].url);
  if (urlStr.indexOf("aac") >= 0 || urlStr.indexOf("AAC") >= 0) {
    decoder = (AudioGenerator *)new AudioGeneratorAAC(preallocateCodec, preallocateCodecSize);
  } else {
    decoder = (AudioGenerator *)new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
  }
  Serial.println("created decoder");
  Serial.println("Decoder start...");
  Serial.flush();
  decoder->begin(buff, out); // Stream through buffer!
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
