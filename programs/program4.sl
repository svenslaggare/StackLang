func absFloat(Float x): Float {
	if (x < 0.0) {
		return -x;
	} else {
		return x;
	}
}

func absInt(Int x): Int {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

func main(): Int {
	var x = absFloat(-5.5);
	var y = absInt(-5);

	std::println(Float(y));
	std::println(Int(x));
	return 0;
}