#include "includes.hpp"
#include "CumuloServer.hpp"

// Data
static ID3D10Device* g_pd3dDevice = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D10RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void SetupCirrusLight();
void SetupCirrusDark();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ReloadWindowEnumProc(HWND hWnd, LPARAM lParam);
HudWindowManager* registry;

void hideConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);
}

void showConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_SHOW);
}


// Main code
int main(int argc, char* argv[])
{
    hideConsole();
    

    ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("Cirrus HUD Container"), nullptr };
    ::RegisterClassEx(&wc);
    const wchar_t CLASS_NAME[] = L"Cirrus HUD Container";

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();

    GetWindowRect(hDesktop, &desktop);

    HWND hwnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED, CLASS_NAME, L"Cirrus HUD Container", WS_POPUP, 0, 0, desktop.right, desktop.bottom, NULL, NULL, wc.hInstance, NULL);

    COLORREF color = 0;
    BYTE alpha = 255;
    SetLayeredWindowAttributes(hwnd, color, alpha, LWA_ALPHA);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    SetupCirrusDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX10_Init(g_pd3dDevice);
    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    bool dragWindow = false;
    ImVec2 windowPos;

    ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    InputHelper* input = new InputHelper();
    TimeKeeper* timeKeeper = new TimeKeeper();
    registry = new HudWindowManager(input, exStyle, hwnd, timeKeeper);

    registry->slotMode = false;

    registry->initLua();


    bool done = false;
    if (argc > 1) {
        // The URL should be in argv[1]
        std::string url = argv[1];

        bool downloadSoftware = false;

        // Alternate Main Loop
        while (!done)
        {
            input->update();
            SetForegroundWindow(hwnd);



            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    done = true;
            }
            if (done)
                break;

            if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
                g_ResizeWidth = g_ResizeHeight = 0;
                CreateRenderTarget();
            }

            // Start the Dear ImGui frame
            ImGui_ImplDX10_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Confirm?", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            if (ImGui::IsWindowHovered())
            {
                exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
                SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
            }
            else {
                exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
                SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
            }

            ImGui::Text("Are you sure you want to download this addon?");

            if (ImGui::Button("Yes")) {
                downloadSoftware = true;
                done = true;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel")) {
                done = true;
            }

            ImGui::End();

            ImGui::Render();
            const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
            g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(1, 0);
        }

        delete registry;

        ImGui_ImplDX10_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        showConsole();

        manageURLProtocolRequest(url);



        EnumWindows(ReloadWindowEnumProc, 0);

        return 0;
    }

    ImFont* customFont = io.Fonts->AddFontFromFileTTF("./K2D-Thin.ttf", 16.0f);

    if (!customFont) {
        std::cout << "no worky" << std::endl;
        return -1;
    }

    init_url_protocol();
    initCumuloServer();

    // Main loop
    while (!done)
    {
        input->update();

        HWND foregroundWindow = GetForegroundWindow();
        if (foregroundWindow != hwnd) {
            SetActiveWindow(hwnd);
        }


        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX10_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::PushFont(customFont);

        ImGui::PopFont();

        


        timeKeeper->update();
        registry->renderAll();

        if (input->isKeyPressed(VK_CONTROL)) {
            if (input->wasKeyPressed(0x52)) {
                registry->initLua();
            }
        }

        if (input->isKeyPressed(VK_LMENU)) {
            if (input->isKeyPressed(VK_CONTROL)) {
                if (input->wasKeyPressed('H')) {
                    registry->scaredMode = !registry->scaredMode;
                }
            }
            
        }

        if (input->isKeyPressed(VK_LMENU)) {
            if (input->isKeyPressed(VK_CONTROL)) {
                if (input->wasKeyPressed('S')) {
                    registry->slotMode = !registry->slotMode;
                }
            }

        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    delete registry;

    ImGui_ImplDX10_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    endCumuloServer();

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
    HRESULT res = D3D10CreateDeviceAndSwapChain(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D10CreateDeviceAndSwapChain(nullptr, D3D10_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D10Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;

    case WM_COPYDATA: {
        PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
        if (pCDS->cbData == 32) { // Check if the data size is 32 bytes
            char* message = (char*)pCDS->lpData;
            
            if (std::string(message) == std::string("reload")) {
                registry->initLua();
            }
        }
        break;
    }
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK ReloadWindowEnumProc(HWND hWnd, LPARAM lParam) {
    const int length = GetWindowTextLength(hWnd) + 1;
    wchar_t* buffer = new wchar_t[length];

    GetWindowText(hWnd, buffer, length);

    // Check if the window title is "Cirrus HUD Container"
    if (std::wstring(buffer) == L"Cirrus HUD Container") {
        COPYDATASTRUCT cds;
        char message[32] = "reload";
        cds.dwData = 1; // Can be anything
        cds.lpData = message;
        cds.cbData = 32; // Size of the message

        SendMessage(hWnd, WM_COPYDATA, (WPARAM)(HWND)hWnd, (LPARAM)(LPVOID)&cds);
    }


    delete[] buffer;
    return TRUE;
}

void SetupCirrusLight()
{
    // Cirrus style from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.007843137718737125f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.9999899864196777f, 0.9999899864196777f, 0.9399999976158142f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.5965665578842163f, 0.5965605974197388f, 0.5965605974197388f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(1.0f, 0.9999899864196777f, 0.9999899864196777f, 0.9399999976158142f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 0.4549019634723663f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 0.454935610294342f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 0.4549019634723663f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.9999899864196777f, 0.9999899864196777f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9999899864196777f, 0.9999940395355225f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.5064377784729004f, 0.5064327120780945f, 0.5064327120780945f, 0.5099999904632568f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.8326179981231689f, 0.8326096534729004f, 0.8326096534729004f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9999899864196777f, 0.9999945759773254f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.6930752396583557f, 0.5104164481163025f, 0.729613721370697f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 0.5193133354187012f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.8369098901748657f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.6652360558509827f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4706293940544128f, 0.4706326723098755f, 0.6266094446182251f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.7289553880691528f, 0.5640921592712402f, 0.7467811107635498f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.8047022819519043f, 0.7450127601623535f, 0.8111587762832642f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.6497769355773926f, 0.5861960649490356f, 0.6566523313522339f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.7939302325248718f, 0.5949639678001404f, 0.8154506683349609f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9999990463256836f, 0.9999899864196777f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.5137255191802979f, 0.3529411852359772f, 0.4233275055885315f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void SetupCirrusDark()
{
    // Cirrus Dark style from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.9999899864196777f, 0.9999973177909851f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1637164205312729f, 0.180317223072052f, 0.1888412237167358f, 0.9399999976158142f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.5965665578842163f, 0.5965605974197388f, 0.5965605974197388f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1647058874368668f, 0.1803921610116959f, 0.1882352977991104f, 0.9411764740943909f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 0.4549019634723663f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 0.454935610294342f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 0.4549019634723663f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2282230257987976f, 0.2502303123474121f, 0.3004291653633118f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1199138164520264f, 0.1321801841259003f, 0.1502146124839783f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.5064377784729004f, 0.5064327120780945f, 0.5064327120780945f, 0.5099999904632568f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.159756138920784f, 0.1706055998802185f, 0.2103004455566406f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9999899864196777f, 0.9999945759773254f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.6930752396583557f, 0.5104164481163025f, 0.729613721370697f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 0.5193133354187012f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.8369098901748657f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.6652360558509827f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4706293940544128f, 0.4706326723098755f, 0.6266094446182251f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.7289553880691528f, 0.5640921592712402f, 0.7467811107635498f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.4033823609352112f, 0.3236014544963837f, 0.4120171666145325f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.6497769355773926f, 0.5861960649490356f, 0.6566523313522339f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.7939302325248718f, 0.5949639678001404f, 0.8154506683349609f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9999990463256836f, 0.9999899864196777f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.4980392158031464f, 0.3529411852359772f, 0.5137255191802979f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.5137255191802979f, 0.3529411852359772f, 0.4233275055885315f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.6313725709915161f, 0.5333333611488342f, 0.6509804129600525f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}