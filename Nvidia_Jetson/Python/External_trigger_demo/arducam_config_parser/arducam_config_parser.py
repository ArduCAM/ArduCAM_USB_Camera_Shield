# -*- coding: utf-8 -*- 
from ctypes import *
import sys, os
import platform
try:
    abs_path = os.path.dirname(os.path.abspath(__file__))
    if platform.system() == "Windows":
        lib_name = "arducam_config_parser.dll"
    elif platform.system() == "Linux":
        lib_name = "libarducam_config_parser.so"
    
    abs_lib_name = os.path.join(abs_path, lib_name)
    if os.path.exists(abs_lib_name):
        _lib = cdll.LoadLibrary(abs_lib_name)
    else:
        _lib = cdll.LoadLibrary(lib_name)
except Exception as e:
    print("Load libarducam_config_parser fail.")
    print("Make sure you have arducam_config_parser installed.")
    print("For more information, please visit: https://github.com/ArduCAM/arducam_config_parser")
    print(e)
    sys.exit(0)


MAX_CONFIGS = 8192
SECTION_TYPE_CAMERA     = (0x01 << 24)
SECTION_TYPE_BOARD      = (0x02 << 24)
SECTION_TYPE_REG        = (0x03 << 24)

SECTION_TYPE_BOARD_2    = SECTION_TYPE_BOARD | (0x02 << 16)
SECTION_TYPE_BOARD_3_2  = SECTION_TYPE_BOARD | (0x04 << 16)
SECTION_TYPE_BOARD_3_3  = SECTION_TYPE_BOARD | (0x03 << 16)
SECTION_TYPE_REG_3_2    = SECTION_TYPE_REG | (0x04 << 16)
SECTION_TYPE_REG_3_3    = SECTION_TYPE_REG | (0x03 << 16)

CONFIG_TYPE_REG     = 0x0001
CONFIG_TYPE_VRCMD   = 0x0002
CONFIG_TYPE_DELAY   = 0x0003

# +───────────────+───────────+──────────────+
# |     8bit      |    8bit   |    16bit     |
# +───────────────+───────────+──────────────+
# | section type  | usb type  | config type  |
# +───────────────+───────────+──────────────+


class Config(Structure):
    _fields_ = [
        ("type",c_uint32),
        ("params",c_uint32 * 16),
        ("params_length",c_uint8),
    ]

class Control(Structure):
    _fields_ = [
        ("min",c_int64),
        ("max",c_int64),
        ("step",c_int32),
        ("def",c_int64),
        ("flags",c_uint32),
        ("name",c_char * 128),
        ("func",c_char * 128),
        ("code",c_char_p),
    ]

class CameraParam(Structure):
    _fields_ = [
        ("cfg_mode",c_uint32),
        ("type",c_char * 50),
        ("width",c_uint32),
        ("height",c_uint32),
        ("bit_width",c_uint8),
        ("format",c_uint16),
        ("i2c_mode",c_uint8),
        ("i2c_addr",c_uint16),
        ("trans_lvl",c_uint32),
    ]
    def getdict(struct):
        return dict((field.upper(), getattr(struct, field) if field != "format" else (struct.format >> 8, struct.format & 0xFF)) \
            for field, _ in struct._fields_)

class CameraConfigs(Structure):
    _fields_ = [
        ("camera_param", CameraParam),
        ("configs", POINTER(Config)),
        ("configs_length", c_uint32),
        ("controls", POINTER(Control)),
        ("controls_length", c_uint32),
    ]


parse = _lib.arducam_parse_config
parse.argtypes = [c_char_p, POINTER(CameraConfigs)]

def LoadConfigFile(name):
    cfgs = CameraConfigs()
    if sys.version_info[0] == 3:
        filename = name.encode('utf-8')
    else:
        filename = name
    if parse(filename, byref(cfgs)) != 0:
        raise RuntimeError("Loading configuration file {} failed.".format(name))
    return cfgs