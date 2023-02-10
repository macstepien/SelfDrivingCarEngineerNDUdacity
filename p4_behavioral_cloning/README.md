# **Behavioral Cloning** 

**Behavioral Cloning Project**

The goals / steps of this project are the following:
* Use the simulator to collect data of good driving behavior
* Build, a convolution neural network in Keras that predicts steering angles from images
* Train and validate the model with a training and validation set
* Test that the model successfully drives around track one without leaving the road
* Summarize the results with a written report


[//]: # (Image References)

[image1]: ./examples/center_lane.jpg "Center lane driving"

---
## Files Submitted & Code Quality

### 1. Submitted files

My project includes the following files:
* modelTrack1.py containing the script to create and train the model for first track
* modelTrack1.h5 containing a trained convolution neural network for first track
* track1.mp4 video with completed first track lap (autonomous mode, modelTrack1.h5)
* modelTrack2.py containing the script to create and train the model for second track
* modelTrack2.h5 containing a trained convolution neural network for second track
* track2.mp4 video with completed second track lap (autonomous mode, modelTrack2.h5)
* modelTrackBoth.py containing the script to create and train the model for both tracks
* drive.py for driving the car in autonomous mode
* writeup_report.md summarizing the results

### 2. Submission includes functional code
Using the Udacity provided simulator and drive.py file, the car can be driven autonomously around the track by executing 
```sh
python drive.py modelTrack1.h5
```
(or ```modelTrack2.h5```).

## Model Architecture and Training Strategy

### 1. Design Approach

As suggested I started with architecture presented in [End-to-End Deep Learning for Self-Driving Cars](https://developer.nvidia.com/blog/deep-learning-self-driving-cars/). It was already able to perform really well on first track, so I left it without changes. 

For second track I slightly modified it - mainly by my mistake I discovered that it performed even better without last but one fully connected layer (the one with 10 neurons). Steering angles were smoother, and what is even more important it was able to finish whole lap. I also increased `min_delta` (in `EarlyStopping` callback) up to 0.001 - training was finished before overfitting started. 

To create model for both tracks first I tried modified architecture from second track. While training network for both tracks I observed large overfitting, so I added dropouts for all fully connected layers. It was able to largely improve overfitting and lower final val_loss score. Also in result performance on second track, which was more problematic improved, but it still wasn't able to complete whole track.

I also tried original architecture, which in this case resulted in opposite behaviour - second track was completed, but on first car had problems and drove off the track right at the beginning. To improve results I increased data from first track by adding my new data (before there was a little disproportion - there were more data from second track, which I think could cause better performance on second track), but it still didn't help. 

I wasn't able to create generic network that could complete both tracks, best performance I got was with second track model with dropouts - then model was able to complete track 1 and on track 2 got to first downhill part. 

### 2. Attempts to reduce overfitting in the model

On first two models I didn't add dropout layers as overfitting wasn't that big and models did a good job in simulation. Combining these two models into one, that could drive on both tracks was a challenge though. There overfitting was much larger and model could perform only on first track. By introducing dropouts I was ably to significantly reduce overfitting, and also model was able to get lower *val_loss* overall. It also improved performance on second track - it was able to drive around 30 seconds longer, but still couldn't finish track. 


### 3. Appropriate training data

Training data was chosen to keep the vehicle driving on the road. I mostly used a center lane driving. For first track I used already prepared data, that gave me really good results (recording data myself was really painful due to slow and unreliable internet connection). For second track I recorded two laps (center lane driving) - one normal and one counterclockwise. For both tracks I also tried adding recovery data, but failed. Resulting models were worse than before. I believe that my data was correct - only recovery from side of the road was recorded. I think that I was too slow with turning on recovery though, so network wasn't able to clearly see desired behaviour. 

Here is an example image of center lane driving on track 2:

![alt text][image1]

To augment the data set, I used images from three cameras, that were available. For side cameras I added 0.2 (and -0.2) offset to steering angle to compensate that they were taken from the sides. To further augment data I also flipped all three images (and steering angle by multiplying by -1).  

After all augmentation I got 6 times more data. For first track this resulted in 48216 samples and 74292 for second: 

In preprocessing step I normalized data (division by 255 and 0.5 subtraction) and cropped (so only lower part of image containing road information was visible).

Data set was split into training and validation, 20% of the data was put into a validation set. 

### 4. Model parameter tuning

The model used an adam optimizer, so the learning rate was not tuned manually. I chose large number of epochs (20) and mainly relied on early stopping with parameters min_delta=0.0005 and patience=3. I also used ModelCheckpoint to save only the best model available. 

### 5. Final Model Architecture

The final model architecture for first track looked exactly like in [suggested model](https://developer.nvidia.com/blog/deep-learning-self-driving-cars/). It is implemented on lines 73-85 in *modelTrack1.py*. 

The model for second track has removed last but one fully connected 10 neuron layer (implemented on lines 75-86 in *modelTrack2.py*).

The model for both tracks that performed best (and I included) is like the second one, but with dropouts added for all fully connected layers (lines 97-113 in *modelTrackBoth.py*).