#include "Graphics.h"

Graphics::Graphics(HWND hWnd, uint16_t width, uint16_t height)
{
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
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

  // create depth stensil state
  D3D11_DEPTH_STENCIL_DESC dsDesc = {};
  dsDesc.DepthEnable = TRUE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
  CHECK_HR(m_device->CreateDepthStencilState(&dsDesc, &pDSState));

  // bind depth state
  m_context->OMSetDepthStencilState(pDSState.Get(), 1u);

  // create depth stensil texture
  D3D11_TEXTURE2D_DESC descDepth = {};
  descDepth.Width = width;
  descDepth.Height = height;
  descDepth.MipLevels = 1u;
  descDepth.ArraySize = 1u;
  descDepth.Format = DXGI_FORMAT_D32_FLOAT;
  descDepth.SampleDesc.Count = 1u;
  descDepth.SampleDesc.Quality = 0u;
  descDepth.Usage = D3D11_USAGE_DEFAULT;
  descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
  CHECK_HR(m_device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

  // create view of depth stensil texture
  D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
  descDSV.Format = DXGI_FORMAT_D32_FLOAT;
  descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  descDSV.Texture2D.MipSlice = 0u;

  CHECK_HR(m_device->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &m_DSV));

  m_context->OMSetRenderTargets(1u, m_target.GetAddressOf(), m_DSV.Get());

  D3D11_VIEWPORT vp;
  vp.Width = static_cast<float>(width);
  vp.Height = static_cast<float>(height);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0.0f;
  vp.TopLeftY = 0.0f;

  m_context->RSSetViewports(1u, &vp);
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
  const float color[] = { red,green,blue,1.0f };

  m_context->ClearRenderTargetView(m_target.Get(), color);
  m_context->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame()
{
  CHECK_HR(m_swap->Present(1, 0));
}

void Graphics::DrawIndexed(UINT count) noexcept
{
  m_context->DrawIndexed(count, 0u, 0u);
}
