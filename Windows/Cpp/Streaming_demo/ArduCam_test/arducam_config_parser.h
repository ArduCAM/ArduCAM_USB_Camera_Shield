#ifndef __ARDUCAM_CONFIG_PARSER_H__
#define __ARDUCAM_CONFIG_PARSER_H__

#if defined _WIN32 || defined __CYGWIN__
  #define DLL_IMPORT __declspec(dllimport)
  #define DLL_EXPORT __declspec(dllexport)
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_IMPORT __attribute__ ((visibility ("default")))
    #define DLL_EXPORT __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_IMPORT
    #define DLL_EXPORT
    #define DLL_LOCAL
  #endif
#endif

#ifdef ARDUCAM_DLL
  #ifdef ARDUCAM_DLL_EXPORTS
    #define ARDUCAM_API DLL_EXPORT
  #else
    #define ARDUCAM_API DLL_IMPORT
  #endif
  #define ARDUCAM_LOCAL DLL_LOCAL
#else
  #define ARDUCAM_API
  #define ARDUCAM_LOCAL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#define MAX_CONFIGS 8192

#define SECTION_TYPE_CAMERA     (0x01 << 24)
#define SECTION_TYPE_BOARD      (0x02 << 24)
#define SECTION_TYPE_REG        (0x03 << 24)
#define SECTION_TYPE_CONTROL    (0x04 << 24)

#define SECTION_TYPE_BOARD_2    (SECTION_TYPE_BOARD | (0x02 << 16))
#define SECTION_TYPE_BOARD_3_2  (SECTION_TYPE_BOARD | (0x04 << 16))
#define SECTION_TYPE_BOARD_3_3  (SECTION_TYPE_BOARD | (0x03 << 16))

#define SECTION_TYPE_REG_3_2    (SECTION_TYPE_REG | (0x04 << 16))
#define SECTION_TYPE_REG_3_3    (SECTION_TYPE_REG | (0x03 << 16))

#define CONFIG_TYPE_REG     0x0001
#define CONFIG_TYPE_VRCMD   0x0002
#define CONFIG_TYPE_DELAY   0x0003

#define CONTROL_TYPE_MIN    0x0001
#define CONTROL_TYPE_MAX    0x0002
#define CONTROL_TYPE_STEP   0x0003
#define CONTROL_TYPE_DEF    0x0004
#define CONTROL_TYPE_NAME   0x0005
#define CONTROL_TYPE_FUNC   0x0006


// 
// +───────────────+───────────+──────────────+
// |     8bit      |    8bit   |    16bit     |
// +───────────────+───────────+──────────────+
// | section type  | usb type  | config type  |
// +───────────────+───────────+──────────────+

typedef struct {
    const char *name;
    int type;
} TypeMap;

#if !defined(__ARDUCAM_STRUCT_CONTROL__)
#define __ARDUCAM_STRUCT_CONTROL__
typedef struct {
    int64_t min;
    int64_t max;
    int32_t step;
    int64_t def;
    uint32_t flags;
    char name[128];
    char func[128];
    char *code;
} Control;
#endif

typedef struct {
    uint32_t type;
    uint32_t params[16];
    uint8_t params_length;
} Config;

typedef struct {
    uint32_t cfg_mode;
    char type[50];
    uint32_t width;
    uint32_t height;
    uint8_t bit_width;
    uint16_t format;
    uint8_t i2c_mode;
    uint16_t i2c_addr;
    uint32_t trans_lvl;
} CameraParam;

typedef struct {
    CameraParam camera_param;
    Config *configs;
    uint32_t configs_length;
    Control *controls;
    uint32_t controls_length;
} CameraConfigs;


#define LOG(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)


ARDUCAM_API int arducam_parse_config(const char *file_name, CameraConfigs *cam_cfgs);

#ifdef __cplusplus
}
#endif

#endif