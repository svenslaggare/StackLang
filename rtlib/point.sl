class Point {
	private Int x;
	private Int y;

	Point(Int getX, Int getY) {
		this.x = getX;
		this.y = getY;
	}

	func getX(): Int {
		return x;
	}

	func getY(): Int {
    	return y;
    }

	private func squareX(): Int {
		return x * x;
	}

	private func squareY(): Int {
		return y * y;
	}

	func length(): Int {
		return std::math::sqrt(cast<Float>(x * x + y * y));
	}

	func normalize(): Void {
		var len = this.length();
		x /= len;
		y /= len;
	}
}