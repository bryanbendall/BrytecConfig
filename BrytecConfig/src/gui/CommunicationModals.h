#pragma once

#include "communication/CanBusStream.h"

namespace Brytec {

class CommunicationModals {

public:
    enum State {
        ModuleCommands,
        SendReceive
    };

public:
    void open(State state);
    void draw();

private:
    void drawTable(bool allModules);
    void drawModuleCommands();
    void drawSendReceive();
    void drawProgressBar();
    void drawCloseButton();
    void callback(CanBusStreamCallbackData data);

private:
    bool m_open = false;
    State m_state;
    uint8_t m_selectedModuleAddr = CanCommands::AllModules;
    uint8_t m_newAddress = 1;
    bool m_changeModuleAddress = true;
    CanBusStreamCallbackData m_callbackData = {};
};
}