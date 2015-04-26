namespace math {
	class Point {
		Int x;
		Int y;
	}
}

using namespace math;

func newPoint(): Point {
	return new Point();
}

func main(): Int {
	Point p = newPoint();
	p.x = 4;
	p.y = 2;
	return p.x + p.y;
}