func printdensity(Float d): Void {
	if (d > 8.0) {
		std::printchar(32); # ' '
	} else {
		if (d > 4.0) {
			std::printchar(46); # '.'
		} else {
			if (d > 2.0) {
				std::printchar(43); # '+'
			} else {
				std::printchar(42); # '*'
			}
		}
	}
}

func mandleconverger(Float real, Float imag, Float iters, Float creal, Float cimag): Float {
	if (iters > 255.0 || (real*real + imag*imag) > 4.0) {
		return iters;
	} else {
		return mandleconverger(
			real*real - imag*imag + creal,
			2.0*real*imag + cimag,
			iters+1.0, creal, cimag);
	}
}

func mandleconverge(Float real, Float imag): Float {
	return mandleconverger(real, imag, 0.0, real, imag);
}

func mandelhelp(Float xmin, Float xmax, Float xstep, Float ymin, Float ymax, Float ystep): Void {
	for (var y = ymin; y < ymax; y += ystep) {
		for (var x = xmin; x < xmax; x += xstep) {
			printdensity(mandleconverge(x, y));
		}

		std::printchar(10);
	}
}

func mandel(Float realstart, Float imagstart, Float realmag, Float imagmag): Void {
	mandelhelp(
		realstart, realstart+realmag*78.0, realmag,
		imagstart, imagstart+imagmag*40.0, imagmag);
}

func main(): Int {
	mandel(-2.3, -1.3, 0.05, 0.07);
	return 0;
}