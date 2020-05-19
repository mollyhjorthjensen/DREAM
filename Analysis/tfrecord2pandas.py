import sys
import tensorflow as tf
import pandas as pd
import numpy as np

fileName = sys.argv[1]
assert(len(sys.argv) == 2)

signal = 'S'

# Create a description of the features.
feature_description = {
    'eventId': tf.io.FixedLenFeature([], tf.int64, default_value=-1),
    signal+'_comi': tf.io.VarLenFeature(tf.float32),
    signal+'_comj': tf.io.VarLenFeature(tf.float32),
    signal+'_sum': tf.io.VarLenFeature(tf.float32),
    signal+'_rad_mean': tf.io.VarLenFeature(tf.float32),
    signal+'_hot': tf.io.VarLenFeature(tf.float32),
}

def _parse_function(example_proto):
  # Parse the input `tf.Example` proto using the dictionary above.
  return tf.io.parse_single_example(example_proto, feature_description)

filenames = [fileName]
raw_dataset = tf.data.TFRecordDataset(filenames)
raw_dataset

parsed_dataset = raw_dataset.map(_parse_function)

df = pd.DataFrame(columns=['eventId', signal+'_comi', signal+'_comj',
			   signal+'_sum', signal+'_rad_mean', signal+'_hot'])
for parsed in parsed_dataset.take(10):
	eventId = parsed['eventId'].numpy()
	d = {k: v.values.numpy() for k,v in parsed.items() if k != 'eventId'}
	d['eventId'] = np.repeat(eventId, len(d[signal+'_comi']))
	print(d)
