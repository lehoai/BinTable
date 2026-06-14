#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#include <GLFW/glfw3.h>

#include "app/Application.h"

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Rebuild the style from scratch and re-apply DPI scaling, so the UI matches whichever
// monitor the window currently lives on (handles dragging the window between displays
// with different scale factors, e.g. a 2K and a 4K screen).
static void ApplyDpiScale(const float scale) {
    ImGuiStyle &style = ImGui::GetStyle();
    style = ImGuiStyle();
    ImGui::StyleColorsDark();

    // custom style
    // ImGuiStyle& guiStyle = ImGui::GetStyle();
    // guiStyle.Colors[ImGuiCol_Button] = ImVec4(0.137, 0.149, 0.176, 1.0);

    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
}

static void glfw_content_scale_callback(GLFWwindow *, float xscale, float) {
    ApplyDpiScale(xscale);
}

static void SetupFonts(const ImGuiIO &io) {
    static ImVector<ImWchar> mainRanges;
    ImFontGlyphRangesBuilder b;
    b.AddRanges(io.Fonts->GetGlyphRangesDefault());
    b.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    b.BuildRanges(&mainRanges);

    const ImFont* primary = io.Fonts->AddFontFromFileTTF("assets/fonts/DejaVuSansMono.ttf", 14.0f, nullptr, mainRanges.Data);
    IM_ASSERT(primary != nullptr && "DejaVu font not found");

    ImFontConfig cfg;
    cfg.MergeMode = true;
    const ImFont* jp = io.Fonts->AddFontFromFileTTF("assets/fonts/NotoMonoJP-subset.ttf", 18.0f, &cfg, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(jp != nullptr && "Noto font not found");

    io.Fonts->Build();
}

int main(int, char **) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.3 + GLSL 330
    const auto glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window with graphics context
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(1280, 800, "amgui", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SetupFonts(io);

    // Scale UI element sizes and font rasterization for the DPI of the monitor the window
    // actually opened on (e.g. 1.0x on a 2K@100% screen, 1.5x on a 4K@150% screen).
    // FontScaleDpi >= ~1.15 (13px * scale >= 15px) makes ImGui pick the scalable ProggyForever
    // default font instead of the fixed-size ProggyClean bitmap, so text stays crisp at larger
    // sizes while keeping the same distinctive look. The content-scale callback keeps this in
    // sync if the window is later dragged to a display with a different scale factor.
    // NOTE: must be called after ImGui_ImplGlfw_InitForOpenGL so the window->context map is populated.
    ApplyDpiScale(ImGui_ImplGlfw_GetContentScaleForWindow(window));
    glfwSetWindowContentScaleCallback(window, glfw_content_scale_callback);

    app::Application application;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // WaitEvents blocks until the next OS event, but ImGui often needs one
        // extra frame to reflect state changes (e.g. a popup closing after a
        // button click).  PostEmptyEvent at the end of the loop guarantees that
        // a second frame always follows immediately, so the UI never appears
        // "stuck" waiting for the user to wiggle the mouse.
        glfwWaitEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        application.RenderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // Ensure one more frame renders so that ImGui state changes made this
        // frame (popup close, button press, etc.) are actually displayed before
        // we block again on glfwWaitEvents.
        glfwPostEmptyEvent();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
