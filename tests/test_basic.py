import sys
import pytest
# from pytest import approx
# from numpy.testing import assert_array_equal, assert_allclose


def test_version():
    import curtains.version

    assert isinstance(curtains.version.version, str)
    print("FUN WITH TESTS", file=sys.stderr)


def test_any():
    assert 1 + 1 == 2
    assert True


def indep_func():
    raise ValueError("hello error")


def test_indep_func():
    with pytest.raises(ValueError, match="hello"):
        indep_func()
