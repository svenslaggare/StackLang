namespace math {
	class Point {
		Int x;
		Int y;
	}
}

using namespace math;

func main(): Int {
	Point p = new Point();
	p.x = 4;
	p.y = 2;
	return p.x + p.y;
}