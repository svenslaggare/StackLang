func newIntArray(Int len): Int[] {
	return new Int[len];
}

func printbool(Bool b): Void {
	if (b) {
		std::println(1);
	} else {
		std::println(0);
	}
}

func printLength(Int[] ints): Void {
	std::println(ints.length);
}

func printLength(Bool[] bools): Void {
	std::println(bools.length);
}

func printLength(Int[][] matrix): Void {
	#std::println(matrix.length);
	#std::println(matrix[0].length);
}

func main(): Int {
	var matrix = new Int[5];
	printLength(matrix);
	std::gc::collect();
	return 0;
}