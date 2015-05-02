class Point {
	private Int x;
	private Int y;

	func add(Int newX, Int newY): Void {
		this.x = this.x + newX;
		this.y = this.y + newY;
	}

	func set(Int newX, Int newY): Void {
		this.x = newX;
		this.y = newY;
	}
}

func main(): Int {
	var p1 = new Point();
	p1.set(1, 4);
	return 0;
}