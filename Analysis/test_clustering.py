import tensorflow as tf
import numpy as np
from ipynb.fs.full.clustering import finding_seeds

def test_finding_seeds():
	# arrange
	m = np.zeros([4,4], dtype=np.float32)
	m[1:3,1] = 1; m[3,2] = 1; m[0,3] = 1
	tseed = 0.5
	# act
	result = finding_seeds(m, tseed)
	# assert
	expected = tf.constant([[1,0,3], [2,1,1], [2,2,1], [2,3,2]], dtype=tf.int64)
	assert tf.reduce_all(tf.equal(result, expected))