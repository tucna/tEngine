#pragma once

#include "Common.h"

class Bindable;

class Graphics
{
  friend class Bindable;

public:
  Graphics(HWND hWnd);
  Graphics(const Graphics&) = delete;

  ~Graphics() {};

  Graphics& operator=(const Graphics&) = delete;

  void EndFrame();
  void Clear(float r, float g, float b) noexcept;
  void DrawIndexed(UINT count) noexcept;

  void SetProjection(DirectX::FXMMATRIX projection) noexcept { m_projection = projection; }
  void SetCamera(DirectX::FXMMATRIX camera) noexcept { m_camera = camera; }

  DirectX::XMMATRIX GetProjection() const noexcept { return m_projection; }
  DirectX::XMMATRIX GetCamera() const noexcept { return m_camera; }

private:
  Microsoft::WRL::ComPtr<ID3D11Device> m_device;
  Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_target;

  DirectX::XMMATRIX m_projection;
  DirectX::XMMATRIX m_camera;
};

