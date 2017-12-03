// locations are unsigned int and 0 is a valid co-ordinate, so Bad location is 255
#define BAD_LOC 127
template<typename T>
class Vec2T
{
  public:
    T x;
    T y;  

  public:
    Vec2T() : x(0), y(0) {};
    Vec2T(T x_, T y_) : x(x_), y(y_) {};
    Vec2T(const Vec2T &rhs) : x(rhs.x), y(rhs.y) {}; 

    bool isValid() const { return x<8 && y<8; };

    void print() { Serial.print("("); Serial.print(x); Serial.print(","); Serial.print(y); Serial.print(")"); };    
    void println() { Serial.print("("); Serial.print(x); Serial.print(","); Serial.print(y); Serial.println(")"); };
};



