#include "ArxWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <fstream>
#include <sstream>
ARX_NAMESPACE_BEGIN


ArxWindow::ArxWindow(std::string_view title, Size size, Position position, int attributes)
    : UIObject(nullptr, ArxWindow::GetRectSize() < size ? size : ArxWindow::GetRectSize(), position)
    , m_title(title)
{
    GLFWwindow *win = glfwCreateWindow(ArxWindow::GetSize().width, ArxWindow::GetSize().height, title.data(), nullptr, nullptr);
    SetGLFWwindow(win);
    SetWindowAttributes(attributes);
    glfwSetWindowUserPointer(GetGLFWwindow(), this);
    glfwSetWindowPos(GetGLFWwindow(), position.x, position.y);
    InitGlfwCallbacks();
    //if (parent == nullptr)
    //TODO currently non TopLevelWindows aren't supported
    ArxTheApp->GetTopLevelWindowManager().AddTopLevelWindow(this);

    GetEventHandler().Bind<CloseEvent>(std::function<void(CloseEvent&)>([](CloseEvent&e) {
        ArxWindow *arxWin = dynamic_cast<ArxWindow*>(e.GetCallingObject());
        /*if (arxWin->GetParent())
            arxWin->SetParent(nullptr);
        else*/
        
        ArxTheApp->GetTopLevelWindowManager().RemoveTopLevelWindow(arxWin);
        e.Skip();
    }));

}

void ArxWindow::InitGlfwCallbacks()
{
    glfwSetWindowCloseCallback(GetGLFWwindow(), &ArxWindow::close_callback); 
    glfwSetWindowFocusCallback(GetGLFWwindow(), &ArxWindow::focus_callback);
    glfwSetWindowSizeCallback(GetGLFWwindow(),  &ArxWindow::size_callback);
    glfwSetWindowPosCallback(GetGLFWwindow(), &ArxWindow::position_callback);
}

/*static*/ void ArxWindow::close_callback(GLFWwindow *win)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->Destroy();
}

/*static*/ void ArxWindow::focus_callback(GLFWwindow *win, int focused)
{
    std::cout << "fokus" << '\n';
    if (focused)
    {
std::cout << "fokus got" << '\n';
        glfwMakeContextCurrent(win);
        ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
        glViewport(0, 0, arxWin->GetSize().width, arxWin->GetSize().height);
    }
}

/*static*/ void ArxWindow::size_callback(GLFWwindow *win, int width, int height)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->UIObject::SetSize(Size{ width, height });
    glViewport(0, 0, arxWin->GetSize().width, arxWin->GetSize().height);
}

/*static*/ void ArxWindow::position_callback(GLFWwindow *win, int x, int y)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->UIObject::SetPosition(Position{ x, y });
}

void ArxWindow::SetPosition(const Position &pos)
{
    glfwSetWindowPos(GetGLFWwindow(), pos.x, pos.y);
}

Size ArxWindow::GetRectSize() 
{
    return Size{ 1, 1 };
}


FT_Library GetLibrary()
{
    static FT_Library library;
    static std::once_flag onceFlag;
    std::call_once(onceFlag, FT_Init_FreeType, &library);

    return library;
}

FT_Face GetHackFace()
{
    static FT_Face face;
    static std::once_flag onceFlag;
    std::call_once(onceFlag, FT_New_Face, GetLibrary(), "/usr/share/fonts/TTF/Hack-Bold.ttf", 0, &face);
    FT_Set_Pixel_Sizes(face, 0, 48);
    
    return face;
}

unsigned int CreateShaderProgram()
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open("test_data/font_shader.vert");
        std::stringstream fileContentStream;
        fileContentStream << file.rdbuf();
        file.close();
        std::string fileContent = fileContentStream.str(); 
        const char *fileContentPtr = fileContent.c_str();
        glShaderSource(vertexShader, 1, &fileContentPtr, NULL);
        glCompileShader(vertexShader);
        
        int status = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        assert(status == GL_TRUE && "vert");
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open("test_data/font_shader.frag");
        std::stringstream fileContentStream;
        fileContentStream << file.rdbuf();
        file.close();
        std::string fileContent = fileContentStream.str(); 
        const char *fileContentPtr = fileContent.c_str();
        glShaderSource(fragmentShader, 1, &fileContentPtr, NULL);
        glCompileShader(fragmentShader);

        int status = 0;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
        assert(status == GL_TRUE && "frag");
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int status = 0;
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE && "prog");

    return shaderProgram;
}

unsigned int GetTestShaderProgram()
{
    static unsigned int shaderProgram = CreateShaderProgram();
    return shaderProgram;
}

unsigned int CreateRectVAO()
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    float rectData[] = {
        -0.8f,  0.8f, 1.0f, 1.0f,
        -0.8f, -0.8f, 1.0f, 0.0f,
         0.8f, -0.8f, 0.0f, 0.0f,
         0.8f, -0.8f, 0.0f, 0.0f,
         0.8f,  0.8f, 0.0f, 1.0f,
        -0.8f,  0.8f, 1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(rectData), rectData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR && "vbo + vao");

    assert(vao != 0);

    return vao;
}

unsigned int GetRectVAO()
{
    static unsigned int vao = CreateRectVAO();
    return vao;
}

unsigned int CreateLetterTexture()
{
    FT_Face face = GetHackFace();
    FT_UInt glyphIndex = FT_Get_Char_Index(face, 0x00000039);
    assert(glyphIndex != 0);
    FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    unsigned int tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    assert(glGetError() == GL_NO_ERROR && "tex");
    assert(face->glyph->bitmap.pixel_mode == ft_pixel_mode_mono);
    return tex;
}

unsigned int GetLetterTexture()
{
    static unsigned int tex = CreateLetterTexture();
    return tex;
}

void ArxWindow::Draw()
{
    glm::vec3 normalizedColor = GetBackgroundColor().GetNormalizedRGB(); 
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (UIObject *obj : GetChildren())
        if (dynamic_cast<ArxWindow*>(obj) == nullptr)
            obj->Draw();
    
    unsigned int vao = GetRectVAO();
    glBindVertexArray(vao);
    unsigned int program = GetTestShaderProgram();
    glUseProgram(program);
    unsigned int tex = GetLetterTexture();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    assert(glGetError() == GL_NO_ERROR && "drawing");
    glfwSwapBuffers(GetGLFWwindow());
}

void ArxWindow::SetMinSize(const Size &s)
{
    UIObject::SetMinSize(s);
    glfwSetWindowSizeLimits(GetGLFWwindow(), GetMinSize().width, GetMinSize().height, GetMaxSize().width, GetMaxSize().height);
}

void ArxWindow::SetMaxSize(const Size &s)
{
    UIObject::SetMaxSize(s);
    glfwSetWindowSizeLimits(GetGLFWwindow(), GetMinSize().width, GetMinSize().height, GetMaxSize().width, GetMaxSize().height);
}

void ArxWindow::SetSize(const Size &s)
{
    glfwSetWindowSize(GetGLFWwindow(), s.width, s.height);
}

void ArxWindow::Show(bool visible)
{
    (visible) ? glfwShowWindow(GetGLFWwindow()) : glfwHideWindow(GetGLFWwindow());
}

void ArxWindow::SetFocus(bool focus)
{
    if (focus)
        glfwRequestWindowAttention(GetGLFWwindow());
}

void ArxWindow::SetWindowAttributes(int attributes)
{
    glfwSetWindowAttrib(GetGLFWwindow(), GLFW_RESIZABLE, !!(attributes & WindowAttributes::RESIZABLE));
    glfwSetWindowAttrib(GetGLFWwindow(), GLFW_DECORATED, !!(attributes & WindowAttributes::DECORATED));
    glfwSetWindowAttrib(GetGLFWwindow(), GLFW_FLOATING,  !!(attributes & WindowAttributes::DISPLAY_ON_TOP));
}

std::string_view ArxWindow::GetTitle()
{
    return m_title;
}

void ArxWindow::SetTitle(std::string_view title)
{
    m_title = title;
    glfwSetWindowTitle(GetGLFWwindow(), GetTitle().data());
}

GLFWwindow *ArxWindow::GetGLFWwindow()
{
    return m_win.get();
}

void ArxWindow::SetGLFWwindow(::GLFWwindow *window)
{
    m_win = decltype(m_win)(window, &glfwDestroyWindow);
}

ARX_NAMESPACE_END
