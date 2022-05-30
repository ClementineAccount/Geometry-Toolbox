#include "GLApplication.h"
#include "AssignmentOne.h"
#include <memory>
#include <iostream>

int mainAssignmentOne()
{
    GeometryToolbox::GLApplication app;
    app.initApplication();

    AssignmentOne::setApplicationPtr(app);

    AssignmentOne::InitAssignment();

    app.updateFunctions.emplace_back(AssignmentOne::UpdateAssignment);
    app.updateFunctions.emplace_back(AssignmentOne::RenderAssignment);


    app.updateApplication(); //has an update loop
    app.shutdownApplication();
    return 0;
}

//can have argc
int main(int argc, char* argv[])
{

    mainAssignmentOne();
    return 0;
}