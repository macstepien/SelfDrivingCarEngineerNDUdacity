# Advanced Lane Finding Project

The goals / steps of this project are the following:

* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.

[//]: # (Image References)

[imgUndist]: ./output_images/undistorted.jpg "Undistorted"
[imgDist]: ./output_images/distorted.jpg "Distorted"
[imgRoadUndist]: ./output_images/roadUndist.jpg "Distorted"
[imgRoadBin]: ./output_images/roadBinary.jpg "Distorted"
[imgRoadBinTrans]: ./output_images/roadBinaryTransformed.jpg "Distorted"
[imgDetectedLines]: ./output_images/detectedLines.jpg "Distorted"
[imageProcessed]: ./output_images/imageProcessed.jpg "Distorted"
[imageProcessed8]: ./output_images/imageProcessed8.jpg "Distorted"
[video1]: ./project_video_output_prior.mp4 "Video"

Project is implemented in the `p2_advanced_lane_line_detection.ipynb`.

---

## Camera Calibration

For camera calibration and distortion correction I created ```CameraDistortion``` class. It has two methods:
* ```calibrate``` - takes list of images used for calibration as a argument. First ```cv2.findChessboardCorners``` function is used to find chessboard corners on image. This coordinates are then appended to the array that stores image positions of corners. To the other array real world position of corners are appended (for all detection they are the same - we assume that chessboard is plane, so z=0 and other coordinates are number of row and column of detected corner). Then ```cv2.calibrateCamera()``` calculates camera matrix and distortion coefficients from detected chessboard corners.
* ```undistort``` - wraps OpenCV function by the same name: ```cv2.undistort()``` and calls it with camera matrix and distortion coefficients calculated during calibration

Original image:
![alt text][imgDist]
Undistorted image:
![alt text][imgUndist]


## Pipeline (single images)

### 1. Distortion correction

For this step I used ```CameraDistortion```, with calibration done beforehand. Undistorted road image:
![alt text][imgRoadUndist]

### 2. Thresholded binary image 

In ```LinePixelDetector``` I implemented various methods that are used to find lines on images. 

Gradient detection:
* `abs_sobel_thresh` - uses Sobel operator in defined direction (x or y) and then selects pixels that are within threshold
* `mag_thresh` - applies Sobel in x and y direction and calculates magnitude. After that values within threshold are selected
* `dir_threshold` - applies Sobel in x and y direction and calculates angle between them.  Values within threshold are selected
* `gradient_detection` - here all three operations are combined into one final image. If some point in image is considered as a line pixel abs_sobel_thresh in x and y direction have to be equal 1 or both mag_thresh and dir_threshold have to be 1

Color detection:
* `hls_select` - threshold is applied to saturation channel 

Finally outputs from gradient detection and hls_select are combined in `detect` function - I used sum of this two detection methods. 


![alt text][imgRoadBin]

### 3. Perspective transform

In class `ViewTransformer` I created two methods that are used to transform image from perspective to birds view and the other way around: `standardToBird` and `birdToStandard`. 
Transform is calculated based on defined points:

| Source        | Destination   | 
|:-------------:|:-------------:| 
| 243, 685      | 243, 720      | 
| 578, 460      | 243, 0        |
| 704, 460      | 1062,0        |
| 1062, 685     | 1062, 720     |


![alt text][imgRoadBinTrans]

### 4. Lane-line pixels detection

This functionality is implemented in `LineDetector` class. It can work in two modes - with or without prior (option with prior is mainly useful for video, so for picture it wasn't used). Two methods that search for line pixels are:
* `findLinePixels` - it uses window around given x value to find line pixels. Initial window position is calculated as maximum histogram value in each half of the image. Then x values are updated based on pixels detected earlier.
* `findLinePixelsPrior` - pixels that are within margin from prior lines are used as line pixels.

Based on detected line pixels second degree polynomial is calculated (using `np.polyfit`).

For prior search first it is determined if both lines have valid detections. If they have then `findLinePixelsPrior` is called and line detections are updated:
* if new detection is within thresholds to old, then it is used as valid detection and new line coefficients are calculated as 0.75*old+0.25*new
* if it isn't, then old line coefficients are used. If lines weren't detected in the last 5 frames, search is run from scratch - detections are marked as invalid

If prior can't be used `findLinePixels` function is called. 

![alt text][imgDetectedLines]

### 5. Calculating radius of curvature of the lane and the position of the vehicle with respect to center.

For this purpose I created `LineMeasurments` class with following methods:
* `measureCurvature` - curvature is calculated for left and right line, final curvature is returned as a average between them 
* `measureCarPosition` - car position is calculated as difference between middle position of image (middle of the car) and point halfway between two detected lines at the bottom of image

### 6. Creating ouput image with detected lines, curvature and position information

I created `AdvancedLaneDetector` class that combines all steps together. It also adds text with lane curvature and car position to the image.

![alt text][imageProcessed]

Here lines are almost straight, which results in large radius.

![alt text][imageProcessed8]


---

## Pipeline (video)


Here's a [link to my video result](./project_video_output_prior.mp4)

---

## Discussion


* Thresholded binary image - simple sum of detections from gradient and hls color space isn't too good. Especially saturation thresholding sometimes returns also pixels from shadows. 
* Lane-line pixels detection - right now there is no initial check on lines, but we know how lines approximately should look like, which will help disregarding some obvious bad detections. 
* My pipeline fails to properly identify lines in presence of lines from shadows or changes in shades of road. Something like checking width of line could help here - road lines are wider than change in color caused by shade 
