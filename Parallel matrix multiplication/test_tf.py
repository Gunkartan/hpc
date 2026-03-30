import tensorflow as tf

print(tf.__version__)
print(tf.config.list_physical_devices('GPU'))

with tf.device('/GPU:0'):
    a = tf.random.uniform((4096, 4096))
    b = tf.random.uniform((4096, 4096))
    c = tf.matmul(a, b)

print(c.shape)
print(c.device)