//! system includes
#include <iostream>

//! generator
#include "Generator/Generator.hpp"

int main()
{

    // bool bIsGeneratorDone = Generator.
    char *pDestAddress = new char[5]{"1111"};
    char *pSrcAddress = new char[5]{"FFFF"};
    uint32_t ui32FramesCount = 32;
    Generator oGenerator(DummyConfigurations(ui32FramesCount, pDestAddress, pSrcAddress));

    do
    {
        MacFrame frame;
        oGenerator.getPacket(frame);
        std::cout << "Frame:" << frame.m_pSrcAddress << '\n';
    } while (!oGenerator.isDone());
}
