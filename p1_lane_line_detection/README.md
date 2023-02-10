# **Finding Lane Lines on the Road** 

[//]: # (Image References)

[image1]: ./test_images_output/solidWhiteRight.jpg "Grayscale"

You can find implementation of the following pipeline in the `p1_lane_line_detection.ipynb`.

## 1. Pipeline

For lines detection I created RoadLineDetector class, which wrapped together parameters (defined in constructor) and pipeline (function detectLines). My pipeline consisted of 5 steps:
1. **Grayscale conversion**
2. **Gaussian blur** with kernel size equal to 3. I tried also value 7, but it didn't make much difference. Also gaussian blur of size 5 is applied in canny opnecv implementation.
3. **Canny edge detection**. I defined maxThreshold to bo 2 times larger than minThreshold (it should be between 2:1 and 3:1). Then I tuned min Threshold by applying edge detection and looking at images. It was important to also check yellow lines, as they needed lower threshold to be detected. I settled for 90 value.
4. **Region of interest** - I used trapeze shape and used pixel position values tuned for given images. 
5. Line detection (**Hough transform**). For this step I didn't use helper function, but manually called HoughLinesP function from OpneCV. Increasing theta value gave better resolution in Hough Space, which resoulted in more accurate lines (I changed to 0.07 degree compered to 1 degree proposed earlier in course). Rho was set to lowest possible (and highest resolution): 1. For max gap I used 4 - I played around with this value and it looked best. I used quite low threshold and min line length (30 and 10) so smaller line segments further away can also be detected. 
6. Lines estimation. In this step I took line segments detected with hough lines and merged them to obtain final lines. Separate class method was created for this purpose (I think that it is cleaner then modyfying draw_lines function, which remained as only drawing function). I used following line equation ```y=ax+b```. Segments were grouped into possible line by checking if they are within threshold of first segment (parameters aDiffThreshold and bDiffThreshold). I also used b values for grouping in case of further improving line detection - we not necesarily want to only detect two lines left and right. For example to allow car to change lanes we should be able to detect two left lines. They will differ in a values, but considering b should make it easier. Lines are then calculated by weighted mean of (a, b) segments values. Weights are defined as length of segments - longer ones should be able to make more difference as they are more likely to be accurate (some further away dashed line may not be very accurate). 


![alt text][image1]

### Challange
I also tried modyfying my pipeline so it able to work in some tougher conditions. I decided to redefine whole class, not to interfere with previous results. Here are changes:
1. Region of interest - due to change in resolution previously defined one was no longer useful. I decided to chenge shape to triangle and defined it based on shape proportions (so it will also work for previous examples)
2. Lowered minThreshold to 40. It was quite large change - in challange video in one place there is yellow line in difficult lightning conditions, which motivated significant change of minThreshold.
3. As final lines that are detected from segments I choose only two candidates. This choice is based on sum of length of segments that were classified as line. This criteria should only leave two most probable lines with assumption that there are two lines in selected ROI. 

These changes allowed line detection to be functional in challange video, not perfect, but it worked. I then reapplied detection to all previous examples to check if I didn't overfit this method, it also worked (all results can be found in *challange_output* directory).

## 2. Potential shortcomings

1. One potential shortcoming would be what would happen when car is in the middle of changing lanes - then one line can be detected as perpendicular, which won't be handled (with ```y=ax+b``` line equation)
2. Region of interest - even after improving it to not rely on resolution of image, it still isn't quite versatile. For example when camera is mounted differentely, it will come short. Also I think that detecting other lines can be useful (to change lanes for example). And another possible problems could be bumps in the road, which will cause ROI to change. 
3. Turns - only detecting lines can't quite deal with turns, especially when they are narrow.
4. In my implementation I used (a,b) values calculated for first segment to then match other segments to line. I think that it could be a little bit random. 

## 3. Possible improvements 

1. Change line representation to Polar coordinate system (as in hough transform implementation).
2. Detect lines in whole image, without defined Region of Interest
3. Use curves instead of lines
4. Modified Euclidean clustering for grouping segments into lines. I've got two ideas for this: scale a value or cluster a and b separetly.  
5. In challange video sometimes right line isn't detected and two lines appear on the left. It could be filtered that there should be one left and one right line. 
6. Also in challange video few times right line jumps to much to the right. Maybe using Kalman filter could smooth out detections and weaken such jumps. 
