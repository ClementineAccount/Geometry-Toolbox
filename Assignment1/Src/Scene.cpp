//
// Created by pushpak on 3/28/18.
//

#include "Scene.h"
#include <assert.h>

Scene::Scene() : _windowWidth(100), _windowHeight(100)
{

}

Scene::Scene(int windowWidth, int windowHeight, GeometryToolbox::GLApplication* parentApplicationPtr)
{
    _windowHeight = windowHeight;
    _windowWidth = windowWidth;
    parentApplicationPtr = parentApplicationPtr;
    assert(parentApplicationPtr != nullptr && "Scenes must belong to an application");
}

Scene::~Scene()
{
    CleanUp();
}

// Public methods

// Init: called once when the currScene is initialized
int Scene::Init()
{
    return -1;
}

// LoadAllShaders: This is the placeholder for loading the shader files
void Scene::LoadAllShaders()
{
    return;
}


// preRender : called to setup stuff prior to rendering the frame
int Scene::preRender()
{
    return -1;
}

// Render : per frame rendering of the currScene
int Scene::Render()
{
    return -1;
}

// postRender : Any updates to calculate after current frame
int Scene::postRender()
{
    return -1;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    return;
}

// Display : Per-frame execution of the currScene
int Scene::Display()
{
    preRender();

    Render();

    postRender();

    return -1;
}