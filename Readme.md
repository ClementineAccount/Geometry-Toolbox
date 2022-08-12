# CSD 3150 Assignment 3 
# Build Instructions

## From Visual Studio
Open the solution file using Visual Studio and set `AssignmentTwo` to [`"Set as Startup Project"`](https://docs.microsoft.com/en-us/visualstudio/ide/how-to-set-multiple-startup-projects?view=vs-2022) then Build Solution.

The project is mislabelled as Assignment2 but it should show OctTree scene

# Screenshots

![Scene Image](.\ReadmeImages\a3-1.png)
![Scene Image](.\ReadmeImages\a3-2.png)
![Scene Image](.\ReadmeImages\a3-3.png)


# Controls

First Person-Like camera.

| Control | Function
|--|--|
|W A S D | Basic Flying around
|Q and E | Fly Up or Down
Left Shift | Doubles your flying speed
Right mouse button | Look Around (Adjust Pitch/Yaw)
V Key | Look Around (Adjust Pitch/Yaw)


<br/>

# Brief
Spatial	 Partitioning  using	 Adaptive	 Octrees and	BSP	Trees
<br/>

Wasn't able to finish on time. Started too late in the week and underestimated the workload.
Created octree partially and triangle soup concept.

The scenes were hence tested mostly in 2D with triangles.

<br/>

## Features and Grading

<br/>

| Grading Sheet  |  Progress
|--|--|
| Scene created using specified objects | Not Completed
| Top down creation of the Octree | Completed 
| Creation of Adaptive Octtree* | Completed
| Top down creation of the BSP tree | Not Completed
| Polygons intersecting the plane are split correctly | Not Completed
| Choosing appropriate split planes  | Not Completed
| BSP Tree serialized and loaded on demand | Not Completed
| Toggle Octree display | Completed
| Show all levels of the octree in different colors | Completed
| Toggle BSP tree display | Not Completed
| Show all nodes of the BSP tree in a different color | Not Completed

*Not tested with tri = 30