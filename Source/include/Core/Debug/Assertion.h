#pragma once

namespace Core
{
	#ifdef _DEBUG
		#define ASSERT(condition) if (!condition) __debugbreak();
	#else
		#define ASSERT(condition) if (!condition) std::abort();
	#endif
}