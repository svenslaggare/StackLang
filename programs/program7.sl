class Point {
	Float x;
	Float y;

	func length(): Float {
		return std::math::sqrt(x * x + y * y);
	}
}

class string {
	Char[] chars;
}

func main(): Int {
	Point p = new Point();
	p.x = 4.0;
	std::println(p.x);

	var str = new string();
	str.chars = "Troll";
	std::println(str.chars.length);

	return 0;
}