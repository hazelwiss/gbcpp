#include<display/display.h>
#include<display/imgui_backends.h>
#include<display/debug_window.h>
#include<interpreter.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<thread>

std::atomic_bool main_window::enable_debug_window{false};
std::atomic_bool main_window::enable_display{false};
interpreter_t main_window::placeholder{};
std::atomic_bool main_window::request_handshake{false};
std::atomic_bool main_window::allow_handshake{false};

GLFWwindow* window;
std::thread thread;

void glfw_error_callback(int error, const char* description){
    std::cout << "application aborted with code: " << error << "and error string:";
    std::cout << description << std::endl;
    std::abort();
}

void main_window::threaded_loop(){
    const std::string glsl_version = "#version 130";
    glfwSetErrorCallback(glfw_error_callback);
    if(glfwInit() == GLFW_FALSE)
        throw std::runtime_error("error occured during initialization of glfw.");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(1920,1080,"sometitle",NULL,NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("error occured with glew.");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    while(true){
        if(enable_display){
            draw();
        }
    }
}

void main_window::init(){
    thread = std::thread(threaded_loop);
}

void main_window::bind(interpreter_t& interp){
    dbg_window::hook(interp);
}

void main_window::start(){
    enable_display = true;
}

void main_window::stop(){
    enable_display = false;
}

void main_window::on_pause(){
    dbg_window::on_pause();
}

void main_window::draw(){
    if(request_handshake){
        allow_handshake = true;
        while(request_handshake);
        allow_handshake = false;
    }
    if(!glfwWindowShouldClose(window)){
        //  poll events.
        glfwPollEvents();
        // starts the imgui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //  call on routines.
        draw_main_menu();
        if(enable_debug_window)
            dbg_window::draw();
        //  renders the image.
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    } else{  
        glfwDestroyWindow(window);
    }
}

void main_window::draw_main_menu(){
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("Debug")){
            ImGui::MenuItem("Debugger", nullptr, reinterpret_cast<bool*>(&enable_debug_window));
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
