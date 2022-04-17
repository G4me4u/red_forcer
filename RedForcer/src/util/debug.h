#pragma once

#if RF_DEBUG
#if defined(HZ_PLATFORM_WINDOWS)
#define RF_DEBUGBREAK() __debugbreak()
#else
#define RF_DEBUGBREAK()
#endif

#define RF_INFO(x) std::cout << x << std::endl
#define RF_ERROR(x) RF_INFO(x)

#define RF_ASSERT(check, msg) { if(!(check)) { RF_ERROR("Assertion failed: "#msg); RF_DEBUGBREAK(); } }
#else
#define RF_INFO(x)
#define RF_ERROR(x)

#define RF_ASSERT(...)
#endif
