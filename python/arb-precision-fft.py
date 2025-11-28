import numpy as np
import mpmath as mp

sin = np.vectorize(mp.sin)
cos = np.vectorize(mp.cos)
tan = np.vectorize(mp.tan)
exp = np.vectorize(mp.exp)
sqrt = np.vectorize(mp.sqrt)
real = np.vectorize(mp.re)
imag = np.vectorize(mp.im)


def arange(*args):
    """Returns a numpy array with n repetitions of mp.mpf(0)"""
    return np.array(mp.arange(*args))


def fft(yg):
    """
    Performs an fft in arbitrary precision arithmetic.
    Arithmetic is implemented via the package mpmath
    Precision should be set by setting the variable mp.mp.dps to the desired precision

    Input:
    yg: 1D numpy vector, with yg.shape[0] a power of 2
        elements of yg should be of type mp.mpc

    Currently, the input vector must have a length given by a power of 2
    The algorithm used here was based on code posted at https://jakevdp.github.io/blog/2013/08/28/understanding-the-fft/
    """
    N = yg.shape[0]
    if np.log2(N) % 1 > 0:
        raise ValueError("size of x must be a power of 2")
    N_min = min(N, 8)
    # Perform an O[N^2] DFT on all length-N_min sub-problems at once
    n = arange(N_min)
    k = n[:, None]
    A = -mp.mpc(2j) * mp.pi * n * k / mp.mpf(N_min)
    M = exp(A)
    X = M.dot(yg.reshape((N_min, -1)))
    while X.shape[0] < N:
        xslice = int(X.shape[1] / 2)
        X_even = X[:, :xslice]
        X_odd = X[:, xslice:]
        A = -mp.mpc(1j) * mp.pi * arange(X.shape[0]) / mp.mpf(X.shape[0])
        factor = exp(A)[:, None]
        X = np.vstack([X_even + factor * X_odd,
                       X_even - factor * X_odd])
    # build-up each level of the recursive calculation all at once
    return X.ravel()


mp.mp.dps = 100
# set size of transform to test
N = 16

# construct
x = mp.linspace(mp.mpf(0), mp.mpf(2.0)*mp.pi, 16, endpoint=False)
y = sin(x)

breakpoint()

# take the fft
yh = fft(y)
