# Assignment 1 - Geometry Toolbox

# Build Instructions

## From Visual Studio
Open the solution file using Visual Studio and set `Assignment1` to [`"Set as Startup Project"`](https://docs.microsoft.com/en-us/visualstudio/ide/how-to-set-multiple-startup-projects?view=vs-2022) then Build Solution.

<br/>

# Controls

(put controls here once its there as well as describe dear imgui stuff)

# Brief

```
The purpose of this assignment is to be able to construct and use core bounding
volumes (Bounding Spheres and AABBs) for simple optimizations. This includes
using them for bounding volume spatial partitions to optimize ray-casting, frustum-
casting, and pair-queries.
```
(extract from `Assignment 1 SU2022.pdf` on Moodle)

The project was completed with the following interperation of the assignment brief.
1. Automated test cases demonstrating functionality
1. Interactive scene that allows freeform experimentation

The default sequence when the program is ran can be taught of in this manner: <br>

$start \rightarrow \left\{test_{0} \rightarrow test_{1} \rightarrow ... \rightarrow \,test_{n}\right\}\rightarrow interactive \ scene$ <br>
(where n is the total number of tests)


## Test Cases
The project uses various visual and non-visual unit tests in the scene in order to prove that the rubrics are properly satisifed. 
The initial scene will first automatically undergo through a series of tests in the listed sequence. The tests will automatically transition after a set time.


| Test Type | Test Description  |
|--|--|
|Sphere vs. Sphere  | Sphere vs Sphere collision with no motion |
|Sphere vs. Sphere | Sphere vs Sphere collision with preset motion |


## Interaction Scene
After all tests are completed, the scene will transition to an interactive setup with some randomized elements for the purposes of freeform demonstration of successful physics.
