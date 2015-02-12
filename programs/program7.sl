class Point {
	Float x;
	Float y;

	func length(): Float {
		return std::math::sqrt(x * x + y * y);
	}
}