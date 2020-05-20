# see https://www.tensorflow.org/tutorials/load_data/tfrecord (Accessed 27 Nov 2019)
import sys
import os
import ROOT
import numpy as np
import tensorflow as tf
assert tf.__version__ == "2.2.0-rc1"

def _bytes_feature(value):
    """Returns a bytes_list from a string / byte."""
    if isinstance(value, type(tf.constant(0))):
        value = value.numpy()  # BytesList won't unpack a string from an EagerTensor.
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=list(value)))

def _float_feature(value):
    """Returns a float_list from a float / double."""
    return tf.train.Feature(float_list=tf.train.FloatList(value=[value]))

def _int64_feature(value):
    """Returns an int64_list from a bool / enum / int / uint."""
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))

def serialize_example(feature0, feature2):
    """Creates a tf.Example message ready to be written to a file."""
    # Create a dictionary mapping the feature name to the tf.Example-compatible data type.
    feature = {
        'eventId': _int64_feature(feature0),
        'image': _bytes_feature(feature2),
    }
    # Create a Features message using tf.train.Example.
    example_proto = tf.train.Example(features=tf.train.Features(feature=feature))
    return example_proto.SerializeToString()
    
def getNi(idx, voxelsAlongY):
    return np.array(idx) // np.array(voxelsAlongY)

def getNj(idx, voxelsAlongY):
    return np.array(idx) % np.array(voxelsAlongY)

def get_image(t):
    Si = getNi(t.VecIndexScnt, t.VoxelsAlongY)
    Sj = getNj(t.VecIndexScnt, t.VoxelsAlongY)
    Ci = getNi(t.VecIndexCkov, t.VoxelsAlongY)
    Cj = getNj(t.VecIndexCkov, t.VoxelsAlongY)
    Sval = np.array(t.VecSignalScnt_cal)
    Cval = np.array(t.VecSignalCkov_cal)
    index_0 = np.append(Si, Ci).reshape((-1, 1)).astype(np.int64)
    index_1 = np.append(Sj, Cj).reshape((-1, 1)).astype(np.int64)
    index_2 = np.append(np.zeros_like(Si), np.ones_like(Ci)).reshape((-1, 1)).astype(np.int64)
    indices = np.hstack((index_0, index_1, index_2))
    values = np.append(Sval, Cval).astype(np.float32)
    height = width = np.array(t.VoxelsAlongY)
    sp = tf.SparseTensor(indices, values, dense_shape=[height, width, 2])
    sp = tf.sparse.reorder(sp)
    return tf.io.serialize_sparse(sp)

fileName = sys.argv[1]
assert(len(sys.argv) == 2)
 
f = ROOT.TFile.Open(fileName, "read")
t = f.Get("B4")

# Write the tf.Example observations to the file
base, ext = os.path.splitext(fileName)
nentries = t.GetEntries()
with tf.io.TFRecordWriter(base+".tfrecord", options="GZIP") as writer:
    for i in range(nentries):
        t.GetEntry(i)
        feature0 = t.eventId
        print(feature0)
        feature2 = get_image(t)
        example = serialize_example(feature0, feature2)
        writer.write(example)
