# Assignment 1 - Geometry Toolbox

# Build Instructions

## From Visual Studio
Open the solution file using Visual Studio and set `Assignment1` to [`"Set as Startup Project"`](https://docs.microsoft.com/en-us/visualstudio/ide/how-to-set-multiple-startup-projects?view=vs-2022) then Build Solution.

<br/>

# Running 

![Running1](.\ReadmeImages\Running1.png)
![Running2](.\ReadmeImages\Running2.png)

Ensure the Shaders folder is inside this directory

<br/>

# Controls

Controls and interactivity can be done through Dear Imgui menu in the various scenes.
![Controls](.\ReadmeImages\ScreenshotUI.png)

Camera interactivity, some control of scene context objects and other options should be visible on the screen as in the example screenshot.
The compiled project will ship will a reasonable imgui.ini file that should resemble the screenshot. Instructions are self-explantory

<br/>

# Brief

```
The purpose of this assignment is to be able to construct and use core bounding
volumes (Bounding Spheres and AABBs) for simple optimizations. This includes
using them for bounding volume spatial partitions to optimize ray-casting, frustum-
casting, and pair-queries.

This assigmment simply implements the 14 Tests as indicated in the Assignment Brief
```
(extract from `Assignment 1 SU2022.pdf` on Moodle)

<br/>

## Scenes

<br/>

Use the Dear Imgui menu in order to navigate to the various scenes. 
Note that point collisions are are very sensitive so Dear Imgui controls should be used to verify its collisions

![Scene Image](.\ReadmeImages\SceneList.png)


| Scene | Description  |  Completed
|--|--|--|
|Sphere vs. Sphere  | Sphere vs Sphere collision | Completed successfully
|AABB vs. Sphere*  | AABB vs Sphere collision | Completed successfully
|Sphere vs. AABB*  | Sphere vs AABB collision | Completed successfully
|Point vs. Sphere  | Point vs Sphere collision | Completed successfully
|Point vs. AABB  | Point vs AABB collision | Completed successfully
|Point Vs Triangle| Point vs Triangle Collision | Partially unfinished***
|Point Vs Plane| Point vs Plane Collision | Completed successfully
|Ray Vs Plane| Ray vs Plane Collision | Completed successfully
|Ray Vs AABB| Ray vs AABB Collision | Completed successfully
|Ray Vs Sphere| Ray vs Sphere Collision | Completed successfully
|Ray Vs Triangle| Ray vs Triangle Collision | Partially unfinished***
|Plane Vs AABB | Plane vs AABB Collision | Completed successfully**
|Plane Vs Sphere | Plane vs Sphere Collision | Completed successfully


*The difference between them is which object has a defualt kinematic
** Uses segment interval checks so might be a bit inaccurate

*** Ran out of time to debug Ray/Point vs triangle interactions. Was getting close to it and applying barycentric coordinate principle of edge detection test but I must have missed something

<br/>

## Failures and Assumptions

Triangle collisions are buggy and unfinished.

<br/>

## Relevant Source Code
![Source Image](.\ReadmeImages\SourceCode.png)


![Source Image](.\ReadmeImages\ShaderCode.png)

<br/>

## Features and Grades

Completed credit features

| Feature | Description  |  Completed
|--|--|--|
|Picture in Picture  | View top down perspective on the top right of the screen | Completed successfully
|Display the primitives   | Display the primitives of the various bounding voxes| Completed successfully and can be viewed using Wireframe mode
|Camera   | Simple interactive camera| Completed successfully
|Correct Rendering| Rendering using shader to display color changes | Completed successfully
|Objects loaded correctly from the files ... | Load .obj files | NOT COMPLETED 

v

# Hours Spent
Around 13+ accumlated Hours from scratch. Remebered having two 7 hour days back to back. Did not estimate properly but I spent too long on it.