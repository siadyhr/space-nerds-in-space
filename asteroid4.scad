
$fn=16;

rotate(a = 27, v = [0.7, 0.3, 0.1])
union() {
scale(v = [2, 1.5, 1.5])
	sphere(r = 15);
translate(v = [6, 5, 14])
	scale(v = [1.3, 0.8, 1.5])
		sphere(r = 20);
}

