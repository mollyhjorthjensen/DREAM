import tensorflow as tf
import numpy as np
from ipynb.fs.full.clustering import *

def test_get_seedlist():
	# arrange
	m = np.zeros([4,4], np.float32)
	m[1:3,1] = 1; m[3,2] = 1; m[0,3] = 1
	m = tf.constant(m)
	tseed = tf.constant(0.5, m.dtype)
	# act
	result = get_seedlist(m, tseed)
	# assert
	expected = tf.constant([[1,0,3], [2,1,1], [2,2,1], [2,3,2]], tf.int64)
	assert tf.reduce_all(tf.equal(result, expected))

def test_sort_seedlist():
	# arrange
	m = np.zeros([2,2], np.float32)
	m[0,0] = 1; m[1,0] = 2; m[1,1] = 3
	m = tf.constant(m)
	tseed = tf.constant(0.5, m.dtype)
	s = get_seedlist(m, tseed)
	# act
	result = sort_seedlist(m, s)
	# assert
	expected = tf.constant([[1,1,1], [1,1,0], [1,0,0]], tf.int64)
	assert tf.reduce_all(tf.equal(result, expected))

def test_get_neighbours1():
	# arrange
	m = np.ones([4,5], np.float32)
	m = tf.constant(m)
	idx = tf.constant([0,0], tf.int64)
	dense_shape = tf.shape(m, idx.dtype)
	# act
	result = get_neighbours(idx, dense_shape)
	# assert
	expected = tf.constant([[0,1], [1,0], [1,1]], idx.dtype)
	assert tf.reduce_all(tf.equal(result, expected))

def test_get_neighbours2():
	# arrange
	m = np.ones([4,5], np.float32)
	m = tf.constant(m)
	idx = tf.constant([3,4], tf.int64)
	dense_shape = tf.shape(m, idx.dtype)
	# act
	result = get_neighbours(idx, dense_shape)
	# assert
	expected = tf.constant([[2,3], [2,4], [3,3]], idx.dtype)
	assert tf.reduce_all(tf.equal(result, expected))

def test_get_neighbours3():
	# arrange
	m = np.ones([4,5], np.float32)
	m = tf.constant(m)
	idx = tf.constant([0,2], tf.int64)
	dense_shape = tf.shape(m, idx.dtype)
	incl = tf.constant([[0,1], [0,2], [0,3], [1,3]], idx.dtype)
	excl = tf.constant([[1,3]], idx.dtype)
	# act
	result = get_neighbours(idx, dense_shape, incl, excl)
	# assert
	expected = tf.constant([[0,1], [0,3]], idx.dtype)
	assert tf.reduce_all(tf.equal(result, expected))