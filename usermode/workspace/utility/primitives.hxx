#pragma once

namespace External::Primitives
{
	template <typename T>
	struct vector2_t
	{
		T x, y;

		vector2_t() : x(0), y(0) {}
		vector2_t(T _x, T _y) : x(_x), y(_y) {}
		~vector2_t() = default;

		[[nodiscard]] T Dot(vector2_t<T> v) const noexcept
		{
			return x * v.x + y * v.y;
		}

		[[nodiscard]] T SizeSquared() const noexcept
		{
			return x * x + y * y;
		}

		[[nodiscard]] T Length() const noexcept
		{
			return std::sqrt(SizeSquared());
		}

		[[nodiscard]] T Distance(vector2_t<T> v) const noexcept
		{
			vector2_t<T> delta{
				x - v.x,
				y - v.y
			};

			return std::sqrt(delta.SizeSquared());
		}

		[[nodiscard]] vector2_t<T> Normalize() const noexcept
		{
			double length = std::sqrt(static_cast<double>(SizeSquared()));
			if (length <= 1.0e-6)
				return vector2_t<T>{ 0, 0 };

			return vector2_t<T>{
				x / static_cast<T>(length),
				y / static_cast<T>(length)
			};
		}

		bool operator==(const vector2_t<T>& v) const noexcept
		{
			return x == v.x && y == v.y;
		}

		bool operator!=(const vector2_t<T>& v) const noexcept
		{
			return x != v.x || y != v.y;
		}

		[[nodiscard]] vector2_t<T> operator+(const vector2_t<T>& v) const noexcept
		{
			return vector2_t<T>(x + v.x, y + v.y);
		}

		[[nodiscard]] vector2_t<T> operator-(const vector2_t<T>& v) const noexcept
		{
			return vector2_t<T>(x - v.x, y - v.y);
		}

		[[nodiscard]] vector2_t<T> operator*(const vector2_t<T>& v) const noexcept
		{
			return vector2_t<T>(x * v.x, y * v.y);
		}

		[[nodiscard]] vector2_t<T> operator/(const vector2_t<T>& v) const noexcept
		{
			return vector2_t<T>(x / v.x, y / v.y);
		}

		[[nodiscard]] vector2_t<T> operator*(T scalar) const noexcept
		{
			return vector2_t<T>(x * scalar, y * scalar);
		}

		[[nodiscard]] vector2_t<T> operator/(T scalar) const noexcept
		{
			return vector2_t<T>(x / scalar, y / scalar);
		}

		[[nodiscard]] T& operator[](size_t i) noexcept
		{
			T t = 0;
			if (i >= 2 || i < 0)
				return t;

			return (&x)[i];
		}

		[[nodiscard]] const T& operator[](size_t i) const noexcept
		{
			if (i >= 2 || i < 0)
				return static_cast<T>(0);

			return (&x)[i];
		}
	};

	template <typename T>
	struct vector3_t
	{
		T x, y, z;

		vector3_t() : x(0), y(0), z(0) {}
		vector3_t(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
		~vector3_t() = default;

		[[nodiscard]] T Dot(vector3_t<T> v) const noexcept
		{
			return x * v.x + y * v.y + z * v.z;
		}

		[[nodiscard]] T SizeSquared() const noexcept
		{
			return x * x + y * y + z * z;
		}

		[[nodiscard]] T Length() const noexcept
		{
			return std::sqrt(SizeSquared());
		}

		[[nodiscard]] T Distance(vector3_t<T> v) const noexcept
		{
			vector3_t<T> delta{
				x - v.x,
				y - v.y,
				z - v.z
			};

			return std::sqrt(delta.SizeSquared());
		}

		[[nodiscard]] vector3_t<T> Normalize() const noexcept
		{
			double length = std::sqrt(static_cast<double>(SizeSquared()));
			if (length <= 1.0e-6)
				return vector3_t<T>{ 0, 0, 0 };

			return vector3_t<T>{
				x / static_cast<T>(length),
				y / static_cast<T>(length),
				z / static_cast<T>(length)
			};
		}

		bool operator==(const vector3_t<T>& v) const noexcept
		{
			return x == v.x && y == v.y && z == v.z;
		}

		bool operator!=(const vector3_t<T>& v) const noexcept
		{
			return x != v.x || y != v.y || z != v.z;
		}

		[[nodiscard]] vector3_t<T> operator+(const vector3_t<T>& v) const noexcept
		{
			return vector3_t<T>(x + v.x, y + v.y, z + v.z);
		}

		[[nodiscard]] vector3_t<T> operator-(const vector3_t<T>& v) const noexcept
		{
			return vector3_t<T>(x - v.x, y - v.y, z - v.z);
		}

		[[nodiscard]] vector3_t<T> operator*(const vector3_t<T>& v) const noexcept
		{
			return vector3_t<T>(x * v.x, y * v.y, z * v.z);
		}

		[[nodiscard]] vector3_t<T> operator/(const vector3_t<T>& v) const noexcept
		{
			return vector3_t<T>(x / v.x, y / v.y, z / v.z);
		}

		[[nodiscard]] vector3_t<T> operator*(T scalar) const noexcept
		{
			return vector3_t<T>(x * scalar, y * scalar, z * scalar);
		}

		[[nodiscard]] vector3_t<T> operator/(T scalar) const noexcept
		{
			return vector3_t<T>(x / scalar, y / scalar, z / scalar);
		}

		[[nodiscard]] T& operator[](size_t i) noexcept
		{
			T t = 0;
			if (i >= 3 || i < 0)
				return t;

			return (&x)[i];
		}

		[[nodiscard]] const T& operator[](size_t i) const noexcept
		{
			if (i >= 3 || i < 0)
				return static_cast<T>(0);

			return (&x)[i];
		}
	};

	template <typename T>
	struct vector4_t
	{
		T x, y, z, w;

		vector4_t() : x(0), y(0), z(0), w(0) {}
		vector4_t(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
		~vector4_t() = default;

		[[nodiscard]] T Dot(vector4_t<T> v) const noexcept
		{
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		[[nodiscard]] T SizeSquared() const noexcept
		{
			return x * x + y * y + z * z + w * w;
		}

		[[nodiscard]] T Length() const noexcept
		{
			return std::sqrt(SizeSquared());
		}

		[[nodiscard]] T Distance(vector4_t<T> v) const noexcept
		{
			vector4_t<T> delta{
				x - v.x,
				y - v.y,
				z - v.z,
				w - v.w
			};

			return std::sqrt(delta.SizeSquared());
		}

		[[nodiscard]] vector4_t<T> Normalize() const noexcept
		{
			double length = std::sqrt(static_cast<double>(SizeSquared()));
			if (length <= 1.0e-6)
				return vector4_t<T>{ 0, 0, 0, 0 };

			return vector4_t<T>{
				x / static_cast<T>(length),
				y / static_cast<T>(length),
				z / static_cast<T>(length),
				w / static_cast<T>(length)
			};
		}

		bool operator==(const vector4_t<T>& v) const noexcept
		{
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}

		bool operator!=(const vector4_t<T>& v) const noexcept
		{
			return x != v.x || y != v.y || z != v.z || z != v.w;
		}

		[[nodiscard]] vector4_t<T> operator+(const vector4_t<T>& v) const noexcept
		{
			return vector4_t<T>(x + v.x, y + v.y, z + v.z, w + v.w);
		}

		[[nodiscard]] vector4_t<T> operator-(const vector4_t<T>& v) const noexcept
		{
			return vector4_t<T>(x - v.x, y - v.y, z - v.z, w - v.w);
		}

		[[nodiscard]] vector4_t<T> operator*(const vector4_t<T>& v) const noexcept
		{
			return vector4_t<T>(x * v.x, y * v.y, z * v.z, w * v.w);
		}

		[[nodiscard]] vector4_t<T> operator/(const vector4_t<T>& v) const noexcept
		{
			return vector4_t<T>(x / v.x, y / v.y, z / v.z, w / v.w);
		}

		[[nodiscard]] vector4_t<T> operator*(T scalar) const noexcept
		{
			return vector4_t<T>(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		[[nodiscard]] vector4_t<T> operator/(T scalar) const noexcept
		{
			return vector4_t<T>(x / scalar, y / scalar, z / scalar, w / scalar);
		}

		[[nodiscard]] T& operator[](size_t i) noexcept
		{
			T t = 0;
			if (i >= 4 || i < 0)
				return t;

			return (&x)[i];
		}

		[[nodiscard]] const T& operator[](size_t i) const noexcept
		{
			if (i >= 4 || i < 0)
				return static_cast<T>(0);

			return (&x)[i];
		}
	};

	template <typename T>
	struct matrix3x3_t
	{
		vector3_t<T> v1, v2, v3;

		matrix3x3_t() : v1(), v2(), v3() {}
		matrix3x3_t(const vector3_t<T>& _v1, const vector3_t<T>& _v2, const vector3_t<T>& _v3)
			: v1(_v1), v2(_v2), v3(_v3) {}
		~matrix3x3_t() = default;

		[[nodiscard]] matrix3x3_t<T> operator+(const matrix3x3_t<T>& m) const noexcept
		{
			return matrix3x3_t<T>(
				v1 + m.v1,
				v2 + m.v2,
				v3 + m.v3
			);
		}

		[[nodiscard]] matrix3x3_t<T> operator-(const matrix3x3_t<T>& m) const noexcept
		{
			return matrix3x3_t<T>(
				v1 - m.v1,
				v2 - m.v2,
				v3 - m.v3
			);
		}

		[[nodiscard]] matrix3x3_t<T> operator*(const matrix3x3_t<T>& m) const noexcept
		{
			return matrix3x3_t<T>(
				vector3_t<T>(
					v1.x * m.v1.x + v1.y * m.v2.x + v1.z * m.v3.x + v1.w * m.v4.x,
					v1.x * m.v1.y + v1.y * m.v2.y + v1.z * m.v3.y + v1.w * m.v4.y,
					v1.x * m.v1.z + v1.y * m.v2.z + v1.z * m.v3.z + v1.w * m.v4.z
				),
				vector3_t<T>(
					v2.x * m.v1.x + v2.y * m.v2.x + v2.z * m.v3.x + v2.w * m.v4.x,
					v2.x * m.v1.y + v2.y * m.v2.y + v2.z * m.v3.y + v2.w * m.v4.y,
					v2.x * m.v1.z + v2.y * m.v2.z + v2.z * m.v3.z + v2.w * m.v4.z
				),
				vector3_t<T>(
					v3.x * m.v1.x + v3.y * m.v2.x + v3.z * m.v3.x + v3.w * m.v4.x,
					v3.x * m.v1.y + v3.y * m.v2.y + v3.z * m.v3.y + v3.w * m.v4.y,
					v3.x * m.v1.z + v3.y * m.v2.z + v3.z * m.v3.z + v3.w * m.v4.z
				)
			);
		}

		[[nodiscard]] matrix3x3_t<T> operator/(const matrix3x3_t<T>& m) const noexcept
		{
			return matrix3x3_t<T>(
				v1 / m.v1,
				v2 / m.v2,
				v3 / m.v3
			);
		}
	};

	template <typename T>
	struct matrix4x4_t
	{
		vector4_t<T> v1, v2, v3, v4;

		matrix4x4_t() : v1(), v2(), v3(), v4() {}
		matrix4x4_t(const vector4_t<T>& _v1, const vector4_t<T>& _v2, const vector4_t<T>& _v3, const vector4_t<T>& _v4)
			: v1(_v1), v2(_v2), v3(_v3), v4(_v4) {
		}
		~matrix4x4_t() = default;

		[[nodiscard]] matrix4x4_t<T> operator+(const matrix4x4_t<T>& m) const noexcept
		{
			return matrix4x4_t<T>(
				v1 + m.v1,
				v2 + m.v2,
				v3 + m.v3,
				v4 + m.v4
			);
		}

		[[nodiscard]] matrix4x4_t<T> operator-(const matrix4x4_t<T>& m) const noexcept
		{
			return matrix4x4_t<T>(
				v1 - m.v1,
				v2 - m.v2,
				v3 - m.v3,
				v4 - m.v4
			);
		}

		[[nodiscard]] matrix4x4_t<T> operator*(const matrix4x4_t<T>& m) const noexcept
		{
			return matrix4x4_t<T>(
				vector4_t<T>(
					v1.x * m.v1.x + v1.y * m.v2.x + v1.z * m.v3.x + v1.w * m.v4.x,
					v1.x * m.v1.y + v1.y * m.v2.y + v1.z * m.v3.y + v1.w * m.v4.y,
					v1.x * m.v1.z + v1.y * m.v2.z + v1.z * m.v3.z + v1.w * m.v4.z,
					v1.x * m.v1.w + v1.y * m.v2.w + v1.z * m.v3.w + v1.w * m.v4.w
				),
				vector4_t<T>(
					v2.x * m.v1.x + v2.y * m.v2.x + v2.z * m.v3.x + v2.w * m.v4.x,
					v2.x * m.v1.y + v2.y * m.v2.y + v2.z * m.v3.y + v2.w * m.v4.y,
					v2.x * m.v1.z + v2.y * m.v2.z + v2.z * m.v3.z + v2.w * m.v4.z,
					v2.x * m.v1.w + v2.y * m.v2.w + v2.z * m.v3.w + v2.w * m.v4.w
				),
				vector4_t<T>(
					v3.x * m.v1.x + v3.y * m.v2.x + v3.z * m.v3.x + v3.w * m.v4.x,
					v3.x * m.v1.y + v3.y * m.v2.y + v3.z * m.v3.y + v3.w * m.v4.y,
					v3.x * m.v1.z + v3.y * m.v2.z + v3.z * m.v3.z + v3.w * m.v4.z,
					v3.x * m.v1.w + v3.y * m.v2.w + v3.z * m.v3.w + v3.w * m.v4.w
				),
				vector4_t<T>(
					v4.x * m.v1.x + v4.y * m.v2.x + v4.z * m.v3.x + v4.w * m.v4.x,
					v4.x * m.v1.y + v4.y * m.v2.y + v4.z * m.v3.y + v4.w * m.v4.y,
					v4.x * m.v1.z + v4.y * m.v2.z + v4.z * m.v3.z + v4.w * m.v4.z,
					v4.x * m.v1.w + v4.y * m.v2.w + v4.z * m.v3.w + v4.w * m.v4.w
				)
			);
		}

		[[nodiscard]] matrix4x4_t<T> operator/(const matrix4x4_t<T>& m) const noexcept
		{
			return matrix4x4_t<T>(
				v1 / m.v1,
				v2 / m.v2,
				v3 / m.v3,
				v4 / m.v4
			);
		}
	};
}