func main(): Int {
	return fib(8);
}

func fib(Int n): Int {
	if (n <= 1) {
		return n;
	} else {
		return fib(n - 1) + fib(n - 2);
	}
}