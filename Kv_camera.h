#pragma once
#include "stdafx.h"

namespace ku
{
	class Camera
	{
		QMatrix4x4 model_;
		QMatrix4x4 perspective_;
		QMatrix4x4 view_;

	public:
		float vertical_angle_ = 60.f;

		QVector3D pos_ = { 1, 1.5, 3 };
		QVector3D target_ = { 0,0,0 };
		QVector3D up_ = { 0,1,0 };

		float dolly_speed_ = 1.0f;
		float pan_speed_ = 1.0f;

		QSize aspect_ratio_;

		QVector3D forward(bool normalize = true) const;

		QVector3D back(bool normalize = true)  const;

		QVector3D left(bool normalize = true) const;

		QVector3D right(bool normalize = true) const;

		void translate(const QVector3D& angle);

		void rotate(const float& angle);

		void rotate_around_target(const float& angle);

		void update();

		QMatrix4x4 mvp();
		const QMatrix4x4& model() const noexcept;
		const QMatrix4x4& view() const noexcept;

	};
}