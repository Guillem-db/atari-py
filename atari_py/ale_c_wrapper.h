#ifndef __ALE_C_WRAPPER_H__
#define __ALE_C_WRAPPER_H__

#include <ale_interface.hpp>
#include <immintrin.h>

extern "C" {
  // Declares int rgb_palette[256]
  #include "atari_ntsc_rgb_palette.h"
  ALEInterface *ALE_new() {return new ALEInterface();}
  void ALE_del(ALEInterface *ale){delete ale;}
  const char *getString(ALEInterface *ale, const char *key){return ale->getString(key).c_str();}
  int getInt(ALEInterface *ale,const char *key) {return ale->getInt(key);}
  bool getBool(ALEInterface *ale,const char *key){return ale->getBool(key);}
  float getFloat(ALEInterface *ale,const char *key){return ale->getFloat(key);}
  void setString(ALEInterface *ale,const char *key,const char *value){ale->setString(key,value);}
  void setInt(ALEInterface *ale,const char *key,int value){ale->setInt(key,value);}
  void setBool(ALEInterface *ale,const char *key,bool value){ale->setBool(key,value);}
  void setFloat(ALEInterface *ale,const char *key,float value){ale->setFloat(key,value);}
  void loadROM(ALEInterface *ale,const char *rom_file){ale->loadROM(rom_file);}
  int act(ALEInterface *ale,int action){return ale->act((Action)action);}
  bool game_over(ALEInterface *ale){return ale->game_over();}
  void reset_game(ALEInterface *ale){ale->reset_game();}
  void getLegalActionSet(ALEInterface *ale,int *actions){
    ActionVect action_vect = ale->getLegalActionSet();
    for(unsigned int i = 0;i < ale->getLegalActionSet().size();i++){
      actions[i] = action_vect[i];
    }
  }
  int getLegalActionSize(ALEInterface *ale){return ale->getLegalActionSet().size();}
  void getMinimalActionSet(ALEInterface *ale,int *actions){
    ActionVect action_vect = ale->getMinimalActionSet();
    for(unsigned int i = 0;i < ale->getMinimalActionSet().size();i++){
      actions[i] = action_vect[i];
    }
  }
  int getMinimalActionSize(ALEInterface *ale){return ale->getMinimalActionSet().size();}
  int getFrameNumber(ALEInterface *ale){return ale->getFrameNumber();}
  int lives(ALEInterface *ale){return ale->lives();}
  int getEpisodeFrameNumber(ALEInterface *ale){return ale->getEpisodeFrameNumber();}
  void getScreen(ALEInterface *ale,unsigned char *screen_data){
    int w = ale->getScreen().width();
    int h = ale->getScreen().height();
    pixel_t *ale_screen_data = (pixel_t *)ale->getScreen().getArray();
    memcpy(screen_data,ale_screen_data,w*h*sizeof(pixel_t));
  }
  void getRAM(ALEInterface *ale,unsigned char *ram){
    unsigned char *ale_ram = ale->getRAM().array();
    int size = ale->getRAM().size();
    memcpy(ram,ale_ram,size*sizeof(unsigned char));
  }
  int getRAMSize(ALEInterface *ale){return ale->getRAM().size();}
  int getScreenWidth(ALEInterface *ale){return ale->getScreen().width();}
  int getScreenHeight(ALEInterface *ale){return ale->getScreen().height();}

  void getScreenRGB(ALEInterface *ale, unsigned char *output_buffer){
    size_t w = ale->getScreen().width();
    size_t h = ale->getScreen().height();
    size_t screen_size = w*h;
    pixel_t *ale_screen_data = ale->getScreen().getArray();

    ale->theOSystem->colourPalette().applyPaletteRGB(output_buffer, ale_screen_data, screen_size);
  }

  void getScreenRGB2(ALEInterface *ale, unsigned char *output_buffer) {
      size_t w = ale->getScreen().width();
      size_t h = ale->getScreen().height();
      size_t screen_size = w * h;
      pixel_t *ale_screen_data = ale->getScreen().getArray();

      const __m256i shuffle_mask = _mm256_setr_epi8(2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 0x80, 0x80, 0x80, 0x80,
                                                    2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 0x80, 0x80, 0x80, 0x80);
      int i = 0;
      for (; i < screen_size - 31; i += 8) {
          __m256i indices = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(ale_screen_data + i));
          __m256i indices32 = _mm256_cvtepu8_epi32(_mm256_castsi256_si128(indices));
          __m256i output = _mm256_i32gather_epi32(rgb_palette, indices32, 4);
          __m256i packed = _mm256_shuffle_epi8(output, shuffle_mask);
          int pix = i * 3;
          _mm_storeu_si128(reinterpret_cast<__m128i *>(output_buffer + pix), _mm256_castsi256_si128(packed));
          _mm_storeu_si128(reinterpret_cast<__m128i *>(output_buffer + pix + 12), _mm256_extracti128_si256(packed, 1));
      }
      for (; i < screen_size; i++) {
          unsigned int zrgb = __builtin_bswap32(rgb_palette[ale_screen_data[i]]) >> 8;
          if (i < screen_size - 1) {
              *reinterpret_cast<unsigned int *>(output_buffer + i * 3) = zrgb;
          } else {
              output_buffer[i * 3] = zrgb & 0xff;
              output_buffer[i * 3 + 1] = (zrgb >> 8) & 0xff;
              output_buffer[i * 3 + 2] = (zrgb >> 16) & 0xff;
          }
      }
  }

  void getScreenGrayscale(ALEInterface *ale, unsigned char *output_buffer){
    size_t w = ale->getScreen().width();
    size_t h = ale->getScreen().height();
    size_t screen_size = w*h;
    pixel_t *ale_screen_data = ale->getScreen().getArray();

    ale->theOSystem->colourPalette().applyPaletteGrayscale(output_buffer, ale_screen_data, screen_size);
  }

  void saveState(ALEInterface *ale){ale->saveState();}
  void loadState(ALEInterface *ale){ale->loadState();}
  ALEState* cloneState(ALEInterface *ale){return new ALEState(ale->cloneState());}
  void restoreState(ALEInterface *ale, ALEState* state){ale->restoreState(*state);}
  ALEState* cloneSystemState(ALEInterface *ale){return new ALEState(ale->cloneSystemState());}
  void restoreSystemState(ALEInterface *ale, ALEState* state){ale->restoreSystemState(*state);}
  void deleteState(ALEState* state){delete state;}
  void saveScreenPNG(ALEInterface *ale,const char *filename){ale->saveScreenPNG(filename);}

  // Encodes the state as a raw bytestream. This may have multiple '\0' characters
  // and thus should not be treated as a C string. Use encodeStateLen to find the length
  // of the buffer to pass in, or it will be overrun as this simply memcpys bytes into the buffer.
  void encodeState(ALEState *state, char *buf, int buf_len);
  int encodeStateLen(ALEState *state);
  ALEState *decodeState(const char *serialized, int len);

  // 0: Info, 1: Warning, 2: Error
  void setLoggerMode(int mode) { ale::Logger::setMode(ale::Logger::mode(mode)); }
}

#endif
