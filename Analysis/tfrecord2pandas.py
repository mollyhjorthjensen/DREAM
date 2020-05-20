import sys
import os
import tensorflow as tf
import pandas as pd
import numpy as np

fileName = sys.argv[1]
assert len(sys.argv) == 2

base = os.path.basename(fileName)
base, ext = os.path.splitext(base)
signal = base.split('_')[0]
assert signal in ['S', 'C']

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

parsed_dataset = raw_dataset.map(_parse_function)

df1 = pd.DataFrame(columns=['eventId', signal+'_comi', signal+'_comj',
			   signal+'_sum', signal+'_rad_mean', signal+'_hot'])
for parsed in parsed_dataset:
	eventId = parsed['eventId'].numpy()
	d = {k: v.values.numpy() for k,v in parsed.items() if k != 'eventId'}
	d['eventId'] = np.repeat(eventId, len(d[signal+'_comi']))
	df2 = pd.DataFrame(d)
	df1 = df1.append(df2, ignore_index=True, sort=False)

print(f"Dataframe shape : {df1.shape}")
print(df1.head())

path = os.path.dirname(fileName)
df1.to_csv(os.path.join(path, base+".csv"), index=False)
