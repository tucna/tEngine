#pragma once

#include "Common.h"

namespace Bind
{
  class Bindable;
}

class Graphics
{
  friend class Bind::Bindable;

public:
  Graphics(HWND hWnd, uint16_t width, uint16_t height);
  Graphics(const Graphics&) = delete;

  ~Graphics() {};

  Graphics& operator=(const Graphics&) = delete;

  void BeginFrame(float red, float green, float blue) noexcept;
  void EndFrame();

  void DrawIndexed(UINT count) noexcept;

  void SetProjection(DirectX::FXMMATRIX projection) noexcept { m_projection = projection; }
  void SetCamera(DirectX::FXMMATRIX camera) noexcept { m_camera = camera; }

  DirectX::XMMATRIX GetProjectionMatrix() const noexcept { return m_projection; }
  DirectX::XMMATRIX GetViewMatrix() const noexcept { return m_camera; }

private:
  Microsoft::WRL::ComPtr<ID3D11Device> m_device;
  Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_target;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;

  DirectX::XMMATRIX m_projection;
  DirectX::XMMATRIX m_camera;
};

