
#include <stdio.h>
#include <stdlib.h>  
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "libretro.h"

#include "compiler.h"
#include "strres.h"
#include "xmil.h"
#include "dosio.h"
#include "fontmng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "mousemng.h"
#include "sysmng.h"
#include "taskmng.h"
#include "joymng.h"
#include "ini.h"
#include "pccore.h"
#include "iocore.h"
#include "scrndraw.h"
#include "x1f.h"
#include "diskdrv.h"
#include "timing.h"
#include "keystat.h"
#include "vramhdl.h"
#include "statsave.h"
#include "joymng.h"
#include "sdlkbd.h"
#include "fddfile.h"

#ifdef _WIN32
char slash = '\\';
#else
char slash = '/';
#endif

#define SOUNDRATE 44100.0
#define SNDSZ 735

char RPATH[512];
char RETRO_DIR[512];
const char *retro_save_directory;
const char *retro_system_directory;
const char *retro_content_directory;
char retro_system_conf[512];

char Core_Key_Sate[512];
char Core_old_Key_Sate[512];

int retrow=FULLSCREEN_WIDTH;
int retroh=FULLSCREEN_HEIGHT;
int CHANGEAV=0;

int pauseg=0;

signed short soundbuf[1024*2];

uint16_t videoBuffer[(SCREEN_PITCH / 2) * FULLSCREEN_HEIGHT];  //emu  surf

#define MAX_DISK_IMAGES 100
static char *images[MAX_DISK_IMAGES];
static int cur_disk_idx, cur_disk_num;

static retro_video_refresh_t video_cb;
static retro_environment_t environ_cb;

static  retro_input_poll_t input_poll_cb;
static retro_log_printf_t log_cb;

retro_input_state_t input_state_cb;
retro_audio_sample_t audio_cb;
retro_audio_sample_batch_t audio_batch_cb;

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { audio_cb  =cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

static long framecount = 0;
int allow_scanlines = 0;

long GetTicks(void)
{
  return (framecount * 100) / 6;
}

int pre_main(const char *floppy)
{
   xmil_main(floppy); 

   return 0;
}

void texture_init(void)
{
   memset(videoBuffer, 0,retrow*retroh*2);
}

void log_printf(const char *format, ...)
{
	va_list va;
	char formatted[1024];

	va_start(va, format);
	vsnprintf(formatted, sizeof(formatted) - 1, format, va);
	if (log_cb)
		log_cb(RETRO_LOG_INFO, "%s\n", formatted);
	else
		fprintf(stderr, "%s\n", formatted);
	va_end(va);
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   struct retro_variable variables[] = {
      { "X1_RESOLUTE" , "Resolution; LOW|HIGH" },
      { "X1_BOOTMEDIA" , "Boot media; 2HD|2D" },
      { "X1_ROMTYPE", "ROM type; X1|TURBO|TURBOZ" },
      { "X1_FPS", "FPS; AUTO|60|30|20|15" },
      { "X1_DISPSYNC", "Disp Vsync; OFF|ON" },
      { "X1_RASTER", "Raster; OFF|ON" },
      { "X1_NOWAIT", "No wait; OFF|ON" },
      { "X1_BTN_MODE", "Joy Reverse; OFF|ON" },
      { "X1_BTN_RAPID", "Joy Rapid; OFF|ON" },
      { "X1_AUDIO_RATE", "Audio sampling rate; 44100|22050|11025"},
      {	"X1_SEEKSND", "Seek Sound; OFF|ON" },
      {	"X1_SCANLINES", "Scanlines in low-res; OFF|ON" },
      { "X1_KEY_MODE", "Key mode; Keyboard|JoyKey-1|JoyKey-2|Mouse-Key" },
      { "X1_FMBOARD", "FM Board; ON|OFF"
#if defined(SUPPORT_OPMx2)
	"|DOUBLE"
#endif
      },
      { "X1_AUDIO_DELAYMS", "Audio buffer (ms); 250|100|150|200|300|350|500|750|1000" },
      { "X1_CPU_CLOCK", "Cpu clock (MHz); 4|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20" },

      { NULL, NULL },
   };

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);

   struct retro_vfs_interface_info vfs_interface_info;
   vfs_interface_info.required_interface_version = 1;
   vfs_interface_info.iface = NULL;
   if (cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs_interface_info))
     vfs_interface = vfs_interface_info.iface;

   struct retro_log_callback logging;

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
	   log_cb = logging.log;
}

static int get_booleanvar(const char *name, const char *true_string) {
  struct retro_variable var = {0};

  var.key = name;
  var.value = NULL;

  return environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value && strcmp(var.value, true_string) == 0;
}

static void update_variables(void)
{
   struct retro_variable var = {0};

   int audiorate = 44100;

   var.key = "X1_AUDIO_RATE";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      audiorate = atoi(var.value);      
   }

   if (audiorate != 44100 && audiorate != 22050 && audiorate != 11025)
     audiorate = 44100;

   if (xmilcfg.samplingrate != (UINT16)audiorate) {
     xmilcfg.samplingrate = (UINT16)audiorate;
     corestat.soundrenewal = 1;
   }

   int resolute = get_booleanvar("X1_RESOLUTE", "LOW");
   int bootmedia = get_booleanvar("X1_BOOTMEDIA", "2D");

   xmilcfg.DIP_SW = (resolute ? DIPSW_RESOLUTE : 0) | (bootmedia ? DIPSW_BOOTMEDIA : 0);

   int romtype = 1;

   var.key = "X1_ROMTYPE";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "X1") == 0)
         romtype = 1;
      else if (strcmp(var.value, "TURBO") == 0)
         romtype = 2;
      else if (strcmp(var.value, "TURBOZ") == 0)
         romtype = 3;
   }

   xmilcfg.ROM_TYPE = romtype;

   int fps = 0;

   var.key = "X1_FPS";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "AUTO") == 0)
         fps = 0;
      else if (strcmp(var.value, "60") == 0)
         fps = 1;
      else if (strcmp(var.value, "30") == 0)
         fps = 2;
      else if (strcmp(var.value, "20") == 0)
         fps = 3;
      else if (strcmp(var.value, "15") == 0)
         fps = 4;
   }

   xmiloscfg.DRAW_SKIP = fps;

   xmilcfg.DISPSYNC = get_booleanvar("X1_DISPSYNC", "ON");
   xmilcfg.RASTER = get_booleanvar("X1_RASTER", "ON");
   xmiloscfg.NOWAIT = get_booleanvar("X1_NOWAIT", "ON");
   xmilcfg.BTN_MODE = get_booleanvar("X1_BTN_MODE", "ON");
   xmilcfg.BTN_RAPID = get_booleanvar("X1_BTN_RAPID", "ON");
   xmilcfg.MOTOR = get_booleanvar("X1_SEEKSND", "ON");
   allow_scanlines = get_booleanvar("X1_SCANLINES", "ON");

   int keymode = 0;
   var.key = "X1_KEY_MODE";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "Keyboard") == 0)
         keymode = 0;
      else if (strcmp(var.value, "JoyKey-1") == 0)
         keymode = 1;
      else if (strcmp(var.value, "JoyKey-2") == 0)
         keymode = 2;
      else if (strcmp(var.value, "Mouse-Key") == 0)
         keymode = 3;
   }

   if (xmilcfg.KEY_MODE != keymode) {
     xmilcfg.KEY_MODE = keymode;
     keystat_resetjoykey();
   }

   int fmboard = 1;

   var.key = "X1_FMBOARD";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "OFF") == 0)
         fmboard = 0;
      else if (strcmp(var.value, "ON") == 0)
         fmboard = 1;
      else if (strcmp(var.value, "DOUBLE") == 0)
         fmboard = 2;
   }

   xmilcfg.SOUND_SW = fmboard;

   int delayms = 250;

   var.key = "X1_AUDIO_DELAYMS";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      delayms = atoi(var.value);      
   }

   if (delayms < 100 || delayms > 1000)
     delayms = 250;

   xmilcfg.delayms = delayms;

   xmilcfg.skipline = 0;
   xmilcfg.skiplight = 0;

   int cpuclock = 4;

   var.key = "X1_CPU_CLOCK";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      cpuclock = atoi(var.value);
   }

   if (cpuclock < 1 || cpuclock > 20)
     cpuclock = 4;

   xmilcfg.X1_CPU_CLOCK = 1000000 * cpuclock;
}

#define KEYP(a,b) {\
	if(Core_Key_Sate[a] && Core_Key_Sate[a]!=Core_old_Key_Sate[a]  )\
		sdlkbd_keydown(a);\
	else if ( !Core_Key_Sate[a] && Core_Key_Sate[a]!=Core_old_Key_Sate[a]  )\
		sdlkbd_keyup(a);\
}	

void update_input(void)
{
 	static int mposx=640/2,mposy=400/2;
	int i;

  	input_poll_cb();

  	joymng_sync();




   		for(i=0;i<320;i++)
      			Core_Key_Sate[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;

   		if(memcmp( Core_Key_Sate,Core_old_Key_Sate , sizeof(Core_Key_Sate) ) )
			for(i=0;i<320;i++){
				KEYP(i,i);
			}

   		memcpy(Core_old_Key_Sate,Core_Key_Sate , sizeof(Core_Key_Sate) );
}


#if 0
static void keyboard_cb(bool down, unsigned keycode, uint32_t character, uint16_t mod)
{
}
#endif

/************************************
 * libretro implementation
 ************************************/

//static struct retro_system_av_info g_av_info;

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name = "x1";
   info->library_version = "0.60";
   info->need_fullpath = true;
   info->valid_extensions = "dx1|zip|2d|2hd|tfd|d88|88d|hdm|xdf|dup|cmd";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   /* FIXME handle PAL/NTSC */
   struct retro_game_geometry geom = { retrow, retroh,
				       FULLSCREEN_WIDTH,
				       FULLSCREEN_HEIGHT,
				       4.0 / 3.0 };
   struct retro_system_timing timing = { 60.0, SOUNDRATE };

   info->geometry = geom;
   info->timing   = timing;
}

void update_geometry(void)
{
   struct retro_system_av_info system_av_info;
   system_av_info.geometry.base_width = retrow;
   system_av_info.geometry.base_height = retroh;
   system_av_info.geometry.aspect_ratio = (float)4/3;// retro_aspect;
   environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &system_av_info);
}


void retro_set_controller_port_device(unsigned port, unsigned device)
{
    (void)port;
    (void)device;
}

#define SERIAL_SIZE 265000

size_t retro_serialize_size(void)
{
	return SERIAL_SIZE;
}

bool retro_serialize(void *data, size_t size)
{
  FILEH fh = make_writemem_file();
  int res= statsave_save_fh(fh);
  if (res<0)
    return false;
  if (fh->memsize + 8 > size)
    return false;
  memset(data, 0, size);
  memcpy((char *)data, &framecount, 8);
  memcpy((char *)data + 8, fh->mem, fh->memsize);
  return true;
}

bool retro_unserialize(const void *data, size_t size)
{
  memcpy(&framecount, (char *)data, 8);
  FILEH fh = make_readmem_file(data+8, size-8);
  return statsave_load_fh(fh) >= 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
    (void)index;
    (void)enabled;
    (void)code;
}

bool retro_load_game(const struct retro_game_info *info)
{
   const char *full_path;

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;

   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
	   log_printf("RGB565 is not supported.\n");
	   return false;
   }


   full_path = info->path;
   images[0] = strdup(full_path);
   cur_disk_idx = 0;
   cur_disk_num = full_path ? 1 : 0;

   strcpy(RPATH,full_path);

   log_printf("LOAD EMU\n");

   return true;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
    (void)game_type;
    (void)info;
    (void)num_info;
    return false;
}

void retro_unload_game(void)
{
     pauseg=0;
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void *retro_get_memory_data(unsigned id)
{
    return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    return 0;
}

bool set_eject_state(bool ejected) {
  if (ejected || cur_disk_idx >= cur_disk_num) {
    fddfile_eject(0);
  } else {
    diskdrv_setfdd(0, images[cur_disk_idx], 0);
  }
  return 1;
}

/* TODO: support FDD 1. */

bool get_eject_state (void) {
  return !fddfile_diskready(0);
}

unsigned get_image_index (void) {
    return cur_disk_idx;
}

bool set_image_index(unsigned index) {
  cur_disk_idx = index;
  return 1;
}

unsigned get_num_images(void) {
    return cur_disk_num;
}

bool replace_image_index(unsigned index,
			 const struct retro_game_info *info) {
  if (index >= cur_disk_num)
    return 0;
  images[index] = strdup(info->path);
  return 1;
}

bool add_image_index(void) {
  if (cur_disk_num >= MAX_DISK_IMAGES - 1)
    return 0;
  cur_disk_num++;
  return 1;
}

struct retro_disk_control_callback disk_controller =
  {
   .set_eject_state = set_eject_state,
   .get_eject_state = get_eject_state,
   .get_image_index = get_image_index,
   .set_image_index = set_image_index,
   .get_num_images = get_num_images,
   .replace_image_index = replace_image_index,
   .add_image_index = add_image_index
};

void retro_init(void)
{
   const char *system_dir = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
   {
      // if defined, use the system directory			
      retro_system_directory=system_dir;		
   }		   

   const char *content_dir = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
   {
      // if defined, use the system directory			
      retro_content_directory=content_dir;		
   }			

   const char *save_dir = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
   {
      // If save directory is defined use it, otherwise use system directory
      retro_save_directory = *save_dir ? save_dir : retro_system_directory;      
   }
   else
   {
      // make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend
      retro_save_directory=retro_system_directory;
   }

   if(retro_system_directory==NULL)sprintf(RETRO_DIR, "%s\0",".");
   else sprintf(RETRO_DIR, "%s\0", retro_system_directory);

   sprintf(retro_system_conf, "%s%cxmil\0",RETRO_DIR,slash);

   struct retro_input_descriptor inputDescriptors[] = {
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "A" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "B" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "X" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Y" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "R" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "L" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "R2" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "L2" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "R3" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "L3" }
	};
	environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &inputDescriptors);

/*
    struct retro_keyboard_callback cbk = { keyboard_cb };
    environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cbk);
*/
	environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE, &disk_controller);
  	update_variables();

    memset(Core_Key_Sate,0,512);
    memset(Core_old_Key_Sate ,0, sizeof(Core_old_Key_Sate));
}

void initload() {
  update_variables();
}

void retro_deinit(void)
{
   xmil_end();
   log_printf("Retro DeInit\n");
}

void retro_reset(void)
{
}

static int firstcall=1;

void retro_run(void)
{
   framecount++;
   if(firstcall)
   {
      pre_main(RPATH);
      update_variables();
      mousemng_enable(MOUSEPROC_SYSTEM);
      firstcall=0;
      log_printf("INIT done\n");
      return;
   }

   if (CHANGEAV == 1)
   {
      update_geometry();
      log_printf("w:%d h:%d a:%f\n",retrow,retroh,(float)(4/3));
      CHANGEAV=0;
   }

   bool updated = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
   {
      update_variables();
   }


   update_input();

   pccore_exec(TRUE);
   sound_play_cb(NULL, NULL,SNDSZ*4);

   video_cb(videoBuffer, retrow, retroh, SCREEN_PITCH);
}

