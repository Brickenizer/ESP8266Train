#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"
#define NUM_CHAN 8

// Chug is the audio for a steam locomotive
// This is cyclic sound based on the requested speed
// TODO: time chug with speed sensor
class Chug{
  public:
  Chug(AudioOutputI2S *new_out=NULL);
  ~Chug();

  void InitDefaults(void);
  void BeginPlay(const char *wavfilename, bool is_chuff, uint8_t ch_num);
  void BeginChug(const char *wavefilename){BeginPlay(wavefilename,true,0);};
  void BeginSFX(const char *wavefilename){BeginPlay(wavefilename,false,NUM_CHAN);};
  bool AudioLoop();
  bool SoundEffectPlaying(void);
  // mph - scale speed cpm - chugs per minute
  void CalibrateSpeed(float new_mph=5, float new_cpm=120);
  void SetSpeed(float new_mph);
  float GetSpeedMPH(void){return SpeedMPH;};
  float GetPeriodMilliSec(void){return PeriodMilliSec;};
  // raw gain for I2S sound
  void  SetGain(float new_gain);
  float GetGain(){return gain;};
  // percent gain for I2S sound
  void  SetGainPCT(float new_gain_pct);
  float GetGainPCT(){return gain * 25.0;};
  
  private:
  bool TimeToChug(uint32_t Period);
  void DoChug(void);
  bool UpdateChug(void);

  // 
  int ChugCycle;  
  uint32_t LastChug;
  bool WavDoneMsgSent[NUM_CHAN];
  bool ChugPlaying;
  bool ChugOn;
  float SpeedMPH;
  float MPH2CPmS;
  float PeriodMilliSec;
  float gain;

  AudioOutputMixer      *mixer;
  AudioOutputMixerStub  *mixer_stub[NUM_CHAN];
  AudioGeneratorWAV     *wav[NUM_CHAN];
  AudioFileSourceSPIFFS *file[NUM_CHAN];
  AudioOutputI2S        *out;
  bool out_internal;
  bool initialized;
};

/*expected wav files are:
 Initiate sound:
   /initiated.wav
 
 CHUFFS:
  /ch1.wav
  /ch2.wav
  /ch3.wav
  /ch4.wav
Whistle:
  /whistle.wav
Brake Squeal
  /brakes.wav
*/
