#!/usr/bin/env python

import unittest
import os
import tempfile
import shutil

class BaseTest(unittest.TestCase):
	"""
	Base test for all QFramework test cases. This class provides a
	temporary directory which can be used to create external files. The
	directory is deleted when all test cases have been executed.
	"""

	def setUp(self):
		"""
		Create temporary directory.
		"""
		self.tempdir = tempfile.mkdtemp(prefix="qftest_")

	def tearDown(self):
		"""
		Delete temporary directory.
		""" 	
		if os.path.exists(self.tempdir):
			shutil.rmtree(self.tempdir)
