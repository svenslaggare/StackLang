class Point {
	Float x;
	Float y;

	Point(Float inX, Float inY) {
		x = inX;
		y = inY;
	}
}

func main(): Int {
	var p1 = new Point(1.0, 2.0);
	std::println(p1.x);
	std::println(p1.y);
	return 0;
}