class Point {
	Float x;
	Float y;

	func length(): Float {
		return std::math::sqrt(x * x + y * y);
	}

	func distance(Point other): Float {
		var deltaX = this.x - other.x;
		var deltaY = this.y - other.y;
		return std::math::sqrt(deltaX * deltaX + deltaY * deltaY);
	}

	func self(): Point {
		return this;
	}
}

class string {
	Char[] chars;
}

func main(): Int {
	var p1 = new Point();
	p1.x = 4.0;
	p1.y = 2.0;
	std::println(p1.self().length());

	var p2 = new Point();
	p2.x = 3.0;
	p2.y = 2.1;

	std::println(p1.distance(p2));

	return 0;
}