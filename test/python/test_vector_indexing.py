import os
import sys
import unittest

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))
from Multiview import Vector3f

class TestVector3fLikeList(unittest.TestCase):
    def setUp(self):
        # Initialize vector and reference list
        self.v = Vector3f()
        self.v[:] = [10.0, 20.0, 30.0]
        self.ref = [10.0, 20.0, 30.0]

    def assertVectorEqualsList(self):
        for i in range(3):
            self.assertEqual(self.v[i], self.ref[i])

    def test_setitem_basic_assignment(self):
        # Test normal assignment to the whole vector
        self.v[:] = [1.0, 2.0, 3.0]
        self.ref[:] = [1.0, 2.0, 3.0]
        self.assertVectorEqualsList()

    def test_setitem_reverse_assignment(self):
        # Test reverse slice assignment
        self.v[::-1] = [3.0, 2.0, 1.0]
        self.ref[::-1] = [3.0, 2.0, 1.0]
        self.assertVectorEqualsList()

    def test_setitem_step_assignment(self):
        # Test slice step assignment
        self.v[::2] = [100.0, 300.0]
        self.ref[::2] = [100.0, 300.0]
        self.assertVectorEqualsList()

    def test_setitem_partial_assignment(self):
        # Test partial assignment
        self.v[1:] = [200.0, 300.0]
        self.ref[1:] = [200.0, 300.0]
        self.assertVectorEqualsList()

    def test_setitem_negative_start(self):
        # Test negative start in slice assignment
        self.v[-2:] = [222.0, 333.0]
        self.ref[-2:] = [222.0, 333.0]
        self.assertVectorEqualsList()

    def test_setitem_index_out_of_range(self):
        # Test assignment where slice is out of bounds
        with self.assertRaises(ValueError):
            self.v[::0] = [1.0, 2.0, 3.0]

    def test_setitem_index_error(self):
        # Test when out-of-bounds index is accessed
        with self.assertRaises(IndexError):
            self.v[3:] = [1.0]  # Start index is out of range
        with self.assertRaises(IndexError):
            self.v[-4:] = [1.0]  # Negative index out of range


if __name__ == '__main__':
    unittest.main()
