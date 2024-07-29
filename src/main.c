#include <pd_api.h>

#include "../mod4play/m4p.h"

// It seems that Playdate always requests 256 samples, so 1024 is for safety. You may try lowering this value
#define MIX_BUFFER_SIZE 1024
#define LOG pd->system->logToConsole

// Global variables
PlaydateAPI* pd;
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

char** modsList;
int modsCount = 0;
int modIdx = 0;
int isPlaying = 0;

// mod4play supports 4 formats
char* supportedExtensions[] = {".mod", ".xm", ".s3m", ".it"};

// Playdate audio structures and a sample buffer
SoundChannel* modChannel;
SoundSource* modPlayer;
int16_t* modSampleBuffer;

// Function prototypes
static int update(void* userdata);
void findModules(void);
void loadModule(const char *path);
void unloadModule(void);
int modPlayerAudioSource(void* context, int16_t* left, int16_t* right, int len);
int buttonCallback(PDButtons button, int down, uint32_t when, void* userdata);
void fileCallbackCount(const char* filename, void* userdata);
void fileCallbackAdd(const char* filename, void* userdata);

// Playdate app event handler
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
    
    findModules();
    modSampleBuffer = malloc(MIX_BUFFER_SIZE * 2);    
    loadModule(modsList[modIdx]);
    
    // Create audio source
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

// Playdate update function
static int update(void* userdata) {
  // Draw module file name
  pd->graphics->clear(kColorWhite);
  pd->graphics->setFont(font);
  pd->graphics->drawText(modsList[modIdx], strlen(modsList[modIdx]), kASCIIEncoding, 1, 1);
  
  return 1;
}

// Input handling
int buttonCallback(PDButtons button, int down, uint32_t when, void* userdata) {
  if (down == 0) return 0;

  if (button == kButtonUp) {
    // Previous module
    unloadModule();
    modIdx = (modIdx == 0) ? modsCount - 1 : modIdx - 1;
    loadModule(modsList[modIdx]);
  } else if (button == kButtonDown) {
    // Next module
    unloadModule();
    modIdx = (modIdx == modsCount - 1) ? 0 : modIdx + 1;
    loadModule(modsList[modIdx]);
  }
  
  return 0;
}


// Scan folder to find all modules
int isModuleFile(const char* filename) {
  char* extension = strrchr(filename, '.');
  if (extension == NULL) return 0;
  
  int extCount = sizeof(supportedExtensions) / sizeof(supportedExtensions[0]);
  
  for (int c = 0; c < extCount; c++) {
    if (!strcmp(supportedExtensions[c], extension)) return 1;
  }
  
  return 0;
}

int pstrcmp(const void* a, const void* b) {
  return strcmp(*(const char**)a, *(const char**)b);
}

void findModules(void) {
  // First scan the folder to get the number of module files
  modsCount = 0;
  pd->file->listfiles("", fileCallbackCount, NULL, 0);
  modsList = malloc(modsCount * sizeof(char*));
  
  // Now store module filenames in the modsList array
  modIdx = 0;
  pd->file->listfiles("", fileCallbackAdd, NULL, 0);
  qsort(modsList, modsCount, sizeof(modsList[0]), pstrcmp);
  
  // Reset module index to zero for playback  
  modIdx = 0;
}

void fileCallbackCount(const char* filename, void* userdata) {
  if (isModuleFile(filename)) modsCount++;
}

void fileCallbackAdd(const char* filename, void* userdata) {
  if (!isModuleFile(filename)) return;
  
  modsList[modIdx] = malloc(strlen(filename) + 1);
  strcpy(modsList[modIdx], filename);
  modIdx++;
  
  LOG(filename);
}


// Load tracker module from path
void loadModule(const char* path) {
  uint8_t* musicModule;
  FileStat fileStat;

  // Read module file
  pd->file->stat(path, &fileStat);
  musicModule = malloc(fileStat.size);
  SDFile* file = pd->file->open(path, kFileRead);
  pd->file->read(file, musicModule, fileStat.size);
  pd->file->close(file);

  // It seems that Playdate always requests 256 samples, so 1024 is for safety. You may try lowering this value
  m4p_LoadFromData(musicModule, fileStat.size, 44100, MIX_BUFFER_SIZE);
  free(musicModule);
  m4p_PlaySong();
  
  isPlaying = 1;
}

// Unload module
void unloadModule(void) {
  isPlaying = 0;
  
  m4p_Close();
  m4p_FreeSong();
}

// Audio callback
int modPlayerAudioSource(void* context, int16_t* left, int16_t* right, int len) {
  if (!isPlaying) return 0;
  
  // mod4play mixes audio in a single interleaved buffer, so we need to split it to two buffers for each channel
  m4p_GenerateSamples(modSampleBuffer, len);
  
  int16_t* mixedPtr = modSampleBuffer;
  int16_t* leftPtr = left;
  int16_t* rightPtr = right;
  for (int c = 0; c < len; c++) {
    *leftPtr++ = *mixedPtr++;
    *rightPtr++ = *mixedPtr++;
  }
  
  return 1;    
}
