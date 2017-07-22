#include "stdafx.h"
#include "Kv_camera.h"

void ku::Camera::recalculate_up()
{
	auto looking = (pos_ - target_);
	looking.normalize();
	auto r = QVector3D::crossProduct(looking, QVector3D(0, 1, 0));
	up_ = QVector3D::crossProduct(r, looking);
	up_.normalize();
}

QVector3D ku::Camera::forward() const
{
	auto front = target_ - pos_;
	front.normalize();
	return front;
}

QVector3D ku::Camera::back() const
{
	return -1 * forward();
}

QVector3D ku::Camera::left() const
{
	return  QVector3D::crossProduct(this->up_, this->forward());
}

QVector3D ku::Camera::right() const
{
	return -1 * this->left();
}

const QVector3D& ku::Camera::up() const
{
	return this->up_;
}

QVector3D ku::Camera::down() const
{
	return - this->up();
}

void ku::Camera::translate(const QVector3D & angle)
{
	//qInfo() << dir;
	this->pos_ += angle * pan_speed_;
	this->target_ += angle * pan_speed_;
}

void ku::Camera::rotate_around_target(const float & angle)
{
	const QVector3D& old_dir = this->target_ - this->pos_;
	const QVector3D& new_dir = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), angle).rotatedVector(old_dir);
	this->pos_ = pos_ + (old_dir - new_dir);
	this->recalculate_up();
}

void ku::Camera::focus_origin()
{
	this->target_ = QVector3D(0, 0, 0);
	recalculate_up();
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