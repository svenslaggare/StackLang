func factorial(Int n): Int {
	if (n <= 1) {
		return 1;
	} else {
		return n * factorial(n - 1);
	}
}

func main(): Int {
	return factorial(5);
}