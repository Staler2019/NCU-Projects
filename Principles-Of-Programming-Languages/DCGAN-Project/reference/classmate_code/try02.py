import os
import numpy as np
from tensorflow import keras
from tensorflow.keras.datasets import mnist
from tensorflow.keras.layers import Input, Dense, Flatten, Reshape, Dropout
from tensorflow.keras.layers import BatchNormalization
from keras.layers.advanced_activations import LeakyReLU
from tensorflow.keras.models import Sequential
from tensorflow.keras.optimizers import Adam
import matplotlib.pyplot as plt

plt.switch_backend("agg")


class GAN(object):
    def __init__(self, width=28, height=28, channels=1):
        self.width = width
        self.height = height
        self.channels = channels

        self.shape = (self.width, self.height, self.channels)
        self.optimizer = Adam(lr=0.0002, beta_1=0.5, decay=8e-8)

        self.G = self.__generator()
        self.G.compile(loss="binary_crossentropy", optimizer=self.optimizer)

        self.D = self.__discriminator()
        self.D.compile(
            loss="binary_crossentropy", optimizer=self.optimizer, metrics=["accuracy"]
        )

        self.stacked_generator_discriminator = self.__stacked_generator_discriminator()
        self.stacked_generator_discriminator.compile(
            loss="binary_crossentropy", optimizer=self.optimizer
        )

    def __stacked_generator_discriminator(self):
        self.D.trainable = False

        model = Sequential()
        model.add(self.G)
        model.add(self.D)

        return model

    def __generator(self):
        model = Sequential()
        model.add(Dense(256, input_shape=(28,)))
        model.add(LeakyReLU(alpha=0.2))
        model.add(BatchNormalization(momentum=0.8))
        model.add(Dense(512))
        model.add(LeakyReLU(alpha=0.2))
        model.add(BatchNormalization(momentum=0.8))
        model.add(Dense(self.width * self.height * self.channels, activation="tanh"))
        model.add(Reshape((self.width, self.height, self.channels)))
        model.summary()

        return model

    def __discriminator(self):
        model = Sequential()
        model.add(Flatten(input_shape=self.shape))
        model.add(
            Dense((self.width * self.height * self.channels), input_shape=self.shape)
        )
        model.add(LeakyReLU(alpha=0.2))
        model.add(Dense(int((self.width * self.height * self.channels) / 2)))
        model.add(LeakyReLU(alpha=0.2))
        model.add(Dense(1, activation="sigmoid"))
        model.summary()

        return model

    def train(self, Xtrain, iwant, epochs=4000, batch=32, save_interval=100):
        legit_images = X_train[iwant].reshape(
            int(batch / 2), self.width, self.height, self.channels
        )
        for cnt in range(epochs):

            # train discriminator
            # legit_images = X_train[iwant].reshape(int(batch/2), self.width, self.height, self.channels)
            gen_noise = np.random.normal(0, 1, (int(batch / 2), 28))
            synthetic_image = self.G.predict(gen_noise)
            x_combined_batch = np.concatenate((legit_images, synthetic_image))
            y_combined_batch = np.concatenate(
                (np.ones((int(batch / 2), 1)), np.zeros((int(batch / 2), 1)))
            )
            d_loss = self.D.train_on_batch(x_combined_batch, y_combined_batch)

            # train generator
            noise = np.random.normal(0, 1, (batch, 28))
            y_mislabled = np.ones((batch, 1))
            g_loss = self.stacked_generator_discriminator.train_on_batch(
                noise, y_mislabled
            )

            print(
                "epoch: %d, [Discriminator :: d_loss: %f, [ Generator :: loss: %f]"
                % (cnt, d_loss[0], g_loss)
            )
            if cnt % save_interval == 0:
                self.plot_images(save2file=True, step=cnt)

    def plot_images(self, save2file=False, samples=16, step=0):
        if not os.path.exists("./images"):
            os.makedirs("./images")
        filename = "./images/mnist_%d.png" % step
        noise = np.random.normal(0, 1, (samples, 28))

        images = self.G.predict(noise)

        plt.figure(figsize=(10, 10))

        for i in range(images.shape[0]):
            plt.subplot(4, 4, i + 1)
            image = images[i, :, :, :]
            image = np.reshape(image, [self.height, self.width])
            plt.imshow(image, cmap="gray")
            plt.axis("off")

        plt.tight_layout()

        if save2file:
            plt.savefig(filename)
            plt.close("all")
        else:
            plt.show()


if __name__ == "__main__":
    want = np.arange(16)
    numiwant = int(input("請輸入："))
    (X_train, Y_train_label), (_, _) = mnist.load_data()
    nums = 0
    for i in range(len(Y_train_label)):
        if int(numiwant) == Y_train_label[i]:
            want[nums] = i
            nums = nums + 1
        if nums == 16:
            break
    # rescale -1 to 1
    X_train = (X_train.astype(np.float32) - 127.5) / 127.5
    X_train = np.expand_dims(X_train, axis=3)
    gan = GAN()
    gan.train(X_train, want)
