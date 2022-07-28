# CSD 3150 Assignment 2 - Bounding Volumes Representation

# Build Instructions

## From Visual Studio
Open the solution file using Visual Studio and set `AssignmentTwo` to [`"Set as Startup Project"`](https://docs.microsoft.com/en-us/visualstudio/ide/how-to-set-multiple-startup-projects?view=vs-2022) then Build Solution.


# Controls

First Person-Like camera.

| Control | Function
|--|--|
|W A S D | Basic Flying around
|Q and E | Fly Up or Down
Left Shift | Doubles your flying speed
Right mouse button | Look Around (Adjust Pitch/Yaw)
V Key | Look Around (Adjust Pitch/Yaw)


Dear Imgui panels should look like this that will allow more settings

![Scene Image](.\ReadmeImages\Controls1.png)
![Scene Image2](.\ReadmeImages\Controls2.png)



<br/>

# Brief
This assignment	 focuses on building Bounding Volume Hierarchies. 
<br/>

See Chapter 6 of [Real-Time Collision Detection (Christer Ericson)](https://learning.oreilly.com/library/view/real-time-collision-detection/9781558607323/)

<br/>

## Features and Grading

<br/>

| Grading Sheet  |  Progress
|--|--|
|Objects loaded correctly from the files and displayed  | Completed (Serializable from Scenes folder)
|Correct Rendering using diffuse color model | Completed
|Bounding Volume - AABB | Completed 
|Bounding Sphere - Ritter Method | Completed 
|Bounding Sphere - Modified Larsson Method | Not Done
|Bounding Sphere - PCA Method | Not Done
|Top-Down Construction - AABB | Completed
|Top-Down Construction - Bounding Spheres | Completed
|Bottom-Up Construction - AABB | Not Done
|Bottom-Up Construction - Bounding Spheres | Not Done
|Toggle/Change Bounding Volumes | Completed
|Display Bounding Volumes in Wireframe | Completed
|Camera Implementation | Completed

Estimated Total: 65 to 70% Completed (Accounting for no Misc Issues)


## Top-Down Construction Details

<br/>
Split Point Choice: 
<br/>
Mean of BV Centers

<br/>
Split Plane Choice: <br/>Local x, y, and z
coordinate axes
<br/>
Partitioning plane orthogonal
to axis of largest spread to get “fat”
boxes
<br/>
<br/>
Termination Criteria:
<br/>
Maximum Height = 7
