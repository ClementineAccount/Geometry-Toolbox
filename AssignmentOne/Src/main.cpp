#include "GLApplication.h"
#include "Assignment.h"
#include <memory>
#include <iostream>

int mainAssignmentOne()
{
    GeometryToolbox::GLApplication app;
    app.initApplication();

    Assignment::setApplicationPtr(app);
    Assignment::InitAssignment();

    app.updateFunctions.emplace_back(Assignment::UpdateAssignment);
    app.updateFunctions.emplace_back(Assignment::RenderAssignment);


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