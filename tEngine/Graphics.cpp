#include "Graphics.h"

Graphics::Graphics(HWND hWnd)
{
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 0;
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = 1;
  sd.OutputWindow = hWnd;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.Flags = 0;

  UINT deviceCreateFlags = 0u;
#ifndef NDEBUG
  deviceCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    deviceCreateFlags,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &sd,
    &m_swap,
    &m_device,
    nullptr,
    &m_context
  );

  Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer;
  CHECK_HR(m_swap->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
  CHECK_HR(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_target));

  m_context->OMSetRenderTargets(1u, m_target.GetAddressOf(), nullptr);

  D3D11_VIEWPORT vp;
  vp.Width = 800.0f;
  vp.Height = 600.0f;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0.0f;
  vp.TopLeftY = 0.0f;

  m_context->RSSetViewports(1u, &vp);
}

void Graphics::EndFrame()
{
  m_swap->Present(1, 0);
}

void Graphics::Clear(float r, float g, float b) noexcept
{
  const float color[] = { r, g, b, 1 };

  m_context->ClearRenderTargetView(m_target.Get(), color);
}

void Graphics::DrawIndexed(UINT count) noexcept
{
  m_context->DrawIndexed(count, 0u, 0u);
}
