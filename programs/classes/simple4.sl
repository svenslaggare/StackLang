class Point {
	Float x;
	Float y;

	func length(): Float {
		return std::math::sqrt(x * x + y * y);
	}

	func self(): Point {
		return this;
	}
}

func main(): Int {
	var p1 = new Point();
	p1.x = 4.0;
	p1.y = 2.0;
	std::println(p1.self().length());
	return 0;
}