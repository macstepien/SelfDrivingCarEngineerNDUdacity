import csv 
import cv2
import numpy as np
from sklearn.model_selection import train_test_split
import sklearn
import math

from keras.models import Sequential
from keras.layers import Flatten, Dense, Lambda, Cropping2D, Convolution2D
from keras.layers.pooling import MaxPooling2D
from keras.callbacks import ModelCheckpoint, EarlyStopping

import matplotlib.pyplot as plt

samples = []

#Read image paths and steering angles from csv file
file_paths = ['/home/workspace/data_second_lap_2/',
         '/home/workspace/data_second_lap_counterclockwise/']

for path in file_paths:
    with open(path+'driving_log.csv') as csvfile:
        reader = csv.reader(csvfile)
        for line in reader:
            #drop first row
            if line[3] == 'steering':
                continue
            samples.append(line)

print('Data size: ' + str(len(samples)))
            
#Divide into training and validation sets
train_samples, validation_samples = train_test_split(samples, test_size=0.2)

#Generator that reads and retruns batch of data (loading all data at once causes unnecessary high memory usage)
def generator(samples, batch_size=32):
    num_samples = len(samples)
    while 1: # Loop forever so the generator never terminates
        sklearn.utils.shuffle(samples)
        for offset in range(0, num_samples, batch_size):
            batch_samples = samples[offset:offset+batch_size]

            images = []
            measurements = []
            for batch_sample in batch_samples:
                #use data from three cameras, for side cameras steering angle had to be changed by offset
                offsets = [0, 0.2, -0.2]

                for i in range(3):
                    source_path = batch_sample[i]
                    image = cv2.imread(source_path)
                    images.append(image)
                    measurements.append(float(batch_sample[3])+offsets[i])
            augmented_images, augmented_measurements = [], []

            #augment data by adding mirrored images
            for image, measurement in zip(images, measurements):
                augmented_images.append(image)
                augmented_measurements.append(measurement)
                augmented_images.append(cv2.flip(image, 1))
                augmented_measurements.append(-1.0*measurement)

            X_train = np.array(augmented_images)
            y_train = np.array(augmented_measurements)
            yield sklearn.utils.shuffle(X_train, y_train)

# Set our batch size
batch_size=32

# compile and train the model using the generator function
train_generator = generator(train_samples, batch_size=batch_size)
validation_generator = generator(validation_samples, batch_size=batch_size)

#model based on https://developer.nvidia.com/blog/deep-learning-self-driving-cars/
model = Sequential()
model.add(Lambda(lambda x: (x/255.0)-0.5, input_shape=(160,320,3)))
model.add(Cropping2D(cropping=((70,25), (0,0))))
model.add(Convolution2D(24,5,5,subsample=(2,2),activation='relu'))
model.add(Convolution2D(36,5,5,subsample=(2,2),activation='relu'))
model.add(Convolution2D(48,5,5,subsample=(2,2),activation='relu'))
model.add(Convolution2D(64,3,3,activation='relu'))
model.add(Convolution2D(64,3,3,activation='relu'))
model.add(Flatten())
model.add(Dense(100))
model.add(Dense(50))
model.add(Dense(1))

model.compile(loss='mse', optimizer='adam')

#Add callbacks - train until val_loss isn't improved by 0.001 for 3 epochs, then save only the best model
save_path = 'modelFinalTrack2.h5'
checkpoint = ModelCheckpoint(filepath=save_path, monitor='val_loss', save_best_only=True)
stopper = EarlyStopping(monitor='val_loss', min_delta=0.001, patience=3)

history_object = model.fit_generator(train_generator, 
                    steps_per_epoch=math.ceil(len(train_samples)/batch_size),
                    validation_data=validation_generator,
                    validation_steps=math.ceil(len(validation_samples)/batch_size),
                    epochs=20, verbose=1,
                    callbacks=[checkpoint, stopper])


#Show training information
print(history_object.history.keys())

plt.plot(history_object.history['loss'])
plt.plot(history_object.history['val_loss'])
plt.title('model mean squared error loss')
plt.ylabel('mean squared error loss')
plt.xlabel('epoch')
plt.legend(['training set', 'validation set'], loc='upper right')
plt.show()