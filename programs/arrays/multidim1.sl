func printLength(Int[][] matrix): Void {
	std::println(matrix.length);
	std::println(matrix[0].length);
}

func main(): Int {
	var matrix = new Int[5, 5];
	printLength(matrix);
	matrix[4][4] = 1337;
	return matrix[4][4];
}