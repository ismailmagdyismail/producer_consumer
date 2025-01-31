#pragma once
// Dummy Frame strcuture to transfer data between Generator , Analyzer
// I don't wanna use smart pointers shut upPppppp
class MacFrame
{
public:
  MacFrame() = default;
  explicit MacFrame(MacFrame &&other) = delete;

  const char *toHEX()
  {
    if (nullptr == m_pDestAdress || nullptr == m_pSrcAddress || nullptr == m_pPayload || nullptr == m_pFCS)
    {
      return new char[1]{'\0'};
    }
    size_t tSize = strlen(m_pDestAdress) + strlen(m_pSrcAddress) + strlen(m_pPayload) + strlen(m_pFCS);
    char *pHEX = new char[tSize + 1];
    strcpy(pHEX, m_pDestAdress);
    strcat(pHEX, m_pSrcAddress);
    strcat(pHEX, m_pPayload);
    strcat(pHEX, m_pFCS);
    return pHEX;
  }

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