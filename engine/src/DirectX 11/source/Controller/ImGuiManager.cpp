#include "ImGuiManager.h"


namespace engine::DX
{

    void ImGuiManager::Init(HWND hWnd) 
    {
        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // Setup ImGui style
        ImGui::StyleColorsDark();

        // Initialize the platform/renderer bindings
        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX11_Init(g_device, g_devcon);
    }



    void ImGuiManager::NewFrame() 
    {
        // Start a new ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

    }

    void ImGuiManager::Render() 
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }



    /// <param name="value">pass by value since imgui will handle only the variables that is subscribed to the imgui manager</param>
    void ImGuiManager::RenderSlider(const std::string& identifier, float& value, float min, float max) 
    {
        // wrapped all elements in a main menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Debug"))
            {
                float oldValue = value;
                if (ImGui::SliderFloat(identifier.c_str(), &value, min, max) && value != oldValue)
                {
                    for (const auto& callback : valueChangedCallbacks)
                    {
                        callback(identifier, value);
                    }
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        
    }



}

