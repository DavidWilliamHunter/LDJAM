/*
 * Colour model
 */


class Colour {
  public:
    unsigned char r;    // levels between 0 and 3
    unsigned char g;

  public:
    Colour() : r(0), g(0) {};
    Colour(unsigned char r_, unsigned char g_) : r(r_), g(g_) {};
};



