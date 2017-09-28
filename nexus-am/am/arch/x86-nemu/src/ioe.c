#include <am.h>
#include <x86.h>
#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64
#define I8042_STATUS_HASKEY_MASK 0x1
#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT)-boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

/*static inline int min(int x, int y){
    return (x<y)?x:y;
}*/

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
/*
  int i;
  for (i = 0; i < _screen.width * _screen.height; i++) {
    fb[i] = i;
  }
  */
  /*int cp_bytes = sizeof(uint32_t) * min(w, _screen.width-x);
  for(int j=0;j<h && y+j<_screen.height;j++){
      memcpy(&fb[(y+j)*w+x], pixels, cp_bytes);
      pixels+=w;
  }*/
  int i=0;
  for(i=0; i<h; i++){
      memcpy(fb+x+400*(i+y), pixels+i*w, w*sizeof(uint32_t));
  }
}

void _draw_sync() {
}

int _read_key() {
    int key;
    key = (I8042_STATUS_HASKEY_MASK&inb(I8042_STATUS_PORT))?inl(I8042_DATA_PORT):_KEY_NONE;
  return key;
}
