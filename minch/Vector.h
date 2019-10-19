#ifndef VECTOR_H
#define VECTOR_H

class Vector {
 private:
  float x_, y_, z_;
  float arr_[3];

 public:
  Vector(float x = 0.0f, float y = 0.0f, float z = 0.0f);
  Vector(const float components[3]);
  ~Vector();

  static const Vector Zero();
  static const Vector One();
  static Vector Random();

  float Length() const;
  Vector ProjectOnto(const Vector& other);
  Vector Normed() const;
  Vector Cross(const Vector& other) const;
  Vector Clamped(float x_clamp, float y_clamp, float z_clamp);
  Vector Clamped(float c);

  Vector operator/(const float scalar) const;

  /// <summary> Takes the dot product of two vectors </summary>
  float operator*(const Vector& other) const;
  friend Vector operator*(const float scalar, const Vector& v);
  friend Vector operator*(const Vector& v, const float scalar);

  Vector operator+(const Vector& other) const;

  Vector operator-(const Vector& other) const;

  float operator[](const unsigned int i) const;

  bool operator==(const Vector& other) const;
};

#endif