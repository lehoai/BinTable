#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cstdio>
#include "app/Application.h"
#include "IconsFontAwesome6.h"
#include "imgui_freetype.h"

// Rebuild the style from scratch and re-apply DPI scaling, so the UI matches whichever
// monitor the window currently lives on (handles dragging the window between displays
// with different scale factors, e.g. a 2K and a 4K screen).
static void ApplyDpiScale(const float scale) {
    ImGuiStyle &style = ImGui::GetStyle();
    style = ImGuiStyle();
    ImGui::StyleColorsDark();

    // custom style
    ImGuiStyle& guiStyle = ImGui::GetStyle();
    guiStyle.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0);
    // guiStyle.Colors[ImGuiCol_Button] = ImVec4(0,0,0,0);
    // guiStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.13f, 0.17f, 1.0f);

    style.FontSizeBase = 14.0f;
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
}

static void SetupFonts(const ImGuiIO &io) {
    ImGuiStyle &style = ImGui::GetStyle();
    style.FontSizeBase = 14.0f;

    io.Fonts->FontLoaderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;

    // main font
    const ImFont *primary = io.Fonts->AddFontFromFileTTF("assets/fonts/DejaVuSansMono.ttf");
    IM_ASSERT(primary != nullptr && "DejaVu font not found");

    ImFontConfig cfg;
    cfg.MergeMode = true;
    ImFont *jp = io.Fonts->AddFontFromFileTTF("assets/fonts/NotoMonoJP-subset.ttf", 0.0f, &cfg);
    IM_ASSERT(jp != nullptr && "Noto font not found");

    cfg.MergeMode = true;
    cfg.PixelSnapH = true;
    cfg.GlyphOffset = ImVec2(0.0f, 0.0f);
    static constexpr ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    const ImFont *icons = io.Fonts->AddFontFromFileTTF("assets/fonts/FontAwesome6-Solid.otf", 0.0f, &cfg, iconRanges);
    IM_ASSERT(icons != nullptr && "FontAwesome font not found");


}

int main(int, char **) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    // GL 3.3 + GLSL 330
    const auto glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window with graphics context
    SDL_WindowFlags window_flags =
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window *window = SDL_CreateWindow("BinTable", 1280, 800, window_flags);
    if (window == nullptr)
        return 1;

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);
    // SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SetupFonts(io);

    // Scale UI element sizes and font rasterization for the DPI of the monitor the window
    // actually opened on (e.g. 1.0x on a 2K@100% screen, 1.5x on a 4K@150% screen).
    // FontScaleDpi >= ~1.15 (13px * scale >= 15px) makes ImGui pick the scalable ProggyForever
    // default font instead of the fixed-size ProggyClean bitmap, so text stays crisp at larger
    // sizes while keeping the same distinctive look. The content-scale callback keeps this in
    // sync if the window is later dragged to a display with a different scale factor.
    // NOTE: must be called after ImGui_ImplGlfw_InitForOpenGL so the window->context map is populated.
    ApplyDpiScale(SDL_GetWindowDisplayScale(window));

    app::Application application;

    // Number of frames still owed before the loop is allowed to block again.
    // ImGui settles some widgets over several frames (tab bars only lay out
    // their strip the frame after the tabs are submitted, auto-resize windows
    // and stretch tables behave the same), so after any input - and at startup -
    // we render a short burst back-to-back, then go back to blocking on
    // SDL_WaitEvent once the UI is idle. This keeps the app at ~0% CPU when idle
    // while guaranteeing nested tab bars get the frames they need to appear.
    constexpr int kSettleFrames = 3;
    int framesToRender = kSettleFrames;

    bool done = false;

    auto handleEvent = [&](const SDL_Event &event) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            done = true;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            done = true;
        if (event.type == SDL_EVENT_WINDOW_DISPLAY_CHANGED)
            ApplyDpiScale(SDL_GetWindowDisplayScale(window));
        framesToRender = kSettleFrames;
    };

    // Main loop
    while (!done) {
        // Block only when the UI has settled (no frames owed). Any event resets
        // the counter so we render a fresh burst.
        if (framesToRender <= 0) {
            SDL_Event event;
            SDL_WaitEvent(&event);
            handleEvent(event);
        }

        // Drain anything else already queued (mouse moves, resizes, etc.).
        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(event);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        application.RenderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        SDL_GetWindowSizeInPixels(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        --framesToRender;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
