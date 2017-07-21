#include "stdafx.h"
#include "Kv_camera.h"

QVector3D ku::Camera::forward(bool normalize) const
{
	QVector3D dir = target_ - pos_;;
	if (normalize) {
		dir.normalize();
	}
	return dir;
}

QVector3D ku::Camera::back(bool normalize) const
{
	return -1 * forward(normalize);
}

QVector3D ku::Camera::left(bool normalize) const
{
	QVector3D left = QVector3D::crossProduct(this->up_, this->forward());
	if (normalize) {
		left.normalize();
	}
	return left;
}

QVector3D ku::Camera::right(bool normalize) const
{
	return -1 * this->left(normalize);
}

void ku::Camera::translate(const QVector3D & angle)
{
	//qInfo() << dir;
	this->pos_ += angle * pan_speed_;
	this->target_ += angle * pan_speed_;
}

void ku::Camera::rotate(const float & angle)
{
	const QVector3D& old_dir = this->pos_ - this->target_;
	const QVector3D& new_dir = QQuaternion::fromAxisAndAngle(this->up_, angle).rotatedVector(old_dir);
	this->target_ = target_ + (old_dir - new_dir);
}

void ku::Camera::rotate_around_target(const float & angle)
{
	const QVector3D& old_dir = this->target_ - this->pos_;
	const QVector3D& new_dir = QQuaternion::fromAxisAndAngle(this->up_, angle).rotatedVector(old_dir);
	this->pos_ = pos_ + (old_dir - new_dir);
}

void ku::Camera::update()
{
	perspective_.setToIdentity();
	view_.setToIdentity();
	model_.setToIdentity();

	perspective_.perspective(this->vertical_angle_, aspect_ratio_.width() / aspect_ratio_.height(), 0.001f, 120.0f);

	this->view_.lookAt(this->pos_, target_, up_);
}

QMatrix4x4 ku::Camera::mvp()
{
	return perspective_ * view_ * model_;
}

const QMatrix4x4& ku::Camera::model() const noexcept
{
	return model_;
}

const QMatrix4x4& ku::Camera::view() const noexcept
{
	return view_;
}