# misc -- all kinds of stuff


In CPP:


	// I am taking a closer look at these lines of code that result in undefined behavior
	short_weird_behavior {int a = 70000; short* k = (short*)(&a); *k = 300;}
