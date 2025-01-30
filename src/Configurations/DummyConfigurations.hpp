#pragma once

#include <iostream>

struct DummyConfigurations
{
  DummyConfigurations() = default;
  DummyConfigurations(uint32_t p_ui32FramesCount, char *p_pDestAdress, char *p_pSrcAddress) : m_ui32framesCount{p_ui32FramesCount}, m_pDestAdress{p_pDestAdress}, m_pSrcAddress{p_pSrcAddress} {}
  DummyConfigurations(DummyConfigurations &&other) : m_ui32framesCount{other.m_ui32framesCount}, m_pDestAdress{other.m_pDestAdress}, m_pSrcAddress{other.m_pSrcAddress}
  {
    other.m_pDestAdress = nullptr;
    other.m_pSrcAddress = nullptr;
  }

  DummyConfigurations &operator=(DummyConfigurations &&other)
  {
    m_pDestAdress = other.m_pDestAdress;
    m_pSrcAddress = other.m_pSrcAddress;
    m_ui32framesCount = other.m_ui32framesCount;
    other.m_pDestAdress = nullptr;
    other.m_pSrcAddress = nullptr;
    return *this;
  };

  //! NOT copyable
  DummyConfigurations(DummyConfigurations &other) = delete;
  DummyConfigurations &operator=(DummyConfigurations &) = delete;

  ~DummyConfigurations()
  {
    delete[] m_pDestAdress;
    delete[] m_pSrcAddress;
  }
  uint32_t m_ui32framesCount{0};
  char *m_pDestAdress{nullptr};
  char *m_pSrcAddress{nullptr};
};