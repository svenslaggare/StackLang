namespace math {
	class Point {
		Int x;
		Int y;
	}
}

func main(): Int {
	math::Point p = new math::Point();
	p.x = 2;
	p.y = 3;
	return p.x + p.y;
}