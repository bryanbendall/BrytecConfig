#pragma once

#ifdef _WIN32
// Windows
#define CONFIGS_PATH "C:\\Users\\Bendall\\Documents\\Brytec\\BrytecConfig\\BrytecConfig\\data\\configs\\"
#define MODULES_PATH "C:\\Users\\Bendall\\Documents\\Brytec\\BrytecConfig\\BrytecConfig\\data\\modules\\"
#define NODE_GROUPS_PATH "C:\\Users\\Bendall\\Documents\\Brytec\\BrytecConfig\\BrytecConfig\\data\\node groups\\"
#define FONTS_PATH_DROID_SANS "C:\\Users\\Bendall\\Documents\\Brytec\\BrytecConfig\\BrytecConfig\\vendor\\imgui\\misc\\fonts\\DroidSans.ttf"
#define FONTS_PATH_FONT_AWESOME "C:\\Users\\Bendall\\Documents\\Brytec\\BrytecConfig\\BrytecConfig\\vendor\\fontawesome\\fa-solid-900.ttf"
#define MODULE_SAVE_MEGA_PATH "C:\\Users\\Bendall\\Desktop\\"

#else
// Linux
#define CONFIGS_PATH "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/data/configs/"
#define MODULES_PATH "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/data/modules/"
#define NODE_GROUPS_PATH "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/data/node groups/"
#define FONTS_PATH_DROID_SANS "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/vendor/imgui/misc/fonts/DroidSans.ttf"
#define FONTS_PATH_FONT_AWESOME "/home/bendall/Documents/BrytecCmake/BrytecConfig/BrytecConfig/vendor/fontawesome/fa-solid-900.ttf"
#define MODULE_SAVE_MEGA_PATH "/home/bendall/Documents/PlatformIO/Projects/Brytec Test/.pio/libdeps/env1/BrytecConfigEmbedded/Deserializer/Program.h"
#endif
