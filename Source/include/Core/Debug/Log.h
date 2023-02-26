#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#pragma warning (push)
#pragma warning (disable : 4005)
#include <windows.h>
#pragma warning (pop)

namespace Core
{
    class Log
    {
    private:
        static Log& log;
        Log();
        ~Log();

        std::fstream file;
        void closeFile();
        void writeToFile(const std::string& str);

        template <typename T> static const T& arg(const T& value)
        {
            return value;
        }

        template <typename T> static const char* arg(const std::basic_string<T>& value)
        {
            return value.c_str();
        }

    public:
        static Log& getInstance();
        static void openFile(const std::filesystem::path& path);

        template <typename ... T> static void print(const char* str, const T& ... args)
        {
            std::string formatedString = Log::stringFormat(str, args ...);
            printf(formatedString.c_str());

            Log::log.writeToFile(formatedString);
        }

        template <typename ... T> static std::string stringFormat(const char* str, const T& ... args)
        {
            int size = std::snprintf(nullptr, 0, str, arg(args) ...);
            char* buffer = new char[size];
            sprintf_s(buffer, size + 1, str, Log::arg(args) ...);
            return buffer;
        }
    };

    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

    #define DEBUG_LOG(message, ...) \
        std::string formatedMsg = Core::Log::stringFormat(message, __VA_ARGS__); \
        OutputDebugString(Core::Log::stringFormat("%s(%i): %s\n", __FILENAME__, __LINE__, formatedMsg).c_str());
}