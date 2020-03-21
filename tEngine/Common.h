#pragma once

//TODO _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS must be in "Preprocessor Definitions" - would be happier to find another way how to handle string to wstring

#define NOMINMAX
#include <Windows.h>

#include <codecvt>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <exception>
#include <locale>
#include <stdint.h>
#include <string>
#include <wrl.h>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxguid.lib")

// Useful macros
#define LOG_NUMBER(n) OutputDebugString((std::to_string(n) + "\n").data())
#define CHECK_HR(hr) if (FAILED(hr)) throw std::exception();

// Flow control macros

// 0 disabled, 1 low, 2 high
#define DEBUG_LEVEL 0

// Constants
constexpr uint16_t windowWidth = 1280;
constexpr uint16_t windowHeight = 720;

namespace Utils
{
  inline std::string To_string(std::wstring wstr) { std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter; return strconverter.to_bytes(wstr); }
  inline std::wstring To_wstring(std::string str) { std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter; return strconverter.from_bytes(str); }
}
