class Point {
	Int x;
	Int y;

	func self(): Point {
		return this;
	}
}

func main(): Int {
	var p1 = new Point();
	p1.x = 4;
	p1.y = 2;
	return p1.self().x + p1.self().y;
}