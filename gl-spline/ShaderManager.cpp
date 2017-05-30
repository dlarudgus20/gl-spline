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
 * @file ShaderManager.cpp
 * @date 2015. 10. 26.
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "pch.h"
#include "ext.h"
#include "ShaderManager.h"

ShaderManager &ShaderManager::getInstance()
{
    static ShaderManager obj;
    return obj;
}

Shader &ShaderManager::getShader(int index)
{
    return m_arShader[index];
}

void ShaderManager::initialize()
{
    auto doCompile = [](Shader &shader, const char *vs, const char *fs, const char *name) {
        std::cout << "->compile shader (" << name << ")" << std::endl;
        shader.compile(vs, fs);
        std::cout << ">vertex shader info:\n" << shader.getVSInfoString() << std::endl;
        std::cout << ">fragment shader info:\n" << shader.getFSInfoString() << std::endl;
    };

    try
    {
        for (int i = 0; i < COUNT_SHADER; ++i)
        {
            std::string name = m_names[i];
            std::string vs = "shaders/" + name + ".vert";
            std::string fs = "shaders/" + name + ".frag";

            Shader& shader = m_arShader[i];

            std::cout << "->compile shader (" << name << ")" << std::endl;
            shader.compile(vs.c_str(), fs.c_str());
            std::cout << ">vertex shader info:\n" << shader.getVSInfoString() << std::endl;
            std::cout << ">fragment shader info:\n" << shader.getFSInfoString() << std::endl;
        }
    }
    catch (Shader::CompileError &e)
    {
        std::cerr << "##compile error##\n" << e.what() << std::endl;
        throw;
    }
    catch (Shader::LinkError &e)
    {
        std::cerr << "##link error##\n" << e.what() << std::endl;
        throw;
    }
}
