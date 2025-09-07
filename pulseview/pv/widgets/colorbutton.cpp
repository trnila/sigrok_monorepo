/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2013 Joel Holdsworth <joel@airwebreathe.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "colorbutton.hpp"

#include <cassert>

#include <QApplication>
#include <QColorDialog>
#include <QPainter>

namespace pv {
namespace widgets {

const int ColorButton::SwatchMargin = 7;

ColorButton::ColorButton(QWidget *parent) :
	QPushButton("", parent),
	popup_(nullptr)
{
	connect(this, SIGNAL(clicked(bool)), this, SLOT(on_clicked(bool)));
}

ColorButton::ColorButton(int rows, int cols, QWidget *parent) :
	QPushButton("", parent),
	popup_(new ColorPopup(rows, cols, this))
{
	connect(this, SIGNAL(clicked(bool)), this, SLOT(on_clicked(bool)));
	connect(popup_, SIGNAL(selected(int, int)),
		this, SLOT(on_selected(int, int)));
}

ColorPopup* ColorButton::popup()
{
	return popup_;
}

const QColor& ColorButton::color() const
{
	return cur_color_;
}

void ColorButton::set_color(QColor color)
{
	cur_color_ = color;

	if (popup_) {
		const unsigned int rows = popup_->well_array().numRows();
		const unsigned int cols = popup_->well_array().numCols();

		for (unsigned int r = 0; r < rows; r++)
			for (unsigned int c = 0; c < cols; c++)
				if (popup_->well_array().cellBrush(r, c).color() == color) {
					popup_->well_array().setSelected(r, c);
					popup_->well_array().setCurrent(r, c);
					return;
				}
	}
}

void ColorButton::set_palette(const QColor *const palette)
{
	assert(palette);
	assert(popup_);

	const unsigned int rows = popup_->well_array().numRows();
	const unsigned int cols = popup_->well_array().numCols();

	for (unsigned int r = 0; r < rows; r++)
		for (unsigned int c = 0; c < cols; c++)
			popup_->well_array().setCellBrush(r, c, QBrush(palette[r * cols + c]));
}

void ColorButton::on_clicked(bool)
{
	if (popup_) {
		popup_->set_position(mapToGlobal(rect().center()), Popup::Bottom);
		popup_->show();
	} else {
		QColorDialog dlg(this);
		dlg.setOption(QColorDialog::ShowAlphaChannel);
		dlg.setOption(QColorDialog::DontUseNativeDialog);
		connect(&dlg, SIGNAL(colorSelected(const QColor)),
			this, SLOT(on_color_selected(const QColor)));
		dlg.setCurrentColor(cur_color_);
		dlg.exec();
	}
}

void ColorButton::on_selected(int row, int col)
{
	assert(popup_);

	cur_color_ = popup_->well_array().cellBrush(row, col).color();
	selected(cur_color_);
}

void ColorButton::on_color_selected(const QColor& color)
{
	cur_color_ = color;
	selected(cur_color_);
}

void ColorButton::paintEvent(QPaintEvent *event)
{
	QPushButton::paintEvent(event);

	QPainter p(this);

	const QRect r = rect().adjusted(SwatchMargin, SwatchMargin,
		-SwatchMargin, -SwatchMargin);
	p.setPen(QApplication::palette().color(QPalette::Dark));
	p.setBrush(QBrush(cur_color_));
	p.drawRect(r);
}

}  // namespace widgets
}  // namespace pv
