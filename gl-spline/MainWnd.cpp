// Copyright (c) 2014, 임경현
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file MainWnd.cpp
 * @date 2015. 9. 17.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "pch.h"
#include "ext.h"
#include "MainWnd.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "ShadowMap.h"
#include "MainScene.h"

const int MAINWND_WIDTH = 800;
const int MAINWND_HEIGHT = 600;

std::unique_ptr<MainWnd> MainWnd::createInstance()
{
    return std::unique_ptr<MainWnd>(new MainWnd());
}

MainWnd::MainWnd()
    : m_wnd(nullptr)
    , m_width(MAINWND_WIDTH), m_height(MAINWND_HEIGHT)
{
    // TODO Auto-generated constructor stub
}

MainWnd::~MainWnd()
{
    // TODO Auto-generated destructor stub
}

bool MainWnd::create()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    m_wnd = glfwCreateWindow(MAINWND_WIDTH, MAINWND_HEIGHT, "pfps", nullptr, nullptr);
    if (m_wnd != nullptr)
    {
        glfwMakeContextCurrent(m_wnd);
        glfwSetWindowUserPointer(m_wnd, this);

        initCallback();
        return true;
    }
    else
    {
        return false;
    }
}

auto timeStr = [] (double time) {
    int s = static_cast<int>(time);
    char buf[11];
    int hour = s / 3600;
    int minutes = (s % 3600) / 60;
    int seconds = s % 60;
    snprintf(buf, 11, "[%02d:%02d:%02d]", hour, minutes, seconds);
    return std::string(buf);
};

void MainWnd::loop()
{
    static double startTime = glfwGetTime();
    static double frameTime = startTime, prevFrameTime;
    static double fpsResetTime = startTime + 1.0f;
    static int fps = 0;

    while (!glfwWindowShouldClose(m_wnd))
    {
        glfwPollEvents();
        render();
        fps++;
        idle();

        prevFrameTime = frameTime;
        frameTime = glfwGetTime();
        m_deltaTime = frameTime - prevFrameTime;

        if (fpsResetTime <= frameTime)
        {
            m_fps = fps;
            fps = 0;
            fpsResetTime += 1.0;
            std::cout << timeStr(frameTime) << " fps: " << m_fps << std::endl;
        }
    }
}

void MainWnd::loopManualFPS()
{
    const double expectedDelta = 1.0 / 60.0;

    static double startTime = glfwGetTime();

    static double frameTime = startTime, prevFrameTime;
    static double remain = 0.0f;
    static int fps = 0;

    static double fpsResetTime = startTime + 1.0;

    while (!glfwWindowShouldClose(m_wnd))
    {
        glfwPollEvents();

        if (remain <= 0.0f)
        {
            render();
            fps++;
        }

        idle();

        prevFrameTime = frameTime;
        frameTime = glfwGetTime();
        m_deltaTime = frameTime - prevFrameTime;

        if (fpsResetTime <= frameTime)
        {
            m_fps = fps;
            fps = 0;
            fpsResetTime += 1.0;
            std::cout << timeStr(frameTime) << " fps: " << m_fps << std::endl;
        }

        if (remain <= 0.0f)
        {
            remain += expectedDelta - m_deltaTime;
        }
        else
        {
            remain -= m_deltaTime;
        }
    }
}

void MainWnd::initCallback()
{
#define THIZ static_cast<MainWnd *>(glfwGetWindowUserPointer(wnd))
    glfwSetFramebufferSizeCallback(m_wnd, [] (GLFWwindow *wnd, int width, int height) {
        THIZ->onFrameBufferSize(width, height);
    });
    glfwSetWindowCloseCallback(m_wnd, [](GLFWwindow *wnd) {
        THIZ->onWindowClose();
    });
    glfwSetCursorPosCallback(m_wnd, [](GLFWwindow *wnd, double xpos, double ypos) {
        THIZ->onMouseCursorPos(xpos, ypos);
    });
#undef THIZ
}

bool MainWnd::initialize()
{
    try
    {
        glfwSetInputMode(m_wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        int width, height;
        glfwGetFramebufferSize(m_wnd, &width, &height);
        m_width = static_cast<GLsizei>(width);
        m_height = static_cast<GLsizei>(height);

        glViewport(0, 0, m_width, m_height);
        calcProjection();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_FRAMEBUFFER_SRGB);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        ShaderManager::getInstance().initialize();
        LightManager::getInstance().initialize();
        m_pShadowMap = ext::make_unique<ShadowMap>();
        m_pScene = ext::make_unique<MainScene>();

        return true;
    }
    catch (Shader::CompileError&)
    {
        return false;
    }
    catch (Shader::LinkError&)
    {
        return false;
    }
}

void MainWnd::render()
{
    m_pShadowMap->renderScene(m_pScene.get(), m_projection, m_camera.getMatrix(), m_width, m_height);

    glfwSwapBuffers(m_wnd);
}

void MainWnd::idle()
{
    const float unit = 2.5f;

    int front = 0, right = 0;
    front += glfwGetKey(m_wnd, GLFW_KEY_W) == GLFW_PRESS ? 1 : 0;
    front += glfwGetKey(m_wnd, GLFW_KEY_S) == GLFW_PRESS ? -1 : 0;
    right += glfwGetKey(m_wnd, GLFW_KEY_D) == GLFW_PRESS ? 1 : 0;
    right += glfwGetKey(m_wnd, GLFW_KEY_A) == GLFW_PRESS ? -1 : 0;

    m_camera.move(front, right, unit * static_cast<float>(m_deltaTime));
}

void MainWnd::calcProjection()
{
    float aspect = static_cast<float>(m_width) / m_height;

    m_projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void MainWnd::onMouseCursorPos(double xpos, double ypos)
{
    if (m_isFirstMouse)
    {
        m_prevMouseX = (float)xpos;
        m_prevMouseY = (float)ypos;
        m_isFirstMouse = false;
    }
    else
    {
        const float maxPitch = glm::radians(75.0f);

        const float unit = glm::radians(60.0f);

        float dx = (float)xpos - m_prevMouseX;
        float dy = -((float)ypos - m_prevMouseY);

        float pitch = m_camera.getPitch() + (dy / m_height) * unit;
        float yaw = m_camera.getYaw() + (dx / m_width) * unit;

        if (pitch > maxPitch)
            pitch = maxPitch;
        else if (pitch < -maxPitch)
            pitch = -maxPitch;

        m_camera.setPitchYaw(pitch, yaw);

        m_prevMouseX = (float)xpos;
        m_prevMouseY = (float)ypos;
    }
}

void MainWnd::onFrameBufferSize(int width, int height)
{
    // 최소화 상태일땐 width == height == 0임.
    if (!(width == 0 || height == 0))
    {
        m_width = static_cast<GLsizei>(width);
        m_height = static_cast<GLsizei>(height);

        glViewport(0, 0, width, height);
        calcProjection();
    }
}

void MainWnd::onWindowClose()
{
    // TODO: onWindowClose
}
