func max(Int x, Int y, Int z): Int {
	if (x > y) {
		if (x > z) {
			return x;
		} else {
			return z;
		}
	} else {
		if (y > z) {
			return y;
		} else {
			return z;
		}
	}
}