#include "SerialWindow.h"

#include "BrytecConfigEmbedded/Deserializer/BinaryBufferSerializer.h"
#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include "imgui.h"
#include <exception>
#include <iostream>
#include <string>

static void run(bool& run, serial::Serial& serial, std::mutex& mutex, std::map<uint32_t, Brytec::CanExtFrame>& vec)
{
    while (run) {

        size_t readLenth;
        if (serial.isOpen()) {

            static uint8_t buffer[64];
            // readLenth = serial.read(buffer, 64);

            try {
                readLenth = serial.read(buffer, 64);
            } catch (std::exception& e) {
                std::cerr << "Unhandled Exception: " << e.what() << std::endl;
                serial.close();
                run = false;
            }

            if (readLenth > 0) {
                Brytec::CanExtFrame frame;
                uint8_t size = 0;

                Brytec::BinaryDeserializer des;
                des.setData(buffer, 64);

                des.readRaw<uint32_t>(&frame.id);
                des.readRaw<uint8_t>(&frame.dlc);
                des.readRaw<uint8_t>(&frame.data[0]);
                des.readRaw<uint8_t>(&frame.data[1]);
                des.readRaw<uint8_t>(&frame.data[2]);
                des.readRaw<uint8_t>(&frame.data[3]);
                des.readRaw<uint8_t>(&frame.data[4]);
                des.readRaw<uint8_t>(&frame.data[5]);
                des.readRaw<uint8_t>(&frame.data[6]);
                des.readRaw<uint8_t>(&frame.data[7]);

                mutex.lock();
                vec[frame.id] = frame;
                mutex.unlock();
            }
        }
    }

    serial.close();
}

SerialWindow::~SerialWindow()
{
    m_runThread = false;
    if (m_thread.joinable())
        m_thread.join();
}

void SerialWindow::drawWindow()
{
    if (!m_opened)
        return;

    ImGui::Begin("Serial", &m_opened);

    if (m_serial.isOpen()) {

        ImGui::Text("Connected to: %s", m_selectedDevice.description.c_str());

        if (ImGui::Button("close serial")) {
            m_runThread = false;
            m_thread.join();
        }
    } else {

        m_devices = serial::list_ports();

        // std::find(m_devices.begin(), m_devices.end(), );
        auto ret = std::find_if(m_devices.begin(), m_devices.end(), [this](serial::PortInfo info) { return info.description == m_selectedDevice.description; });
        if (ret == m_devices.end())
            m_selectedDevice.description = "";

        if (ImGui::BeginCombo("##Serial Ports", m_selectedDevice.description.c_str(), ImGuiComboFlags_HeightLarge)) {
            for (auto& device : m_devices) {
                if (ImGui::Selectable(device.description.c_str(), device.port == m_selectedDevice.port)) {
                    m_selectedDevice = device;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("open serial")) {
            if (!m_selectedDevice.port.empty()) {
                m_serial.setPort(m_selectedDevice.port);
                serial::Timeout timeout = serial::Timeout::simpleTimeout(100);
                m_serial.setTimeout(timeout);
                m_serial.setBaudrate(115200);

                try {
                    m_serial.open();
                } catch (std::exception& e) {
                    std::cerr << "Unhandled Exception: " << e.what() << std::endl;
                    m_selectedDevice.description = "";
                }

                if (!m_runThread && m_thread.joinable())
                    m_thread.join();

                if (m_serial.isOpen()) {
                    m_runThread = true;
                    m_thread = std::thread(run, std::ref(m_runThread), std::ref(m_serial), std::ref(m_mutex), std::ref(m_map));
                }
            }
        }
    }

    ImGui::Separator();

    m_mutex.lock();
    for (auto& m : m_map) {
        auto& f = m.second;
        ImGui::Text("id: %d dlc: %d data: %d %d %d %d %d %d %d %d", f.id, f.dlc,
            f.data[0],
            f.data[1],
            f.data[2],
            f.data[3],
            f.data[4],
            f.data[5],
            f.data[6],
            f.data[7]);
    }
    m_mutex.unlock();

    ImGui::End();
}
