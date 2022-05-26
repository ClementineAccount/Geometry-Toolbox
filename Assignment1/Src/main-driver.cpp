#include "GLApplication.h"

#include <memory>
#include <iostream>

#include "Scenes/SimpleScene_Quad.h"


void makeQuadScene(GeometryToolbox::GLApplication* ptr)
{
    //Example of Option #1 of scene creation
    std::shared_ptr<Scenes::SceneClass> scenePtr = std::make_shared<Scenes::SceneClass>(Scenes::SceneQuad::CreateColorQuadScene());

    //Why use container of functions instead of polymorphism? Because we can do this
    {
        using dataContainerType = Scenes::SceneClass::dataContainerType;
        Scenes::SceneClass::sceneFunctionType f_initScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
        {
            std::cout << "Creating a new function to an existing scene\n";
            return 0;
        };

        Scenes::SceneClass::sceneFunctionType f_initScene2 = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
        {
            std::cout << "Another one\n";
            return 0;
        };

        scenePtr.get()->startupFunctions.push_back(f_initScene);
        scenePtr.get()->startupFunctions.push_back(f_initScene2);
    }

    //Placeholder: just pass the parent application ptr directly
    std::static_pointer_cast<Scenes::SceneQuad::QuadData>(scenePtr.get()->sceneDataContainer[0]).get()->parentApplication
        = ptr;


    //To Do: SceneManager automatically init added scenes when suitable
    //Scenes::SceneFunctions::LoopFunctions(scenePtr.get()->startupFunctions, scenePtr.get()->sceneDataContainer, 0.0f);

    ptr->sm.addScene("Color Quad Scene", scenePtr);
    ptr->sm.initScenes();

}


//Intergration tests are necessary if we are using DearImgui or graphics stuff


//Testing just creating an application
int test0()
{
    GeometryToolbox::GLApplication app;
    app.initApplication();

    //To Do: Think of a test manager class or logger so I can have an output check
    if (app.window != nullptr)
    {
        std::cout << "Test 0: Window Created\n";
    }
    else
        std::cout << "Test 0: Window Creation Failed\n";

    app.shutdownApplication();
    return (app.window == nullptr);
}


int mainQuad()
{
    GeometryToolbox::GLApplication app;
    app.initApplication();

    makeQuadScene(&app);

    app.updateApplication(); //has an update loop
    app.shutdownApplication();
    return 0;
}

//can have argc
int main(int argc, char* argv[])
{
    if (argc == 1)
        test0();
    
    mainQuad();
    return 0;
}