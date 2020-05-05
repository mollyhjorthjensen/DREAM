# see https://www.tensorflow.org/tutorials/load_data/tfrecord (Accessed 27 Nov 2019)
import sys
import ROOT
import numpy as np
import tensorflow as tf

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
# def serialize_example(feature0, feature1, feature2, feature3, feature4):
    """Creates a tf.Example message ready to be written to a file."""
    # Create a dictionary mapping the feature name to the tf.Example-compatible data type.
    feature = {
        'eventId': _int64_feature(feature0),
        # 'showerId': _bytes_feature(feature1),
        'image': _bytes_feature(feature2),
        # 'label': _bytes_feature(feature3),
        # 'energy': _bytes_feature(feature4),
        # 'height': _int64_feature(height),
        # 'width': _int64_feature(width),
        # 'depth': _int64_feature(depth),
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
    Sval = np.array(t.VecSignalScnt)
    Cval = np.array(t.VecSignalCkov)
    index_0 = np.append(Si, Ci).reshape((-1, 1)).astype(np.int64)
    index_1 = np.append(Sj, Cj).reshape((-1, 1)).astype(np.int64)
    index_2 = np.append(np.zeros_like(Si), np.ones_like(Ci)).reshape((-1, 1)).astype(np.int64)
    indices = np.hstack((index_0, index_1, index_2))
    values = np.append(Sval, Cval).astype(np.float32)
    height = width = np.array(t.VoxelsAlongY)
    sp = tf.SparseTensor(indices, values, dense_shape=[height, width, 2])
    sp = tf.sparse.reorder(sp)
    return tf.io.serialize_sparse(sp)
    
def get_target(t):
    index_0 = np.array(t.CoMi).reshape((-1, 1))
    index_1 = np.array(t.CoMj).reshape((-1, 1))
    index_2 = np.zeros_like(index_0)
    indices = np.hstack((index_0, index_1, index_2))
    lst = []
    showerId = np.array(t.showerId_filtered).astype(np.float32)
    label = np.array(t.label_filtered).astype(np.float32)
    energy = np.array(t.showerE_filtered)
    energy = energy.astype(np.float32)
    for values in [showerId, label, energy]:
        sp = tf.SparseTensor(indices, values, dense_shape=[height, width, 1])
        sp = tf.sparse.reorder(sp)
        lst += [tf.io.serialize_sparse(sp)]
    return lst

fileName = sys.argv[1]
treeName = "B4"
assert(len(sys.argv) == 2)
 
f = ROOT.TFile.Open(fileName, "read")
t = f.Get(treeName)

print(tf.__version__)
assert tf.__version__ == "2.1.0"

# Write the tf.Example observations to the file
filename = treeName + ".tfrecord"
nentries = t.GetEntries()
with tf.io.TFRecordWriter(filename, options="GZIP") as writer:
    for i in range(nentries):
        t.GetEntry(i)
        feature0 = t.GetEntryNumber(i)
        print(feature0)
        feature2 = get_image(t)
        # feature1, feature3, feature4 = get_target(t)
        # example = serialize_example(feature0, feature1, feature2, feature3, feature4)
        example = serialize_example(feature0, feature2)
        writer.write(example)
