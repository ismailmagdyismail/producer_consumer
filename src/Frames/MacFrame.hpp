#pragma once
// Dummy Frame strcuture to transfer data between Generator , Analyzer
// I don't wanna use smart pointers shut upPppppp
class MacFrame
{
public:
  MacFrame() = default;
  explicit MacFrame(MacFrame &&other) = delete;

  ~MacFrame()
  {
    delete m_pDestAdress;
    delete m_pSrcAddress;
    delete m_pPayload;
    delete m_pFCS;
  }
  char *m_pDestAdress{nullptr};
  char *m_pSrcAddress{nullptr};
  char *m_pPayload{nullptr};
  char *m_pFCS{nullptr};
};