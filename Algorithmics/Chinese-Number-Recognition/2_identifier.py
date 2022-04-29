import matplotlib.image as img
import numpy as np
import os
from keras.models import Sequential
from keras.layers import *
from keras.utils.np_utils import to_categorical
from keras.callbacks import ModelCheckpoint


def loadData(src):
    # output shape is [-1, 28, 28, 1]
    x = np.empty([0, 28, 28, 1])
    # store label of now bmp file
    y = np.empty([0])

    # os.walk for all directories and files in the given directory
    for root, dirs, files in os.walk(src):

        # root is going to be like: "./handwrite__detect/train_image\104602513\0" and "0" is the label
        root_split = root.split("\\")

        # if root get to be like: "./handwrite__detect/train_image\104602513", that's not what we need
        if len(root_split) == 3:
            # go through all files in this root
            for f in files:
                # the file's current position
                in_file = os.path.join(root, f)
                # add the image label to the array
                y = np.append(y, [root_split[2]], axis=0)

                # 1. image only black(255, 255, 255) or white(0, 0, 0), so just choose the first number
                # 2. img.imread(in_file) give shape with RGB and transparency
                #    , so I just choose to see the R's value
                np_img = img.imread(in_file)[:, :, 0].reshape([1, 28, 28, 1])
                # add image to the array
                x = np.append(x, np_img, axis=0)

    # x: image np_array
    # y: corresponding label(np_array) to x
    return x, y


if __name__ == "__main__":
    # training data position
    train_dataset = "./handwrite__detect/train_image"
    # testing data position
    test_dataset = "./handwrite__detect/test_image"

    # load data
    # 1. x: image list, y: number list
    # 2. shape = [-1, 28, 28, 1]
    # -------------------------- #
    # get data from my function
    x_train, y_train = loadData(train_dataset)
    # get data from my function
    x_test, y_test = loadData(test_dataset)

    # normalization the color value: 0~1
    x_train = x_train / 255
    # normalization the color value: 0~1
    x_test = x_test / 255
    # make the label to 10 categories (definition step)
    y_train = to_categorical(y_train, 10)
    # make the label to 10 categories (definition step)
    y_test = to_categorical(y_test, 10)

    # make a CNN model
    # make a sequential model and add setting to it
    model = Sequential(
        [
            # 2D conv. using 3x3 kernel map , relu activation, and output a feature map numbered 32
            Conv2D(32, kernel_size=(3, 3), activation="relu", input_shape=(28, 28, 1)),
            # 2D conv. using 3x3 kernel map , relu activation, and output a feature map numbered 64
            Conv2D(64, (3, 3), activation="relu"),
            # Pooling layer choose Max value in 2D, pool_size=2x2
            MaxPooling2D(pool_size=(2, 2)),
            # avoid to overfit: 0.25 is ratio of feature we remain
            Dropout(0.25),
            # conv layer to fully connected layer
            Flatten(),
            # fully connected using relu activation, and output a feature map numbered 128
            Dense(128, activation="relu"),
            # avoid to overfit: 0.5 is ratio of feature we remain
            Dropout(0.5),
            # fully connected using softmax activation, and output 10 which is about number of categories
            Dense(10, activation="softmax"),
        ]
    )

    # finish the model by adding:
    #   1. loss: loss function, which is defined in "keras.losses"
    #   2. optimizer: define in "keras.optimizers"
    #   3. metrics: performance evaluation functions
    model.compile(
        loss="categorical_crossentropy", optimizer="adam", metrics=["accuracy"]
    )

    # setting
    # sub-function for model.fit to do
    cb = ModelCheckpoint(
        # which position to save the model
        "./models/chinese_number_identification_model.h5",
        # data in a training wanted to monitor
        monitor="val_loss",
        # not to display what's doing
        verbose=0,
        # with the next line, just save the best weight of model
        save_best_only=True,
        save_weights_only=True,
        # if accuracy improved, save the model
        mode="auto",
        # check model once a training
        period=1,
    )

    # training
    model.fit(
        # training bmp data
        x_train,
        # training label
        y_train,
        # batch size of data each time
        batch_size=128,
        # train model 100 times
        epochs=200,
        # add testing data to make model more accuracy
        validation_split=0.1,
        # set callback function
        # callbacks=[cb],
    )

    # accuracy viewing
    # ---------------- #
    # load the saved model
    # model.load_weights("./models/chinese_number_identification_model.h5")
    model.save("./models/chinese_number_identification_model_BN_policy.h5")
    # evaluate this model
    score = model.evaluate(x_test, y_test)
