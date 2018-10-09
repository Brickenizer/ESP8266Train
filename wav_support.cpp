#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets

//#include <Hash.h>

#include <i2s.h>
#include <i2s_reg.h>
#include "wavspiffs.h"
#include "wav_support.h"

// Non-blocking I2S write for left and right 16-bit PCM
bool ICACHE_FLASH_ATTR i2s_write_lr_nb(int16_t left, int16_t right){
  int sample = right & 0xFFFF;
  sample = sample << 16;
  sample |= left & 0xFFFF;
  return i2s_write_sample_nb(sample);
}

struct I2S_status_s {
  wavFILE_t wf;
  int16_t buffer[512];
  int bufferlen;
  int buffer_index;
  int playing;
} I2S_WAV;

void wav_stopPlaying()
{
  i2s_end();
  I2S_WAV.playing = false;
  wavClose(&I2S_WAV.wf);
}

bool wav_playing()
{
  return I2S_WAV.playing;
}

void wav_setup()
{
  Serial.println(F("wav_setup"));
  I2S_WAV.bufferlen = -1;
  I2S_WAV.buffer_index = 0;
  I2S_WAV.playing = false;
}

void wav_loop()
{
  bool i2s_full = false;
  int rc;

  while (I2S_WAV.playing && !i2s_full) {
    while (I2S_WAV.buffer_index < I2S_WAV.bufferlen) {
      int16_t pcm = I2S_WAV.buffer[I2S_WAV.buffer_index];
      if (i2s_write_lr_nb(pcm, pcm)) {
        I2S_WAV.buffer_index++;
      }
      else {
        i2s_full = true;
        break;
      }
      if ((I2S_WAV.buffer_index & 0x3F) == 0) yield();
    }
    if (i2s_full) break;

    rc = wavRead(&I2S_WAV.wf, I2S_WAV.buffer, sizeof(I2S_WAV.buffer));
    if (rc > 0) {
      //Serial.printf("wavRead %d\r\n", rc);
      I2S_WAV.bufferlen = rc / sizeof(I2S_WAV.buffer[0]);
      I2S_WAV.buffer_index = 0;
    }
    else {
      Serial.println(F("Stop playing"));
      wav_stopPlaying();
      break;
    }
  }
}

void wav_startPlayingFile(const char *wavfilename)
{
  wavProperties_t wProps;
  int rc;

  Serial.printf("wav_starPlayingFile(%s)\r\n", wavfilename);
  i2s_begin();
  rc = wavOpen(wavfilename, &I2S_WAV.wf, &wProps);
  Serial.printf("wavOpen %d\r\n", rc);
  if (rc != 0) {
    Serial.println("wavOpen failed");
    return;
  }
  Serial.printf("audioFormat %d\r\n", wProps.audioFormat);
  Serial.printf("numChannels %d\r\n", wProps.numChannels);
  Serial.printf("sampleRate %d\r\n", wProps.sampleRate);
  Serial.printf("byteRate %d\r\n", wProps.byteRate);
  Serial.printf("blockAlign %d\r\n", wProps.blockAlign);
  Serial.printf("bitsPerSample %d\r\n", wProps.bitsPerSample);

  i2s_set_rate(wProps.sampleRate);

  I2S_WAV.bufferlen = -1;
  I2S_WAV.buffer_index = 0;
  I2S_WAV.playing = true;
  wav_loop();
}

void wav_showDir(void)
{
  wavFILE_t wFile;
  wavProperties_t wProps;
  int rc;

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.println(dir.fileName());
    rc = wavOpen(dir.fileName().c_str(), &wFile, &wProps);
    if (rc == 0) {
      Serial.printf("  audioFormat %d\r\n", wProps.audioFormat);
      Serial.printf("  numChannels %d\r\n", wProps.numChannels);
      Serial.printf("  sampleRate %d\r\n", wProps.sampleRate);
      Serial.printf("  byteRate %d\r\n", wProps.byteRate);
      Serial.printf("  blockAlign %d\r\n", wProps.blockAlign);
      Serial.printf("  bitsPerSample %d\r\n", wProps.bitsPerSample);
      Serial.println();
      wavClose(&wFile);
    }
  }
}
