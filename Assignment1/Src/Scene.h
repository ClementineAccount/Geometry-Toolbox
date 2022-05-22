//
// Created by pushpak on 3/28/18.
//

#ifndef SAMPLE3_2_FBO_SCENE_H
#define SAMPLE3_2_FBO_SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include <nanogui/nanogui.h>

//#include "SceneManager.h"
//#include "VertexDataManager.h"

#include "GLApplication.h"

#define _GET_GL_ERROR   { GLenum err = glGetError(); std::cout << "[OpenGL Error] " << glewGetErrorString(err) << std::endl; }

class Scene
{

public:
    Scene();
    Scene( int windowWidth, int windowHeight, GeometryToolbox::GLApplication* parentApplicationPtr );
    virtual ~Scene();

    // Public methods

    // Init: called once when the currScene is initialized
    virtual int Init();

    // LoadAllShaders: This is the placeholder for loading the shader files
    virtual void LoadAllShaders();

    // Display : encapsulates per-frame behavior of the currScene
    virtual int Display();

    // preRender : called to setup stuff prior to rendering the frame
    virtual int preRender();

    // Render : per frame rendering of the currScene
    virtual int Render();

    // postRender : Any updates to calculate after current frame
    virtual int postRender();

    // cleanup before destruction
    virtual void CleanUp();

    // NanoGUI stuff
    virtual void SetupNanoGUI(GLFWwindow *pWwindow) = 0;
    //    virtual void CleanupNanoGUI(GLFWwindow *pWwindow, const nanogui::FormHelper &screen) = 0;


    //To Do: Refactor better design pattern than passing in a ptr to the application
    // This represents a reference to the application this scene currently 'lives in' but it should be refactored to a class or struct
    GeometryToolbox::GLApplication* parentApplicationPtr;

protected:
    int _windowHeight, _windowWidth;


};


#endif //SAMPLE3_2_FBO_SCENE_H
