class Point {
	Float x;
	Float y;

	private func length(): Float {
		return std::math::sqrt(x * x + y * y);
	}

	func normalize(): Void {
		var len = this.length();
		x /= len;
		y /= len;
	}
}

func main(): Int {
	var p = new Point();
	p.x = 2.0;
	p.y = 4.0;
	p.normalize();
	return 0;
}