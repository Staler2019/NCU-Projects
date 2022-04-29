import keras
import numpy as np
import matplotlib.pyplot as plt
import keras.backend as K


# number = input("Please input a test number:")
generator = keras.models.load_model("./models/generator")
generator.summary()
# image show
num = float(input("input: "))
fig, axes = plt.subplots(1, 10, figsize=(10, 10))
for i in range(10):
  axes[i].imshow(generator.predict([K.constant(np.random.uniform(-1, 1, [1, 100])), K.constant([num])])[0].reshape([28, 28]), cmap='gray')
  axes[i].axis(False)
plt.title(int(num))
plt.show()