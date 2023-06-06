#pragma once

#include "communication/CanBusStream.h"

namespace Brytec {

class CommunicationModals {

public:
    enum State {
        ChangeModuleState
    };

public:
    void open(State state);
    void draw();

private:
    void drawChangeModuleState();
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