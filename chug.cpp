#include <Arduino.h>
#include "chug.h"
//#define AudioDebug
#ifdef AudioDebug
 #define AUDIO_DEBUG(x) x
#else
 #define AUDIO_DEBUG(x) 
#endif

const String chug_sounds[4]={
"/ch1.wav",
"/ch2.wav",
"/ch3.wav",
"/ch4.wav"
};

// Chug - start
Chug::Chug(){
  AUDIO_DEBUG(Serial.println("Chug:: Default Constructor"););
  out=NULL;
  Init();
}
Chug::Chug(AudioOutputI2S *new_out){
  out=new_out;
  out_internal = false;
}

Chug::~Chug(){
  
}
void Chug::Init(void){ 
  AUDIO_DEBUG(Serial.println("Chug:: Init"););
  if(out==NULL){
    AUDIO_DEBUG(Serial.println("Chug:: out is null"););
    out = new AudioOutputI2S();
    out_internal=true;
  }
  mixer=new AudioOutputMixer(32,out);
  out->SetGain(0.02);
  int ch_num;
  for (ch_num=0; ch_num < NUM_CHAN; ++ch_num){
    mixer_stub[ch_num] = mixer->NewInput();
    wav[ch_num] = new AudioGeneratorWAV();
    file[ch_num]=NULL;

   WavDoneMsgSent[ch_num]=false;
   mixer_stub[ch_num]->stop();
   mixer_stub[ch_num]->SetGain(1.0);
 }
   LastChug=millis()+1000;
   ChugCycle=0;
   ChugPlaying=false;
   ChugOn=false;
   ChugOn=true;
   CalibrateSpeed();

  // In theory, we're ready to start
  AUDIO_DEBUG(Serial.printf("-- Sound System Initiating -- \n"););
}

bool Chug::TimeToChug(uint32_t Period){
   if (millis()<=(LastChug+Period)){
      return false;
   } else {
      LastChug=millis();
      return true;
   }
}
void Chug::BeginPlay(const char *wavfilename, bool is_chuff, uint8_t ch_num){
  if(ch_num > NUM_CHAN){
    ch_num=0;
  }else if(ch_num == NUM_CHAN){
    ch_num=1;
    // look for an open channel from 1 to NUM_CHAN-1
    for (int qq=1;qq<NUM_CHAN;++qq){
      if(wav[qq]->isRunning()==false){
        ch_num=qq;
        break;
      }
    }    
  }
       WavDoneMsgSent[ch_num]=false;
       if ( is_chuff == true ){
            ChugPlaying=true;
            AUDIO_DEBUG(Serial.printf(" ch %d pse %d file %10.0ld BeginPlay Chug\n",ch_num, false, file[ch_num]););
       } else {
        if (wav[ch_num]->isRunning()) {
          wav[ch_num]->stop(); 
          mixer_stub[ch_num]->stop();
          if(file[ch_num] != NULL){
            delete file[ch_num];
            file[ch_num]=NULL;
          }
          #ifdef AudioDebug
          Serial.printf("-Stopped\n");
          Serial.printf(" ch %d pse %d file %10.0ld BeginPlay SFX\n",ch_num, true, file[ch_num]);
          #endif
        }
      }
        AUDIO_DEBUG(Serial.printf(" ch %d pse %d file %10.0ld BeginPlay Sound %s \n",ch_num, (ch_num==0)?false:true, file[ch_num],wavfilename););
      
  file[ch_num] = new AudioFileSourceSPIFFS(wavfilename);
  mixer_stub[ch_num]->begin();
  wav[ch_num]->begin(file[ch_num], mixer_stub[ch_num]);
}



void Chug::DoChug (void){
    if (ChugOn){
      AUDIO_DEBUG(Serial.printf(" ch %d pse %d file %10.0ld DoChug ChugCycle = %d\n",0, false, file[0],ChugCycle););
      if (wav[0]->isRunning()) {
        mixer_stub[0]->stop();
        wav[0]->stop();

        if(file[0]!=NULL){
          delete file[0];
          file[0]=NULL;
        }
        delay(1);
        AUDIO_DEBUG(Serial.printf(" ch %d pse %d file %10.0ld DoChug WAV done - Truncated\n",0, false, file[0]););
      }// truncate play
      LastChug=millis();
      ChugCycle=(ChugCycle+1)%4;
      BeginChug(chug_sounds[ChugCycle].c_str());
    }
  AUDIO_DEBUG(Serial.println(""););
}
bool Chug::UpdateChug(void){
  if (TimeToChug(PeriodMilliSec)){
    DoChug();
    return true;
  }
  return false;
}
bool Chug::AudioLoop(){
  int ch_num;
  for (ch_num=0; ch_num < NUM_CHAN; ch_num+=1){
    if (wav[ch_num]->isRunning()) {
      if (!wav[ch_num]->loop()) {
        wav[ch_num]->stop();
        mixer_stub[ch_num]->stop();
      }
    } else {
      if (!WavDoneMsgSent[ch_num]){
          WavDoneMsgSent[ch_num]=true;  
          if(file[ch_num]!=NULL){
            delete file[ch_num];
            file[ch_num]=NULL;
          }
          AUDIO_DEBUG(Serial.printf(" ch %d pse %d file %10.0ld AudioLoop WAV done\n",ch_num, (ch_num==0)?false:true, file[ch_num]););
       }
     }
   }
   return UpdateChug();
}

bool Chug::SoundEffectPlaying(void){
  return false;
}

void Chug::CalibrateSpeed(float new_mph, float new_cpm){
  new_mph=max((new_mph<0)?-new_mph:new_mph,(float)0.010);
  new_cpm=max((new_cpm<0)?-new_cpm:new_cpm,(float)0.242);
  // INPUT - MPH OUTPUT - CPM
  // CPM = FACTOR_MIN * MPH
  // FACTOR_MIN= CPM/MPH
  // WANT CPmSEC so FACTOR_mSEC = FACTOR_MIN/(60*10000) = (CPM/MPH)/(60*1000)
  MPH2CPmS=(new_cpm/new_mph)/(60.0*1000.0);
  float MPH2CPS=new_cpm/new_mph;
  Serial.printf("new_mph=%f new_cpm=%f MPH2CPS=%f MPH2CPmS=%f\n",
  new_mph, new_cpm, MPH2CPS, MPH2CPmS);
}

void Chug::SetSpeed(float new_mph){
    SpeedMPH=(new_mph>=0)?new_mph:-new_mph;
    float tmp_mph = max(SpeedMPH,(float)0.01);
    PeriodMilliSec=1.0/(MPH2CPmS*tmp_mph);
}
  
void Chug::SetGain(float new_gain){
  if(new_gain > 4.0){
    new_gain=4.0;
  }
  else if(new_gain < 0){
    new_gain = 0;
  }
  out->SetGain(new_gain);
}

void Chug::SetGainPCT(float new_gain_pct){
    SetGain(new_gain_pct/25.0);
}

// Chug - end
  
