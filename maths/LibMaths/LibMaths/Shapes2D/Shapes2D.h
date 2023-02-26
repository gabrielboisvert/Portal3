#pragma once
#define _USE_MATH_DEFINES_
#include <cmath>

#include "../Vec2/Vec2.h"

#ifndef HALF_CIRCLE
#define HALF_CIRCLE 180.0f
#endif

namespace lm
{
	template<typename T> using Point2D = Vec2<T>;
	typedef Point2D<float> point2D;

	template<typename T> class ITransform
	{
	public:
		virtual void translateX(const T value) = 0;

		virtual void translateY(const T value) = 0;

		virtual void rotate(const T angle) = 0;

		virtual void setRotation(const T angle) = 0;
	};

	template<typename T> class Shapes2D : public ITransform<T>
	{
	protected:
		Point2D<T> origin;
		T angle;

	public:
		Shapes2D() 
		{
			this->origin = point2D<T>(0, 0);
			this->angle = 0;
		};

		Point2D<T>& getOrigin()
		{
			return this->origin;
		}

		const Point2D<T> getOrigin() const
		{
			return this->origin;
		}

		T& getAngle()
		{
			return this->angle;
		}

		const T getAngle() const
		{
			return this->angle;
		}

		void setRotation(const T angle) override
		{
			this->rotate(-this->angle);
			this->rotate(angle);
		}
	};

	template <typename T> class Line2D : public Shapes2D<T>
	{
	private:
		Point2D<T> start;
		Point2D<T> end;

	public:
		Line2D(const Point2D<T> start, const Point2D<T> end)
		{
			this->start = start;
			this->end = end;
			this->origin = Point2D<T>(abs((start.X() + end.X()) / 2), abs((start.Y() + end.Y()) / 2));
			this->angle = 0;
		}

		Line2D(const T x1, const T y1, const T x2, const T y2)
		{
			this->start = Point2D<T>(x1, y1);
			this->end = Point2D<T>(x2, y2);
			this->origin = Point2D<T>(abs((x1 + x2) / 2), abs((y1 + y2) / 2));
			this->angle = 0;
		}

		Line2D(const Line2D<T>& line)
		{
			this->start = line.start;
			this->end = line.end;
			this->origin = line.origin;
			this->angle = line.angle;
		}

		Line2D(Line2D<T>&& line) noexcept
		{
			this->start = std::move(line.start);
			this->end = std::move(line.end);
			this->origin = std::move(line.origin);
			this->angle = std::move(line.angle);
		}

		Line2D<T>& operator=(const Line2D<T>& line)
		{
			if (this == &line)
				return *this;

			this->start = line.start;
			this->end = line.end;
			this->origin = line.origin;
			this->angle = line.angle;

			return *this;
		}

		Line2D<T>& operator=(Line2D<T>&& line) noexcept
		{
			if (this == &line)
				return *this;

			this->start = std::move(line.start);
			this->end = std::move(line.end);
			this->origin = std::move(line.origin);
			this->angle = std::move(line.angle);

			return *this;
		}

		Point2D<T>& getStart()
		{
			return this->start;
		}

		const Point2D<T> getStart() const
		{
			return this->start;
		}

		Point2D<T>& getEnd()
		{
			return this->end;
		}

		const Point2D<T> getEnd() const
		{
			return this->end;
		}

		const T length() const
		{
			return sqrt(((this->start.X() - this->end.X()) * (this->start.X() - this->end.X()))
				+ ((this->start.Y() - this->end.Y()) * (this->start.Y() - this->end.Y())));
		}

		const T length2() const
		{
			return ((this->start.X() - this->end.X()) * (this->start.X() - this->end.X()))
				+ ((this->start.Y() - this->end.Y()) * (this->start.Y() - this->end.Y()));
		}

		void translateX(const T value) override
		{
			this->start.X() += value;
			this->end.X() += value;
			this->origin.X() += value;
		}

		void translateY(const T value) override
		{
			this->start.Y() += value;
			this->end.Y() += value;
			this->origin.Y() += value;
		}

		void rotate(const T angle) override
		{
			T rad = -angle * (M_PI / HALF_CIRCLE);

			// Start point
			T tmpXStart = this->start.X() - this->origin.X();
			T tmpYStart = this->start.Y() - this->origin.Y();

			T rotatedXStart = (tmpXStart * cos(rad) - tmpYStart * sin(rad));
			T rotatedYStart = (tmpXStart * sin(rad) + tmpYStart * cos(rad));

			this->start.X() = rotatedXStart + this->origin.X();
			this->start.Y() = rotatedYStart + this->origin.Y();

			// End point
			T tmpXEnd = this->end.X() - this->origin.X();
			T tmpYEnd = this->end.Y() - this->origin.Y();

			T rotatedXEnd = (tmpXEnd * cos(rad) - tmpYEnd * sin(rad));
			T rotatedYEnd = (tmpXEnd * sin(rad) + tmpYEnd * cos(rad));

			this->end.X() = rotatedXEnd + this->origin.X();
			this->end.Y() = rotatedYEnd + this->origin.Y();

			this->angle += angle;
		}

		const bool collide(const Point2D<T>& point) const
		{
			T dist = (Line2D<T>(this->start, point)).length() + (Line2D<T>(this->end, point)).length();
			T lineDist = this->length();

			return std::abs(dist - lineDist) <= std::numeric_limits<T>::epsilon() ||
				std::abs(dist - lineDist) <= std::numeric_limits<T>::epsilon() * std::max(dist, lineDist);
		}
	};
	typedef Line2D<float> line2D;

	template <typename T> class Circle : public Shapes2D<T>
	{
	private:
		Point2D<T> center;
		T radius;
	public:
		Circle(const Point2D<T> center, const T radius)
		{
			this->center = center;
			this->radius = radius;
			this->origin = center;
			this->angle = 0;
		}

		Circle(const T x, const T y, const T radius)
		{
			this->center = Point2D<T>(x, y);
			this->radius = radius;
			this->origin = this->center;
			this->angle = 0;
		}

		Circle(const Circle<T>& circle)
		{
			this->center = circle.center;
			this->radius = circle.radius;
			this->origin = circle.center;
			this->angle = circle.angle;
		}

		Circle(Circle<T>&& circle) noexcept
		{
			this->center = std::move(circle.center);
			this->radius = std::move(circle.radius);
			this->origin = std::move(circle.center);
			this->angle = std::move(circle.angle);
		}

		Circle<T>& operator=(const Circle<T>& circle)
		{
			if (this == &circle)
				return *this;

			this->center = circle.center;
			this->radius = circle.radius;
			this->origin = circle.center;
			this->angle = circle.angle;

			return *this;
		}

		Circle<T>& operator=(Circle<T>&& circle) noexcept
		{
			if (this == &circle)
				return *this;

			this->center = std::move(circle.center);
			this->radius = std::move(circle.radius);
			this->origin = std::move(circle.center);
			this->angle = std::move(circle.angle);

			return *this;
		}

		Point2D<T>& getCenter()
		{
			return this->center;
		}

		const Point2D<T> getCenter() const
		{
			return this->center;
		}

		T& getRadius()
		{
			return this->radius;
		}

		const T getRadius() const
		{
			return this->radius;
		}

		void translateX(const T value) override
		{
			this->center.X() += value;
			this->origin.X() += value;
		}

		void translateY(const T value) override
		{
			this->center.Y() += value;
			this->origin.Y() += value;
		}

		void rotate(const T angle) override
		{
			T rad = -angle * (M_PI / HALF_CIRCLE);

			T tmpX = this->center.X() - this->origin.X();
			T tmpY = this->center.Y() - this->origin.Y();

			T rotatedX = (tmpX * cos(rad) - tmpY * sin(rad));
			T rotatedY = (tmpX * sin(rad) + tmpY * cos(rad));

			this->center.X() = rotatedX + this->origin.X();
			this->center.Y() = rotatedY + this->origin.Y();

			this->angle += angle;
		}

		const bool collide(const Circle<T>& c2) const
		{
			T dist = Line2D<T>(this->center, c2.center).length();
			return dist <= this->radius + c2.radius;
		}

		const bool collide(const Point2D<T>& point) const
		{
			T dist = (Line2D<T>(this->center, point)).length();
			return dist <= this->radius;
		}

		const bool collide(const Line2D<T>& line) const
		{
			if (this->collide(line.getStart()) || this->collide(line.getEnd()))
				return true;

			T lenth = line.length2();

			T dot = ((this->center.X() - line.getStart().X()) * (line.getEnd().X() - line.getStart().X())
				+ (this->center.Y() - line.getStart().Y()) * (line.getEnd().Y() - line.getStart().Y())) / lenth;

			Point2D<T> closestPoint = Point2D<T>(line.getStart().X() + (dot * (line.getEnd().X() - line.getStart().X())),
				line.getStart().Y() + (dot * (line.getEnd().Y() - line.getStart().Y())));

			if (!line.collide(closestPoint))
				return false;

			T dist = (Line2D<T>(closestPoint, this->center)).length();
			return dist <= this->radius;
		}
	};
	typedef Circle<float> circle;

	template <typename T> class Rectangle : public Shapes2D<T>
	{
	protected:
		Point2D<T> points[4];

	private:
		const Point2D<T> project_points(const Point2D<T>* points, const Point2D<T>& axis, const unsigned int pointCount = 4) const
		{
			Point2D<T> min_max = Point2D<T>(INT_MAX, INT_MIN);

			for (unsigned int i = 0; i < pointCount; i++)
			{
				T projection = (points[i].X() * axis.X()) + (points[i].Y() * axis.Y());

				if (projection < min_max.X())
					min_max.X() = projection;

				if (projection > min_max.Y())
					min_max.Y() = projection;
			}

			return min_max;
		}

	public:
		Rectangle(const Point2D<T> start, const T length, const T height)
		{
			this->points[0] = start;
			this->points[1] = Point2D<T>(start.X() + length, start.Y());
			this->points[2] = Point2D<T>(start.X() + length, start.Y() + height);
			this->points[3] = Point2D<T>(start.X(), start.Y() + height);

			this->origin = this->center();
			this->angle = 0;
		}

		Rectangle(const T x, const T y, const T length, const T height)
		{
			this->points[0] = Point2D<T>(x, y);
			this->points[1] = Point2D<T>(x + length, y);
			this->points[2] = Point2D<T>(x + length, y + height);
			this->points[3] = Point2D<T>(x, y + height);

			this->origin = this->center();
			this->angle = 0;
		}

		Rectangle(const Rectangle<T>& rect)
		{
			for (unsigned int i = 0; i < 4; i++)
				this->points[i] = rect.points[i];

			this->origin = rect.origin;
			this->angle = rect.angle;
		}

		Rectangle(Rectangle<T>&& rect) noexcept
		{
			for (unsigned int i = 0; i < 4; i++)
				this->points[i] = std::move(rect.points[i]);

			this->origin = std::move(rect.origin);
			this->angle = std::move(rect.angle);
		}

		Rectangle<T>& operator=(const Rectangle<T>& rect)
		{
			if (this == &rect)
				return *this;

			for (unsigned int i = 0; i < 4; i++)
				this->points[i] = rect.points[i];

			this->origin = rect.origin;
			this->angle = rect.angle;

			return *this;
		}

		Rectangle<T>& operator=(Rectangle<T>&& rect) noexcept
		{
			if (this == &rect)
				return *this;

			for (unsigned int i = 0; i < 4; i++)
				this->points[i] = std::move(rect.points[i]);

			this->origin = std::move(rect.origin);
			this->angle = std::move(rect.angle);

			return *this;
		}

		const Point2D<T>* getPoints() const
		{
			return &this->points[0];
		}

		const T extendX() const
		{
			return abs(this->points[0].X() - this->points[2].X());
		}

		const T extendY() const
		{
			return abs(this->points[0].Y() - this->points[2].Y());
		}

		const Point2D<T> center() const
		{
			return Point2D<T>((this->points[0].X() + this->points[1].X() + this->points[2].X() + this->points[3].X()) / 4,
				(this->points[0].Y() + this->points[1].Y() + this->points[2].Y() + this->points[3].Y()) / 4);
		}

		void translateX(const T value) override
		{
			for (int i = 0; i < 4; i++)
				this->points[i].X() += value;

			this->origin.X() += value;
		}

		void translateY(const T value) override
		{
			for (int i = 0; i < 4; i++)
				this->points[i].Y() += value;

			this->origin.Y() += value;
		}

		void rotate(const T angle) override
		{
			T rad = -angle * (M_PI / HALF_CIRCLE);

			for (unsigned int i = 0; i < 4; i++)
			{
				T tmpX = this->points[i].X() - this->origin.X();
				T tmpY = this->points[i].Y() - this->origin.Y();

				T rotatedX = (tmpX * cos(rad) - tmpY * sin(rad));
				T rotatedY = (tmpX * sin(rad) + tmpY * cos(rad));

				this->points[i].X() = rotatedX + this->origin.X();
				this->points[i].Y() = rotatedY + this->origin.Y();
			}

			this->angle += angle;
		}

		const bool collide(const Circle<T>& circle)
		{
			T rad = this->angle * (M_PI / HALF_CIRCLE);

			T tmpX = circle.getCenter().X() - this->origin.X();
			T tmpY = circle.getCenter().Y() - this->origin.Y();

			T rotatedX = tmpX * cos(rad) - tmpY * sin(rad);
			T rotatedY = tmpX * sin(rad) + tmpY * cos(rad);

			Point2D<T> rotatedCircle = Point2D<T>(rotatedX + this->origin.X(), rotatedY + this->origin.Y());

			T angle = this->angle;
			this->rotate(-angle);

			T testX, testY;

			if (rotatedCircle.X() < this->points[0].X())
				testX = this->points[0].X();
			else if (rotatedCircle.X() > this->points[1].X())
				testX = this->points[1].X();
			else
				testX = rotatedCircle.X();

			if (rotatedCircle.Y() < this->points[0].Y())
				testY = this->points[0].Y();
			else if (rotatedCircle.Y() > this->points[2].Y())
				testY = this->points[2].Y();
			else
				testY = rotatedCircle.Y();

			T dist = Line2D<T>(Point2D<T>(testX, testY), rotatedCircle).length();

			bool collision = dist <= circle.getRadius();

			this->rotate(angle);

			return collision;
		}

		const bool collide(const Rectangle<T>& r2) const
		{
			for (unsigned int i = 0; i < 2; i++)
			{
				Point2D<T> pointA = this->points[i];
				Point2D<T> pointB = this->points[(i + 1) % 4];

				Point2D<T> edge = Point2D<T>(pointB.X() - pointA.X(), pointB.Y() - pointA.Y());
				Point2D<T> axis = Point2D<T>(edge.Y(), -edge.X());

				Point2D<T> min_max_a = project_points(this->points, axis);
				Point2D<T> min_max_b = project_points(r2.getPoints(), axis);

				if (min_max_a.X() >= min_max_b.Y() || min_max_b.X() >= min_max_a.Y())
					return false;
			}

			for (unsigned int i = 0; i < 2; i++)
			{
				Point2D<T> pointA = r2.getPoints()[i];
				Point2D<T> pointB = r2.getPoints()[(i + 1) % 4];

				Point2D<T> edge = Point2D<T>(pointB.X() - pointA.X(), pointB.Y() - pointA.Y());
				Point2D<T> axis = Point2D<T>(edge.Y(), -edge.X());

				Point2D<T> min_max_a = project_points(this->points, axis);
				Point2D<T> min_max_b = project_points(r2.getPoints(), axis);

				if (min_max_a.X() >= min_max_b.Y() || min_max_b.X() >= min_max_a.Y())
					return false;
			}

			return true;
		}

		const bool collide(const Line2D<T>& line) const
		{
			Point2D<T> points[2] = { line.getStart(), line.getEnd() };
			for (unsigned int i = 0; i < 2; i++)
			{
				Point2D<T> pointA = this->points[i];
				Point2D<T> pointB = this->points[(i + 1) % 4];

				Point2D<T> edge = Point2D<T>(pointB.X() - pointA.X(), pointB.Y() - pointA.Y());
				Point2D<T> axis = Point2D<T>(edge.Y(), -edge.X());

				Point2D<T> min_max_a = project_points(this->points, axis);
				Point2D<T> min_max_b = project_points(&points[0], axis, 2);

				if (min_max_a.X() >= min_max_b.Y() || min_max_b.X() >= min_max_a.Y())
					return false;
			}

			for (unsigned int i = 0; i < 2; i++)
			{
				Point2D<T> pointA = points[i];
				Point2D<T> pointB = points[(i + 1) % 2];

				Point2D<T> edge = Point2D<T>(pointB.X() - pointA.X(), pointB.Y() - pointA.Y());
				Point2D<T> axis = Point2D<T>(edge.Y(), -edge.X());

				Point2D<T> min_max_a = project_points(&points[0], axis, 2);
				Point2D<T> min_max_b = project_points(this->points, axis);

				if (min_max_a.X() >= min_max_b.Y() || min_max_b.X() >= min_max_a.Y())
					return false;
			}

			return true;
		}

		const bool collide(Point2D<T>& point)
		{
			T rad = this->angle * (M_PI / HALF_CIRCLE);

			T tmpX = point.X() - this->origin.X();
			T tmpY = point.Y() - this->origin.Y();

			T rotatedX = tmpX * cos(rad) - tmpY * sin(rad);
			T rotatedY = tmpX * sin(rad) + tmpY * cos(rad);

			Point2D<T> rotatedPoint = Point2D<T>(rotatedX + this->origin.X(), rotatedY + this->origin.Y());

			T angle = this->angle;
			this->rotate(-angle);

			bool collision = rotatedPoint.X() >= this->points[0].X() && rotatedPoint.X() <= this->points[1].X()
				&& rotatedPoint.Y() >= this->points[0].Y() && rotatedPoint.Y() <= this->points[2].Y();

			this->rotate(angle);

			return collision;
		}
	};
	typedef Rectangle<float> rectangle;
}