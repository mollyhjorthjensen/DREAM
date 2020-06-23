#!/usr/bin/env python
# coding: utf-8

# In[1]:
import sys

fileName = sys.argv[1]
assert len(sys.argv) == 2


import tensorflow as tf
print(f"Tensorflow version : {tf.__version__}")


# In[2]:


from tensorflow.python.ops import sparse_ops
import tensorflow.keras.backend as K
import pandas as pd
# import matplotlib.pyplot as plt
import os
# assert tf.test.is_gpu_available()
print(tf.config.list_physical_devices('GPU'))
import matplotlib.pyplot as plt
import numpy as np


# In[3]:


from ipynb.fs.full.clustering import make_clusters, extract_features, tf_serialize_example


# ## Parser

# In[4]:


feature_description = {
    'eventId': tf.io.FixedLenFeature([1], tf.int64, default_value=0),
    'image': tf.io.FixedLenFeature([3], tf.string, default_value=["",]*3)
}

feature_shape = {
    'eventId': tf.TensorShape([1,]),
    'image': tf.TensorShape([568, 568, 2])
}

def parser_fn(proto):
    serialized = tf.io.parse_single_example(proto, feature_description)
    deserialized = {k: (tf.sparse.to_dense(sparse_ops.deserialize_sparse(v, K.floatx()))
                        if k != 'eventId' else v) for k,v in serialized.items()}
    [deserialized[k].set_shape(feature_shape[k]) for k in deserialized.keys()]
    x = deserialized['image']
    x = tf.expand_dims(x, axis=0)
    x = K.pool2d(x, pool_size=(2, 2), strides=(2, 2), pool_mode='avg')
    # sum instead of avg
    x = 4.*x
    deserialized['image'] = tf.squeeze(x)
    deserialized['S_image'] = deserialized['image'][:,:,0]
    deserialized['C_image'] = deserialized['image'][:,:,1]
    
    deserialized['tseed'] = tf.constant(4., tf.float32)
    deserialized['tneighbour'] = tf.constant(2., tf.float32)
    deserialized['tcell'] = tf.constant(0., tf.float32)
    deserialized['tenergy'] = tf.constant(500., tf.float32)
    deserialized['tlocmax'] = tf.constant(250., tf.float32)
    deserialized['tnum'] = tf.constant(3, tf.int32)
    
    return deserialized


# In[6]:


DATA_DIR = '/groups/hep/mojen/repositories/DREAM/Run/final_run/tauolaevts/10000'
#DATA_DIR = '/home/jupyter/DREAM'
BATCH_SIZE = 32
BUFFER_SIZE = 64

filename = [os.path.join(DATA_DIR, 'filtered.tfrecord')]
dataset = tf.data.TFRecordDataset(filename, compression_type='GZIP', buffer_size=BUFFER_SIZE)
dataset = dataset.map(parser_fn)
clusters_dataset = dataset.map(make_clusters)
features_dataset = clusters_dataset.map(extract_features)
S_serialized_features_dataset = features_dataset.map(lambda x: tf_serialize_example(x, 'S'))
C_serialized_features_dataset = features_dataset.map(lambda x: tf_serialize_example(x, 'C'))


# In[15]:


# list(dataset.map(lambda x: tf.reduce_max(x['S_image'])).as_numpy_iterator())


# In[10]:


import datetime
start = datetime.datetime.now()

writer = tf.data.experimental.TFRecordWriter(os.path.join(DATA_DIR, fileName))
if fileName == 'S_cluster.tfrecord':
	writer.write(S_serialized_features_dataset)
elif fileName == 'C_cluster.tfrecord':
	writer.write(C_serialized_features_dataset)
else:
	sys.exit()


end = datetime.datetime.now()
print(end-start)

## In[12]:
#
#
#import datetime
#print(datetime.datetime.now())
#
#
## In[8]:
#
#
#writer = tf.data.experimental.TFRecordWriter(os.path.join(DATA_DIR, 'C_cluster.tfrecord'))
#writer.write(C_serialized_features_dataset)
#
#
## In[9]:
#
#
#datetime.datetime.now()
#
#
## In[51]:
#
#
#filenames = [filename]
#raw_dataset = tf.data.TFRecordDataset(filenames)
#for raw_record in raw_dataset.take(4):
#    example = tf.train.Example()
#    example.ParseFromString(raw_record.numpy())
#    print(example)
#
#
## In[8]:
#
#
#for output in clusters_dataset.take(10):
#    plot_output(output)
#
#
## In[13]:
#
#
#import ROOT
#from array import array
#
#def plot_output(output):
#    h = ROOT.TH2F("","", 284, 0., 284., 284, 0., 284.)
#    c = ROOT.TCanvas("c1","c1", 800, 400)
#    c.Divide(2)
#    ROOT.gStyle.SetOptStat(0)
#    palette = [ROOT.kRed, ROOT.kGreen, ROOT.kYellow, ROOT.kBlue, ROOT.kRed, ROOT.kGreen, ROOT.kYellow, ROOT.kBlue]
#    for l,X in enumerate(['S', 'C']):
#        c.cd(l+1)
#        ROOT.gPad.SetLeftMargin(0.05)
#        ROOT.gPad.SetBottomMargin(0.05)
#        ROOT.gPad.SetRightMargin(0.15)
#        ROOT.gPad.SetTopMargin(0.15)
#        X_image = output[X+'_image']
#        indices = tf.where(X_image)
#        values = tf.reshape(tf.gather_nd(X_image, indices), [-1,1])
#        X_image = tf.concat([tf.cast(indices, values.dtype), values], axis=1)
#
#        htot = h.Clone()
#        for i,j,x in X_image:
#            htot.Fill(j,i,x)
#        htot.DrawCopy("COLZ")
#
#        X_cluster = output[X+'_cluster'].numpy()
#        if X_cluster.size != 0:
#            hk = []
#            u = np.unique(X_cluster[:,0], axis=0).astype(int)
#            arr = array('d',[0.5])
#            for k in range(len(u)):
#                for i,j,x in X_cluster[X_cluster[...,0]==u[k]][:,1:]:
#                    hk.append(h.Clone())
#                    hk[k].Fill(j,i,x)
#
#            for k in range(len(u)):
#                hk[k].SetLineColor(palette[k])
#                hk[k].SetLineWidth(1)
#                hk[k].SetContour(1, arr)
#                hk[k].DrawCopy("cont3 list same")
#
#    c.SaveAs("cluster"+str(output['eventId'][0].numpy())+".png")
#
#
## In[ ]:
#
#
#
#
#
## In[15]:
#
#
#import pandas as pd
#
#
## In[26]:
#
#
#left = pd.DataFrame({'key1': ['K0', 'K0', 'K1', 'K2'],
#                     'A': ['A0', 'A1', 'A2', 'A3'],
#                     'B': ['B0', 'B1', 'B2', 'B3']})
#right = pd.DataFrame({'key1': ['K0', 'K0', 'K0', 'K2'],
#                      'C': ['C0', 'C1', 'C2', 'C3'],
#                      'D': ['D0', 'D1', 'D2', 'D3']})
#
#pd.merge(left, right, how='left', on=['key1'])
#
#
## In[2]:
#
#
#import tensorflow as tf
#tf.constant([1.])/tf.constant([0.])
#
#
## In[ ]:




