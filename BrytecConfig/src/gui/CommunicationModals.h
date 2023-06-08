#pragma once

#include "communication/CanBusStream.h"

namespace Brytec {

class CommunicationModals {

public:
    void open();
    void draw();

private:
    void drawTable();
    void drawModuleCommands();
    void drawSendReceive();
    void drawProgressBar();
    void drawCloseButton();
    void callback(CanBusStreamCallbackData data);

private:
    bool m_open = false;
    uint8_t m_selectedModuleAddr = CanCommands::AllModules;
    uint8_t m_newAddress = 1;
    bool m_changeModuleAddress = true;
    CanBusStreamCallbackData m_callbackData = {};
};
}