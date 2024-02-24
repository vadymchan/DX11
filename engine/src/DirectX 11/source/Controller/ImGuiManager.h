#pragma once


#include <vector>
#include <string>
#include <functional>
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "../../window/WindowDX.h"
#include "../D3D/D3D.h"

namespace engine::DX
{



    class ImGuiManager
    {
    public:
        static ImGuiManager& getInstance()
        {
            static ImGuiManager instance;
            return instance;
        }

        using ValueChangedCallback = std::function<void(const std::string&, float)>;

        void AddValueChangedCallback(ValueChangedCallback callback) {
            valueChangedCallbacks.push_back(callback);
        }

        void Init(HWND hWnd);

        void NewFrame();
        void Render();

        void RenderSlider(const std::string& identifier, float& value, float min, float max);
        void RenderCheckbox(const std::string& identifier, bool& value);


    private:
        ImGuiManager() = default;
        ImGuiManager(const ImGuiManager&) = delete;
        ImGuiManager& operator=(const ImGuiManager&) = delete;


        std::hash<std::string> m_hashFunc;

        ~ImGuiManager()
        {
            // Cleanup platform/renderer bindings
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();

            // Cleanup ImGui context
            ImGui::DestroyContext();
        }

        std::vector<ValueChangedCallback> valueChangedCallbacks;
    };

}
