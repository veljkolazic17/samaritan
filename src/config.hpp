#pragma once
#define BUILTIN_SHADERS_PATH	"../bin/assets/shaders/"

#ifndef HACKS_ENABLED
#define HACKS_ENABLED true
#endif

#if HACKS_ENABLED
#define HACK(...) __VA_ARGS__
#else
#define HACK(...)
#endif

#ifndef USE_WIREFRAME
#define USE_WIREFRAME true
#endif
