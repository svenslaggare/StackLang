func printInts(): Void {
	for (Int x = 0; x < 10; x += 1) {
		std::println(x);
	}

	Int x = 0;
	while (x < 10) {
		println(x);
		x += 1;

		if (x > 4) {
			return;
		}
	}
} 

func absSelf(Int x): Int {
	if (x < 0) {
		return -x;
	} else {
		return x;
	}
}

func troll(Int x, Int y): Int {
	Int z = x + y;

	if (y > 5 && y < 10) {
		z = x + 2;
	}

	Bool b = y > 5;

	if (!b) {
		return -1;
	}

	return absSelf(-x) + 5;
}