func factorial(Int n): Int {
	if (n <= 1) {
		return 1;
	} else {
		return n * factorial(n - 1);
	}
}

func fib(Int n): Int {
	if (n <= 1) {
		return n;
	} else {
		return fib(n - 1) + fib(n - 2);
	}
}

func boolToInt(Bool b): Int {
	if (b) {
		return 1;
	} else {
		return 0;
	}
}

func main(): Int {
	2 + 4;
	return fib(20);
}