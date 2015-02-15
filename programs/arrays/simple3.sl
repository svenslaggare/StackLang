func printLength(Int[] array): Void {
	std::println(array.length);
}

func main(): Int {
	Int[] array = new Int[4];
	printLength(array);
	return 0;
}