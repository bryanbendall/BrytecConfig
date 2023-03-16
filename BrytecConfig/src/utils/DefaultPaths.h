#pragma once

#ifdef _WIN32
// Windows
#define CONFIGS_PATH "C:\\Users\\b_ben\\OneDrive\\Documents\\BrytecConfig\\BrytecConfig\\data\\configs\\"
#define MODULES_PATH "C:\\Users\\b_ben\\OneDrive\\Documents\\BrytecConfig\\BrytecConfig\\data\\modules\\"
#define NODE_GROUPS_PATH "C:\\Users\\b_ben\\OneDrive\\Documents\\BrytecConfig\\BrytecConfig\\data\\node groups\\"
#define MODULE_SAVE_MEGA_PATH "C:\\Users\\b_ben\\OneDrive\\Desktop\\"

#else
// Linux
#define CONFIGS_PATH "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/data/configs/"
#define MODULES_PATH "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/data/modules/"
#define NODE_GROUPS_PATH "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/data/node groups/"
#define MODULE_SAVE_MEGA_PATH "/home/bendall/Documents/PlatformIO/Projects/Brytec Test/.pio/libdeps/mega/BrytecConfigEmbedded/Deserializer/Program.h"
#endif
