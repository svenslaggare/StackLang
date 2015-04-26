class Point {
	Int x;
	Int y;
}

func main(): Int {
	Point[] ps = new Point[4];
	ps[0] = new Point();
	ps[0].x = 2;
	ps[0].y = 4;
	return ps[0].x + ps[0].y;
}