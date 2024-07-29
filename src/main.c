#include <pd_api.h>

#include "../mod4play/m4p.h"

// Functions
static int update(void* userdata);
PlaydateAPI* pd;

const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

void loadModule(int idx);
void unloadModule(void);
int modPlayerAudioSource(void* context, int16_t* left, int16_t* right, int len);
int buttonCallback(PDButtons button, int down, uint32_t when, void* userdata);

SoundChannel* modChannel;
SoundSource* modPlayer;
int16_t* modSampleBuffer;
int modIdx = 0;
int isPlaying = 0;

char* mods[] = {
  "bubblefish.xm",
  "Claustrophobia.xm",
  "inside_out.s3m",
  "lizardking_-_quest.xm",
  "meg_ccl8.s3m",
  "radix_-_scandal.xm",
  "tides_of_dosk.xm",
};

int modsCount = 0;

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pdApi, PDSystemEvent event, uint32_t arg) {
  if (event == kEventInit) {
    pd = pdApi;
    pd->system->setUpdateCallback(update, pd);
    pd->system->setButtonCallback(buttonCallback, NULL, 5);
    
    const char* err;
    font = pd->graphics->loadFont(fontpath, &err);
    
    modsCount = sizeof(mods) / sizeof(mods[0]);
    modSampleBuffer = malloc(1024 * 2);    
    loadModule(modIdx);
    
    modChannel = pd->sound->channel->newChannel();
    modPlayer = pd->sound->channel->addCallbackSource(modChannel, modPlayerAudioSource, NULL, 1);
    
  } else if (event == kEventLock) {
    // Lock Playdate
  } else if (event == kEventUnlock) {
    // Unlocked playdate
  } else if (event == kEventPause) {
    // Game put on pause
  } else if (event == kEventResume) {
    // Resume after pause
  } else if (event == kEventTerminate) {
    // Terminate game
    
    unloadModule();
    pd->sound->channel->freeChannel(modChannel);
    pd->system->realloc(modPlayer, 0);
    free(modSampleBuffer);
  }
  
  return 0;
}

static int update(void* userdata) {
  PlaydateAPI* pd = userdata;

  pd->graphics->clear(kColorWhite);
  pd->graphics->setFont(font);
  pd->graphics->drawText(mods[modIdx], strlen(mods[modIdx]), kASCIIEncoding, 1, 1);

  //pd->system->drawFPS(0,0);
  return 1;
}


int buttonCallback(PDButtons button, int down, uint32_t when, void* userdata) {
  if (down == 0) return 0;
  
  if (button == kButtonUp) {
    unloadModule();
    if (modIdx == 0) {
      modIdx = modsCount - 1;
    } else {
      modIdx--;
    }
    loadModule(modIdx);
  } else if (button == kButtonDown) {
    unloadModule();
    if (modIdx == modsCount - 1) {
      modIdx = 0;
    } else {
      modIdx++;
    }
    loadModule(modIdx);
  }
  
  return 0;
}


void loadModule(int idx) {
  uint8_t* musicModule;
  FileStat fileStat;

  pd->file->stat(mods[idx], &fileStat);
  musicModule = malloc(fileStat.size);
  SDFile* file = pd->file->open(mods[idx], kFileRead);
  pd->file->read(file, musicModule, fileStat.size);
  pd->file->close(file);

  m4p_LoadFromData(musicModule, fileStat.size, 44100, 1024);
  free(musicModule);
  m4p_PlaySong();
  isPlaying = 1;
}

void unloadModule(void) {
  isPlaying = 0;
  m4p_Close();
  m4p_FreeSong();
}

int modPlayerAudioSource(void* context, int16_t* left, int16_t* right, int len) {
  if (isPlaying) {
    m4p_GenerateSamples(modSampleBuffer, len);
    
    int16_t* mixedPtr = modSampleBuffer;
    int16_t* leftPtr = left;
    int16_t* rightPtr = right;
    for (int c = 0; c < len; c++) {
      *leftPtr++ = *mixedPtr++;
      *rightPtr++ = *mixedPtr++;
    }
    
    return 1;    
  } else {
    return 0;
  }
}

