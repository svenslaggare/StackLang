class Point {
	Float x;
	Float y;

	func length(): Float {
		return std::math::sqrt(x * x + y * y);
	}
}

func main(): Int {
	Point p = new Point();
	p.x = 4.0 + 3.0;
	std::println(p.x);
	return 0;
}