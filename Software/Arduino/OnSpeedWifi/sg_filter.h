const char jsSGfilter[] PROGMEM = R"=====(
<script language="javascript" type="text/javascript">
//General Least-Squares Smoothing and Differentiation by the Convolution (Savitzky-Golay) Method Peter A. Gorry
// Filter adapted from https://github.com/mljs/savitzky-golay-generalized

function SavitzkyGolay(ys, xs, options = {}) {
  let { windowSize = 9, derivative = 0, polynomial = 3 } = options;

  if (windowSize % 2 === 0 || windowSize < 5 || !Number.isInteger(windowSize)) {
    throw new RangeError(
      'Invalid window size (should be odd and at least 5 integer number)',
    );
  }
  if (windowSize > ys.length) {
    throw new RangeError(
      `Window size is higher than the data length ${windowSize}>${ys.length}`,
    );
  }
  if (derivative < 0 || !Number.isInteger(derivative)) {
    throw new RangeError('Derivative should be a positive integer');
  }
  if (polynomial < 1 || !Number.isInteger(polynomial)) {
    throw new RangeError('Polynomial should be a positive integer');
  }
  if (polynomial >= 6) {
    // eslint-disable-next-line no-console
    console.warn(
      'You should not use polynomial grade higher than 5 if you are' +
        ' not sure that your data arises from such a model. Possible polynomial oscillation problems',
    );
  }

  let half = Math.floor(windowSize / 2);
  let np = ys.length;
  let ans = new Array(np);
  let weights = fullWeights(windowSize, polynomial, derivative);
  let hs = 0;
  let constantH = true;
  if (Array.isArray(xs)) {
    constantH = false;
  } else {
    hs = Math.pow(xs, derivative);
  }

  //For the borders
  for (let i = 0; i < half; i++) {
    let wg1 = weights[half - i - 1];
    let wg2 = weights[half + i + 1];
    let d1 = 0;
    let d2 = 0;
    for (let l = 0; l < windowSize; l++) {
      d1 += wg1[l] * ys[l];
      d2 += wg2[l] * ys[np - windowSize + l];
    }
    if (constantH) {
      ans[half - i - 1] = d1 / hs;
      ans[np - half + i] = d2 / hs;
    } else {
      hs = getHs(xs, half - i - 1, half, derivative);
      ans[half - i - 1] = d1 / hs;
      hs = getHs(xs, np - half + i, half, derivative);
      ans[np - half + i] = d2 / hs;
    }
  }

  //For the internal points
  let wg = weights[half];
  for (let i = windowSize; i <= np; i++) {
    let d = 0;
    for (let l = 0; l < windowSize; l++) d += wg[l] * ys[l + i - windowSize];
    if (!constantH) hs = getHs(xs, i - half - 1, half, derivative);
    ans[i - half - 1] = d / hs;
  }
  return ans;
}

function getHs(h, center, half, derivative) {
  let hs = 0;
  let count = 0;
  for (let i = center - half; i < center + half; i++) {
    if (i >= 0 && i < h.length - 1) {
      hs += h[i + 1] - h[i];
      count++;
    }
  }
  return Math.pow(hs / count, derivative);
}

function GramPoly(i, m, k, s) {
  let Grampoly = 0;
  if (k > 0) {
    Grampoly =
      ((4 * k - 2) / (k * (2 * m - k + 1))) *
        (i * GramPoly(i, m, k - 1, s) + s * GramPoly(i, m, k - 1, s - 1)) -
      (((k - 1) * (2 * m + k)) / (k * (2 * m - k + 1))) *
        GramPoly(i, m, k - 2, s);
  } else {
    if (k === 0 && s === 0) {
      Grampoly = 1;
    } else {
      Grampoly = 0;
    }
  }
  return Grampoly;
}

function GenFact(a, b) {
  let gf = 1;
  if (a >= b) {
    for (let j = a - b + 1; j <= a; j++) {
      gf *= j;
    }
  }
  return gf;
}

function Weight(i, t, m, n, s) {
  let sum = 0;
  for (let k = 0; k <= n; k++) {
    //console.log(k);
    sum +=
      (2 * k + 1) *
      (GenFact(2 * m, k) / GenFact(2 * m + k + 1, k + 1)) *
      GramPoly(i, m, k, 0) *
      GramPoly(t, m, k, s);
  }
  return sum;
}

function fullWeights(m, n, s) {
  let weights = new Array(m);
  let np = Math.floor(m / 2);
  for (let t = -np; t <= np; t++) {
    weights[t + np] = new Array(m);
    for (let j = -np; j <= np; j++) {
      weights[t + np][j + np] = Weight(j, t, np, n, s);
    }
  }
  return weights;
}
</script>
)=====";
