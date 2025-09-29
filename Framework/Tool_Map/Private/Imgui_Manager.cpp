#include "pch.h"
#include "Imgui_Manager.h"
#include <fstream>

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    ImFont* font = nullptr; // imgui용 폰트
    string font_file = "../Bin/Resources/ImGuiFonts/NanumGothic.ttf";

    ifstream ifile = {};
    ifile.open(font_file);
    if (ifile)
    {
        font = io.Fonts->AddFontFromFileTTF(font_file.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
    }
    else
    {
        MSG_BOX("No Font file");
        font = io.Fonts->AddFontDefault();
    }
    IM_ASSERT(font != NULL);

    return S_OK;
}

void CImgui_Manager::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = false;  // ImGui가 마우스 입력을 독점하지 않도록 설정!
    ImGuizmo::SetOrthographic(false); // 직교할거니?
    ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

    ImGuizmo::BeginFrame(); // 이게 임구이비긴?인듯?

    _float fWidth = (_float)g_iWindowSizeX;
    _float fHeight = (_float)g_iWindowSizeY;

    RECT Rect = { 0, 0, 0, 0 };
    ::GetClientRect(g_hWnd, &Rect);
    
    //도킹아닐때
    //ImVec2 ImvWindowPos = ImGui::GetWindowPos();
    //ImVec2 ImvWindowSize = ImGui::GetWindowSize();
    //ImGuizmo::SetRect(ImvWindowPos.x, ImvWindowPos.y, ImvWindowSize.x, ImvWindowSize.y);

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());  // 뷰포트의 

    //const ImGuiViewport* viewport = ImGui::GetMainViewport();
    //ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
    //ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, fWidth, fHeight);
    


    //static bool show_demo_window { false };
    //static bool show_another_window { false };
    //static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    //// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    //{
    //    static float f = 0.0f;
    //    static int counter = 0;

    //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    //    ImGui::Checkbox("Another Window", &show_another_window);

    //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //        counter++;
    //    ImGui::SameLine();
    //    ImGui::Text("counter = %d", counter);

    //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    //    ImGui::End();
    //}

    //// 3. Show another simple window.
    //if (show_another_window)
    //{
    //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //    ImGui::Text("Hello from another window!");
    //    if (ImGui::Button("Close Me"))
    //        show_another_window = false;
    //    ImGui::End();
    //}


}

void CImgui_Manager::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // Start the Dear ImGui frame

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void CImgui_Manager::Release()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
