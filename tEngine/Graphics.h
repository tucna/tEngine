#pragma once

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

class Graphics
{
public:
  Graphics(HWND hWnd);
  Graphics(const Graphics&) = delete;

  ~Graphics() {};

  Graphics& operator=(const Graphics&) = delete;

  void EndFrame();
  void Clear(float r, float g, float b) noexcept;

private:
  Microsoft::WRL::ComPtr<ID3D11Device> m_device;
  Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_target;
};

