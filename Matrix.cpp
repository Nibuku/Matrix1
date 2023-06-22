#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <utility>


using namespace std;

class Point {
public:
	int row;
	int col;
	Point(int row, int col) : row(row), col(col) {}
};
Point& operator+=(Point& a, const Point& b) {
	a.row += b.row;
	a.col += b.col;
	return a;
}
Point operator+(Point a, const Point& b) {
	return a += b;
}
Point& operator-=(Point& a, const Point& b) {
	a.row -= b.row;
	a.col -= b.col;
	return a;
}
Point operator-(Point a, const Point& b) {
	return a -= b;
}

using Size = Point;

class Matrix {
public:
	virtual Size size() const = 0;
	virtual float operator[](Point p) const = 0;

	bool is_inside(Point p) const {
		const auto s = size();
		return
			p.row >= 0 && p.row < s.row
			&&
			p.col >= 0 && p.col < s.col;
	}

	Matrix(const Matrix&) = delete;
	Matrix& operator=(const Matrix&) = delete;
	virtual ~Matrix() = default;
protected:
	Matrix() = default;
};

class BufferMatrix : public Matrix {
	vector <float> _data;
	Size _size;
public:
	BufferMatrix(Size size) :
		_data(size.row* size.col),
		_size(size) {}


	virtual Size size() const override
	{
		return _size;
	}

	virtual float operator[](Point p) const override
	{
		return _data[p.row * _size.col + p.col];
	};
	float& operator[](Point p)
	{
		return _data[p.row * _size.col + p.col];
	};

};
class SubMatrix : public Matrix {
	const Matrix& _m;
	Point _p;

public:
	SubMatrix(const Matrix& m, Point p) : _m(m), _p(p) {}

	virtual Size size() const override
	{
		return _m.size() - Size(1, 1);
	}

	virtual float operator[](Point p) const override
	{
		if (p.row >= _p.row)
			++p.row;
		if (p.col >= _p.col)
			++p.col;


		return _m[p];
	}

};


pair <Point, Point> my_find(const Matrix& m) {
	Point start(0, 0);
	Point step(0, 1);
	int zeros = 0;
	const auto s = m.size();
	for (int i = 0; i < s.row; ++i) {
		int count = 0;
		for (int j = 0; j < s.col; ++j) {
			if (m[{i, j}] == 0)
				++count;
		}
		if (count > zeros) {
			start = Point(i, 0);
			zeros = count;
		}
	}

	for (int j = 0; j < s.col; ++j) {
		int count = 0;
		for (int i = 0; i < s.row; ++i) {

			if (m[{i, j}] == 0)
				++count;
		}
		if (count > zeros) {
			start = Point(0, j);
			step = Point(1, 0);
			zeros = count;
		}
	}

	return { start,step };
}

float det(const Matrix& m) {
	const auto s = m.size();

	if (s.row != s.col)
		throw exception("row!=col");

	if (s.row == 2)
		return m[{0, 0}] * m[{1, 1}] - m[{0, 1}] * m[{1, 0}];

	float sum = 0;
	const auto start_step = my_find(m);
	const auto start = start_step.first;
	const auto step = start_step.second;
	for (auto p = start; m.is_inside(p); p += step) {
		if (m[p] != 0) {
			SubMatrix sm(m, p);
			sum += pow(-1, p.row + p.col) * m[p] * det(sm);
		}
	}
	return sum;
}


int main() {
	BufferMatrix m({ 2,2 });
	// 2 1
	// 5 4
	m[{0, 0}] = 2; 
	m[{0, 1}] = 1;
	m[{1, 0}] = 5;
	m[{1, 1}] = 4;

	cout << det(m) << endl;
}



