# misc -- all kinds of stuff

short_weird_behavior {

  // I am taking a closer look at these 3 lines of code that result in undefined behavior
  
  int a = 70000;
  short* k = (short*)(&a);
  *k = 300;
  
}
