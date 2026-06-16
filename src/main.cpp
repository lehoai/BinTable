#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cstdio>
#include "app/Application.h"
#include "IconsFontAwesome6.h"

// Rebuild the style from scratch and re-apply DPI scaling, so the UI matches whichever
// monitor the window currently lives on (handles dragging the window between displays
// with different scale factors, e.g. a 2K and a 4K screen).
static void ApplyDpiScale(const float scale) {
    ImGuiStyle &style = ImGui::GetStyle();
    style = ImGuiStyle();
    ImGui::StyleColorsDark();

    // custom style
    // ImGuiStyle& guiStyle = ImGui::GetStyle();
    // guiStyle.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.20f, 0.25f, 1.0f);
    // guiStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.28f, 0.35f, 1.0f);

    style.FontSizeBase = 14.0f;
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
}

static void SetupFonts(const ImGuiIO &io) {
    ImGuiStyle &style = ImGui::GetStyle();
    style.FontSizeBase = 14.0f;

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
    ImFont *icons = io.Fonts->AddFontFromFileTTF("assets/fonts/FontAwesome6-Solid.otf", 10.0f, &cfg, iconRanges);
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

    // Main loop
    bool done = false;
    while (!done) {
        // WaitEvent blocks until an OS event arrives.  After processing real
        // input we push one SDL_EVENT_USER so ImGui gets a second frame to
        // reflect state changes (popup close, button press, etc.).  We do NOT
        // push another event when the frame was itself triggered by that
        // sentinel, so the loop actually blocks when the UI is idle.
        SDL_Event event;
        SDL_WaitEvent(&event);

        bool hadRealEvent = false;
        do {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_EVENT_WINDOW_DISPLAY_CHANGED) {
                ApplyDpiScale(SDL_GetWindowDisplayScale(window));
            }
            if (event.type != SDL_EVENT_USER)
                hadRealEvent = true;
        } while (SDL_PollEvent(&event));

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

        // Push a follow-up frame only when real input arrived this iteration,
        // so ImGui can finalise state changes.  Skipping this for the sentinel
        // frame itself breaks the render loop and lets WaitEvent block.
        if (hadRealEvent) {
            SDL_Event wake;
            SDL_zero(wake);
            wake.type = SDL_EVENT_USER;
            SDL_PushEvent(&wake);
        }
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
