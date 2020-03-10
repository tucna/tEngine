#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <exception>
#include <stdint.h>
#include <Windows.h>
#include <wrl.h>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d11.lib")

// Useful macros
#define LOG_NUMBER(n) OutputDebugString((std::to_string(n) + "\n").data())
#define CHECK_HR(hr) if (FAILED(hr)) throw std::exception();

// Flow control macros

// 0 disabled, 1 low, 2 high
#define DEBUG_LEVEL 0

