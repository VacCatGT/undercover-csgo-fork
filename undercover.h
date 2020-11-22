#pragma once

#pragma warning( disable : 4307 ) // '*': integral constant overflow
#pragma warning( disable : 4244 ) // possible loss of data
#pragma warning( disable : 4800 ) // forcing value to bool 'true' or 'false'
#pragma warning( disable : 4838 ) // conversion from '::size_t' to 'int' requires a narrowing conversion
#pragma warning( disable : 4996 )

// You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

using ulong_t = unsigned long;


// windows / stl includes.
#include <Windows.h>
#include <cstdint>
#include <intrin.h>
#include <xmmintrin.h>
#include <array>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <deque>
#include <functional>
#include <atomic>
#include <map>
#include <shlobj.h>
#include <filesystem>
#include <streambuf>
#include <optional>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "Urlmon.lib")

#include <playsoundapi.h>
#include <mmeapi.h> 
#pragma comment(lib, "Winmm.lib") 

// our custom wrapper.
#include "unique_vector.h"
#include "tinyformat.h"

// imgui
#include "undercover_gui/imgui/imgui.h"
#include "undercover_gui/fonts/droid_sans.cpp"
#include "undercover_gui/fonts/droid_sans_bold.cpp"
#include "undercover_gui/fonts/agency_bold.cpp"
#include "undercover_gui/fonts/SegoeUI.cpp"
#include "undercover_gui/images/logo.c"
#include "undercover_gui/images/background.c"
#include "undercover_gui/imgui/imgui.h"
#include "undercover_gui/imgui/dx9/imgui_impl_dx9.h"

// other includes.
#include "hash.h"
#include "xorstr.h"
#include "pe.h"
#include "winapir.h"
#include "address.h"
#include "modules.h"
#include "pattern.h"
#include "util.h"
#include "vmt.h"
#include "stack.h"
#include "nt.h"
#include "x86.h"
#include "syscall.h"
#include "singleton.h"
#include "fnv1a.h"

// hack includes.
#include "sound_data.h"
#include "interfaces.h"
#include "sdk.h"
#include "csgo.h"
#include "penetration.h"
#include "netvars.h"
#include "entoffsets.h"
#include "hvh.h"
#include "entity.h"
#include "client.h"
#include "gamerules.h"
#include "hooks.h"
#include "detours.h"
#include "render.h"
#include "pred.h"
#include "lagcomp.h"
#include "dormancy.h"
#include "visuals.h"
#include "movement.h"
#include "game_movement.h"
#include "aimbot.h"
#include "animations.h"
#include "tickbase_shift.h"
#include "netdata.h"
#include "chams.h"
#include "notify.h"
#include "resolver.h"
#include "grenades.h"
#include "skins.h"
#include "events.h"
#include "hitsounds.h"
#include "shots.h"
#include "gamemovement_rebuilt.h"

// gui includes.
#include "base64.h"
#include "inputsystem.h"
#include "config.h"

#include "Renderer.h"
#include "menu.h"
#include "undercover_gui/wrapper.h"
#include "gui/Helpers/Easing.h"
#include "gui/Framework/GUI.h"
#include "gui/Elements.h"

// anti debug
#include "anti_debugger.h"