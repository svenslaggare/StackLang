func cond1(): Bool {
	std::println(4);
	return false;
}

func cond2(): Bool {
	std::println(5);
	return true;
}

func cond3(): Bool {
	std::println(6);
	return true;
}

func main(): Int {
	#if (cond1() && cond2() && cond3()) {
	if (cond1() || cond2() || cond3()) {
		return 1;
	} else {
		return 0;
	}
}