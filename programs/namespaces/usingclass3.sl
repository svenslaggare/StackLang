namespace math {
	class Point {
		Int x;
		Int y;
	}
}

using namespace math;

func copyPoint(Point p): Point {
	Point c = new Point();
	c.x = p.x;
	c.y = p.y;
	return c;
}

func main(): Int {
	Point p = new Point();
	p.x = 4;
	p.y = 2;
	Point p2 = copyPoint(p);
	return p2.x + p2.y;
}