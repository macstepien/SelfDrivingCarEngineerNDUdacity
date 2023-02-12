# Path Planning

The project was divided into two main parts: Behavior Planner and Trajectory Planner. 

## Behavior Planner
It isn't very fancy, but I found that it was good enough to do it's job. Basic idea is that the car will keep lane and drive at full speed until it comes within given distance (currently set to 30m) to other vehicle. Then it will check for possible lane changes:
* first check if car isn't currently changing lanes ("CL" state)
* find possible lane changes (for lane 1 it will be 0 and 2, for lane 2 and 0 only lane 1)
* check if these changes are safe
* check cost function - choose best lane and if cost isn't that different from our current lane, then keep lane

If change lane maneuver wasn't initiated, speed is set to closest car's. Earlier on other case is also checked - if car is in critical range then speed is set to be lower  to gain some distance. State "CL" is changed to "KL" once car is close enough to the target lane. As you can see I used only two states "KL" and "CL", which in this simple case was sufficient. Direction of lane change is decided upon current lane. In lane cost function two things are considered: distance to closest vehicle ahead and it's speed. 

## Trajectory Planner
In trajectory planner I used approach from Q&A lesson - for spline ([spline library used](https://kluge.in-chemnitz.de/opensource/spline/)) estimation there is used one previous car position (it could be taken from previous path or calculated from car current position), current car position and 3 points ahead (30m, 60m and 90m). All points are transformed into car coordinates to ensure that every xi+1 is greater than xi (it has to be true for spline function to work properly). It still could break in more narrow corners, but such are not present on highway track. For the next path rest of the previous one is used (to ensure smoothness and continuity) and new points are generated (up to a total point limit) and all points are converted back to global coordinates. 

## Summary
I relied to some extend on methods presented in Q&A lesson, but I also tried to do things on my own - before watching this lesson I did some experiments, but unfortunately results were far from what I expected. I mainly focused on trajectory generation - in my approach I first used closed spline to estimate whole track. The problem with this was a differences between s values (reported for car and ones for track). Although I was able to get correct offset, it still didn't work properly. I also tried to combine it with Jerk Minimizing Trajectory, but it was hard to calculate right time values for it to work properly. Another thing that I tried was some sort of constant jerk trajectory, but I have given up on this idea too - much simpler method with constant acceleration that was presented in Q&A was working fine.

## Possible improvements
There are quite some improvements to be made in this project:
* predicting other vehicles' behavior - as presented in Prediction classes simple Naive Bayes should be enough. Right now lane and speed changes heavily rely on safe distances that were set and it works fine most of the time. But even in simulation sometimes other vehicle does some strange lane change right in front of us and then things can get nasty. Some behavior prediction should help in this case.
* more advanced velocity control - now acceleration is constant and it doesn't exceed limits both for acceleration and jerk. But I think that in some dangerous situations jerk minimizing shouldn't be our top priority and car should be able to decelerate as fast as possible. 
* more advanced behavior planning - now car plans lane changes only on current situation, but in some cases it would be more efficient to wait a little for faster lane and then change, instead of immediate change to slower lane. 