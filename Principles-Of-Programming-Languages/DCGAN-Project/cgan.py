##
# @filename: cgan.py
# @date: 21/5/22
# @last modified date: 21/6/6

import os
import matplotlib.pyplot as plt
import numpy as np
import random
import tensorflow as tf
import imageio
import glob
from tensorflow.keras import Model
from tensorflow.keras import Sequential
import tensorflow.keras.backend as K
from tensorflow.keras.datasets import mnist
from tensorflow.keras.layers import *
from tensorflow.keras.optimizers import Adam


class MyHistory:
    def __init__(self):
        self.acc = []
        self.loss = []

    def addLoss(self, loss):
        self.loss += [loss]

    def addAcc(self, acc):
        self.acc += [acc]

    def getLoss(self):
        return self.loss

    def getAcc(self):
        return self.acc


def load_data(path):
    with np.load(path, allow_pickle=True) as f:
        x_train, y_train = f["x_train"], f["y_train"]
        x_test, y_test = f["x_test"], f["y_test"]

    return (x_train, y_train), (x_test, y_test)


def printShape(train_x, test_x):
    print("----------------------------")
    print("Train shape:", train_x.shape)
    print("Test shape:", test_x.shape)
    print("----------------------------")


def getGenerator(g_sample_input, g_label_input):
    # 生成器 generator
    g_sequential = Sequential(
        [
            Dense(7 * 7 * 64, input_shape=[100 + 10]),
            BatchNormalization(),
            LeakyReLU(),
            Reshape([7, 7, 64]),
            UpSampling2D([2, 2]),
            Conv2DTranspose(64, [3, 3], padding="same"),
            BatchNormalization(),
            LeakyReLU(),
            UpSampling2D([2, 2]),
            Conv2DTranspose(1, [3, 3], padding="same", activation="sigmoid"),
        ]
    )

    # 合併隨機數據輸入與指定標籤獨熱碼
    condition_g_sample_input = K.concatenate(
        [g_sample_input, K.one_hot(g_label_input, 10)]
    )

    # 生成器輸出
    g_output = g_sequential(condition_g_sample_input)
    # 生成器模型
    generator = Model(inputs=[g_sample_input, g_label_input], outputs=g_output)

    return generator


def getDiscriminator():
    # 判別器 discriminator
    discriminator = Sequential(
        [
            Conv2D(64, [3, 3], padding="same", input_shape=[28, 28, 1]),
            BatchNormalization(),
            LeakyReLU(),
            MaxPool2D([2, 2]),
            Conv2D(64, [3, 3], padding="same"),
            BatchNormalization(),
            LeakyReLU(),
            MaxPool2D([2, 2]),
            Flatten(),
            Dense(128),
            BatchNormalization(),
            LeakyReLU(),
            Dense(11, activation="softmax"),
        ]
    )
    return discriminator


def log_clip(x):
    """
    裁減機率到區間 [1e-3, 1] 內，並求其 log ，避免 log 後為 inf，K.stop_gradient 表示訓練時不對其求梯度
    ，這裡也可直接寫成 log_clip = Lambda(lambda x: K.log(x + 1e-3))
    """
    return K.log(K.clip(K.stop_gradient(x), 1e-3, 1) - K.stop_gradient(x) + x)


def compileFitDiscriminator(
    generator,
    g_sample_input,
    g_label_input,
    d_input,
    d_label_input,
    d_prob,
    d_index,
    g_prob,
    g_index,
):
    def getD_loss(d_prob, d_index, g_prob, g_index):
        # log(真實樣本正確標籤的機率值)  # log(1-假樣本指定標籤的機率值)
        d_loss = -log_clip(tf.gather_nd(d_prob, d_index)) - log_clip(
            1.0 - tf.gather_nd(g_prob, g_index)
        )

        return d_loss

    def getFitDiscriminator(
        g_sample_input, g_label_input, d_input, d_label_input, d_loss
    ):
        fit_discriminator = Model(
            inputs=[g_sample_input, g_label_input, d_input, d_label_input],
            outputs=d_loss,
        )
        # 添加自定義loss
        fit_discriminator.add_loss(d_loss)

        return fit_discriminator

    d_loss = getD_loss(d_prob, d_index, g_prob, g_index)
    fit_discriminator = getFitDiscriminator(
        g_sample_input, g_label_input, d_input, d_label_input, d_loss
    )
    generator.trainable = False
    for layer in generator.layers:
        # 設置 BatchNormalization 為訓練模式
        if isinstance(layer, BatchNormalization):
            layer.trainable = True
    # fit_discriminator.compile(optimizer=Adam(0.001), metrics=["accuracy"])
    fit_discriminator.compile(optimizer=Adam(0.001))
    generator.trainable = True

    return fit_discriminator, generator


def compileFitGenerator(discriminator, g_prob, g_index, g_sample_input, g_label_input):
    def getFitGenerator(g_sample_input, g_label_input, g_loss):
        fit_generator = Model(inputs=[g_sample_input, g_label_input], outputs=g_loss)
        # 添加自定義loss
        fit_generator.add_loss(g_loss)

        return fit_generator

    # log(假樣本指定標籤的機率值)
    g_loss = -log_clip(tf.gather_nd(g_prob, g_index))
    fit_generator = getFitGenerator(g_sample_input, g_label_input, g_loss)
    # 生成器訓練時不更新discriminator的參數
    discriminator.trainable = False
    for layer in discriminator.layers:
        # 設置 BatchNormalization 為訓練模式
        if isinstance(layer, BatchNormalization):
            layer.trainable = True
    # fit_generator.compile(optimizer=Adam(0.001), metrics=["accuracy"])
    fit_generator.compile(optimizer=Adam(0.001))
    discriminator.trainable = True

    return fit_generator, discriminator


def printSummary(discriminator, generator):
    print()
    print("Discriminator summary:")
    discriminator.summary()
    print()
    print("Generator summary:")
    generator.summary()
    print()


def saveModels(discriminator, generator, fit_discriminator, fit_generator):
    discriminator.save("./models/discriminator")
    generator.save("./models/generator")
    fit_discriminator.save("./models/fit_discriminator")
    fit_generator.save("./models/fit_generator")

    discriminator.save("./models/discriminator.h5")
    generator.save("./models/generator.h5")
    fit_discriminator.save("./models/fit_discriminator.h5")
    fit_generator.save("./models/fit_generator.h5")


def generateImages(generator, steps):
    if not os.path.exists("./images"):
        os.makedirs("./images")
    filename = "./images/epoch_%d.png" % steps

    fig, axes = plt.subplots(10, 10, figsize=(10, 10))
    for i in range(10):
        for j in range(10):
            axes[i, j].imshow(
                generator.predict(
                    [K.constant(np.random.uniform(-1, 1, [1, 100])), K.constant([i])]
                )[0].reshape([28, 28]),
                cmap="gray",
            )
            axes[i, j].axis(False)
    plt.savefig(filename)


def savePlotLossAndAcc(d_history, g_history):
    fig, axes = plt.subplots(1, 2, figsize=(20, 10))
    # loss
    axes[0].plot(d_history.getLoss(), label="discriminator")
    axes[0].plot(g_history.getLoss(), label="generator")
    axes[0].set_title("Loss")
    # axes[0].set_ylabel("loss")
    # axes[0].set_xlabel("epoch")
    axes[0].legend(loc="upper left")
    # acc
    axes[1].plot(d_history.getAcc(), label="discriminator")
    axes[1].plot(g_history.getAcc(), label="generator")
    axes[1].set_title("Accuracy")
    # axes[1].set_ylabel("accuracy")
    # axes[1].set_xlabel("epoch")
    axes[1].legend(loc="upper left")

    plt.setp(axes[0], xlabel="epoch", ylabel="loss")
    plt.setp(axes[1], xlabel="epoch", ylabel="accuracy")
    plt.savefig("loss_acc.png")


def makeGif():
    anim_file = "dcgan.gif"

    with imageio.get_writer(anim_file, mode="I") as writer:
        filenames = glob.glob("image*.png")
        filenames = sorted(filenames)
        for filename in filenames:
            image = imageio.imread(filename)
            writer.append_data(image)


if __name__ == "__main__":
    ### load data
    # (train_x, train_y), (test_x, test_y) = load_data("./dataset/mnist.npz")
    (train_x, train_y), (test_x, test_y) = mnist.load_data()
    # printShape(train_x, test_x)
    # flatten 後歸一化
    train_x = train_x.reshape([-1, 28, 28, 1]) / 255

    # 生成器輸入
    g_sample_input = Input([100])
    # 指定標籤輸入
    g_label_input = Input([], dtype="int32")
    # 真實樣本輸入
    d_input = Input([28, 28, 1])
    # 真實樣本標籤輸入
    d_label_input = Input([], dtype="int32")

    generator = getGenerator(g_sample_input, g_label_input)
    discriminator = getDiscriminator()

    # 判別器識別假樣本的輸出
    g_prob = discriminator(generator([g_sample_input, g_label_input]))
    # 用於索引 g_prob 指定標籤機率值
    g_index = K.stack([K.arange(0, K.shape(g_prob)[0]), g_label_input], axis=1)
    # 判別器識別真實樣本的輸出
    d_prob = discriminator(d_input)
    # 用於索引 d_prob 正確標籤機率值
    d_index = K.stack([K.arange(0, K.shape(d_prob)[0]), d_label_input], axis=1)

    fit_discriminator, generator = compileFitDiscriminator(
        generator,
        g_sample_input,
        g_label_input,
        d_input,
        d_label_input,
        d_prob,
        d_index,
        g_prob,
        g_index,
    )
    fit_generator, discriminator = compileFitGenerator(
        discriminator, g_prob, g_index, g_sample_input, g_label_input
    )

    printSummary(discriminator, generator)

    h_disc = MyHistory()
    h_gen = MyHistory()

    ### train
    batch_size = 64
    epochs = 20000
    for i in range(epochs):
        index = random.sample(range(len(train_x)), batch_size)
        x_label = train_y[index]
        x = train_x[index]
        g_sample = np.random.uniform(-1, 1, [batch_size, 100])
        g_label = np.random.randint(0, 10, [batch_size])

        h_f_dis = fit_discriminator.fit(
            [
                K.constant(g_sample),
                K.constant(g_label),
                K.constant(x),
                K.constant(x_label),
            ]
        )
        h_f_gen = fit_generator.fit([K.constant(g_sample), K.constant(g_label)])

        h_disc.addLoss(h_f_dis.history["loss"])
        # h_disc.addAcc(h_f_dis.history["acc"])
        h_gen.addLoss(h_f_gen.history["loss"])
        # h_gen.addAcc(h_f_gen.history["acc"])

        if (i + 1) % 100 == 0 or i == 0:
            generateImages(generator, steps=(i + 1))

    saveModels(discriminator, generator, fit_discriminator, fit_generator)
    savePlotLossAndAcc(h_disc, h_gen)
    makeGif()
