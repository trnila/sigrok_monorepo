/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
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

#include <extdef.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QString>

#include "analogsignal.hpp"
#include "logicsignal.hpp"
#include "view.hpp"

#include "pv/util.hpp"
#include "pv/data/analog.hpp"
#include "pv/data/analogsegment.hpp"
#include "pv/data/logic.hpp"
#include "pv/data/logicsegment.hpp"
#include "pv/data/signalbase.hpp"
#include "pv/globalsettings.hpp"

#include <libsigrokcxx/libsigrokcxx.hpp>

using std::abs;
using std::deque;
using std::div;
using std::div_t;
// Note that "using std::isnan;" is _not_ put here since that would break
// compilation on some platforms. Use "std::isnan()" instead in checks below.
using std::fabs;
using std::max;
using std::make_pair;
using std::min;
using std::numeric_limits;
using std::out_of_range;
using std::pair;
using std::shared_ptr;
using std::vector;

using pv::data::LogicSegment;
using pv::data::SignalBase;
using pv::util::SIPrefix;
using pv::util::determine_value_prefix;

namespace pv {
namespace views {
namespace trace {

const QPen AnalogSignal::AxisPen(QColor(0, 0, 0, 30 * 256 / 100), 2);
const QColor AnalogSignal::GridMajorColor = QColor(0, 0, 0, 40 * 256 / 100);
const QColor AnalogSignal::GridMinorColor = QColor(0, 0, 0, 20 * 256 / 100);

const QColor AnalogSignal::SamplingPointColorLo = QColor(200, 0, 0, 80 * 256 / 100);
const QColor AnalogSignal::SamplingPointColorNe = QColor(0,   0, 0, 80 * 256 / 100);
const QColor AnalogSignal::SamplingPointColorHi = QColor(0, 200, 0, 80 * 256 / 100);

const QColor AnalogSignal::ThresholdColor = QColor(0, 0, 0, 30 * 256 / 100);
const QColor AnalogSignal::ThresholdColorLo = QColor(255, 0, 0, 8 * 256 / 100);
const QColor AnalogSignal::ThresholdColorNe = QColor(0,   0, 0, 10 * 256 / 100);
const QColor AnalogSignal::ThresholdColorHi = QColor(0, 255, 0, 8 * 256 / 100);

const int64_t AnalogSignal::TracePaintBlockSize = 1024 * 1024;  // 4 MiB (due to float)
const float AnalogSignal::EnvelopeThreshold = 64.0f;

const int AnalogSignal::MaximumVDivs = 10;
const int AnalogSignal::MinScaleIndex = -6;  // 0.01 units/div
const int AnalogSignal::MaxScaleIndex = 10;  // 1000 units/div

const int AnalogSignal::InfoTextMarginRight = 20;
const int AnalogSignal::InfoTextMarginBottom = 5;

AnalogSignal::AnalogSignal(pv::Session &session, shared_ptr<data::SignalBase> base) :
	LogicSignal(session, base),
	value_at_hover_pos_(std::numeric_limits<float>::quiet_NaN()),
	scale_index_(4), // 20 per div
	pos_vdivs_(1),
	neg_vdivs_(1),
	resolution_(0),
	display_type_(DisplayAnalog),
	autoranging_(true)
{
	axis_pen_ = AxisPen;

	pv::data::Analog* analog_data =
		dynamic_cast<pv::data::Analog*>(base_->analog_data().get());

	connect(analog_data, SIGNAL(min_max_changed(float, float)),
		this, SLOT(on_min_max_changed(float, float)));

	GlobalSettings settings;
	show_sampling_points_ =
		settings.value(GlobalSettings::Key_View_ShowSamplingPoints).toBool();
	fill_high_areas_ =
		settings.value(GlobalSettings::Key_View_FillSignalHighAreas).toBool();
	high_fill_color_ = QColor::fromRgba(settings.value(
		GlobalSettings::Key_View_FillSignalHighAreaColor).value<uint32_t>());
	show_analog_minor_grid_ =
		settings.value(GlobalSettings::Key_View_ShowAnalogMinorGrid).toBool();
	conversion_threshold_disp_mode_ =
		settings.value(GlobalSettings::Key_View_ConversionThresholdDispMode).toInt();
	div_height_ = settings.value(GlobalSettings::Key_View_DefaultDivHeight).toInt();

	update_logic_level_offsets();
	update_scale();
}

std::map<QString, QVariant> AnalogSignal::save_settings() const
{
	LogicSignal::save_settings();

	std::map<QString, QVariant> result;

	result["pos_vdivs"] = pos_vdivs_;
	result["neg_vdivs"] = neg_vdivs_;
	result["scale_index"] = scale_index_;
	result["display_type"] = display_type_;
	result["autoranging"] = autoranging_;
	result["div_height"] = div_height_;

	return result;
}

void AnalogSignal::restore_settings(std::map<QString, QVariant> settings)
{
	LogicSignal::restore_settings(settings);

	auto entry = settings.find("pos_vdivs");
	if (entry != settings.end())
		pos_vdivs_ = settings["pos_vdivs"].toInt();

	entry = settings.find("neg_vdivs");
	if (entry != settings.end())
		neg_vdivs_ = settings["neg_vdivs"].toInt();

	entry = settings.find("scale_index");
	if (entry != settings.end()) {
		scale_index_ = settings["scale_index"].toInt();
		update_scale();
	}

	entry = settings.find("display_type");
	if (entry != settings.end())
		display_type_ = (DisplayType)(settings["display_type"].toInt());

	entry = settings.find("autoranging");
	if (entry != settings.end())
		autoranging_ = settings["autoranging"].toBool();

	entry = settings.find("div_height");
	if (entry != settings.end()) {
		const int old_height = div_height_;
		div_height_ = settings["div_height"].toInt();

		update_logic_level_offsets();
		update_scale();

		if ((div_height_ != old_height) && owner_) {
			// Call order is important, otherwise the lazy event handler won't work
			owner_->extents_changed(false, true);
			owner_->row_item_appearance_changed(false, true);
		}
	}
}

pair<int, int> AnalogSignal::v_extents() const
{
	const int ph = pos_vdivs_ * div_height_;
	const int nh = neg_vdivs_ * div_height_;

	return make_pair(-ph, nh);
}

void AnalogSignal::paint_back(QPainter &p, ViewItemPaintParams &pp)
{
	if (!base_->enabled())
		return;

	bool paint_thr_bg =
		conversion_threshold_disp_mode_ == GlobalSettings::ConvThrDispMode_Background;

	const vector<double> thresholds = base_->get_conversion_thresholds();

	// Only display thresholds if we have some and we show analog samples
	if ((thresholds.size() > 0) && paint_thr_bg &&
		((display_type_ == DisplayAnalog) || (display_type_ == DisplayBoth))) {

		const int visual_y = get_visual_y();
		const pair<int, int> extents = v_extents();
		const int top = visual_y + extents.first;
		const int btm = visual_y + extents.second;

		// Draw high/neutral/low areas
		if (thresholds.size() == 2) {
			int thr_lo = visual_y - thresholds[0] * scale_;
			int thr_hi = visual_y - thresholds[1] * scale_;
			thr_lo = min(max(thr_lo, top), btm);
			thr_hi = min(max(thr_hi, top), btm);

			p.fillRect(QRectF(pp.left(), top, pp.width(), thr_hi - top),
				QBrush(ThresholdColorHi));
			p.fillRect(QRectF(pp.left(), thr_hi, pp.width(), thr_lo - thr_hi),
				QBrush(ThresholdColorNe));
			p.fillRect(QRectF(pp.left(), thr_lo, pp.width(), btm - thr_lo),
				QBrush(ThresholdColorLo));
		} else {
			int thr = visual_y - thresholds[0] * scale_;
			thr = min(max(thr, top), btm);

			p.fillRect(QRectF(pp.left(), top, pp.width(), thr - top),
				QBrush(ThresholdColorHi));
			p.fillRect(QRectF(pp.left(), thr, pp.width(), btm - thr),
				QBrush(ThresholdColorLo));
		}

		paint_axis(p, pp, get_visual_y());
	} else {
		Signal::paint_back(p, pp);
		paint_axis(p, pp, get_visual_y());
	}
}

void AnalogSignal::paint_mid(QPainter &p, ViewItemPaintParams &pp)
{
	assert(base_->analog_data());
	assert(owner_);

	const int y = get_visual_y();

	if (!base_->enabled())
		return;

	if ((display_type_ == DisplayAnalog) || (display_type_ == DisplayBoth)) {
		paint_grid(p, y, pp.left(), pp.right());

		shared_ptr<pv::data::AnalogSegment> segment = get_analog_segment_to_paint();

		if (segment && (segment->get_sample_count() > 0)) {
			const double pixels_offset = pp.pixels_offset();
			const double samplerate = max(1.0, segment->samplerate());
			const pv::util::Timestamp& start_time = segment->start_time();
			const int64_t last_sample = (int64_t)segment->get_sample_count() - 1;
			const double samples_per_pixel = samplerate * pp.scale();
			const pv::util::Timestamp start = samplerate * (pp.offset() - start_time);
			const pv::util::Timestamp end = start + samples_per_pixel * pp.width();

			const int64_t start_sample = min(max(floor(start).convert_to<int64_t>(),
				(int64_t)0), last_sample);
			const int64_t end_sample = min(max((ceil(end) + 1).convert_to<int64_t>(),
				(int64_t)0), last_sample);

			if (samples_per_pixel < EnvelopeThreshold)
				paint_trace(p, segment, y, pp.left(), start_sample, end_sample,
					pixels_offset, samples_per_pixel);
			else
				paint_envelope(p, segment, y, pp.left(), start_sample, end_sample,
					pixels_offset, samples_per_pixel);
		}
	}

	if ((display_type_ == DisplayConverted) || (display_type_ == DisplayBoth))
		if (base_->logic_data())
			LogicSignal::paint_mid(p, pp);

	const QString err = base_->get_error_message();
	if (!err.isEmpty())
		paint_error(p, pp);
}

void AnalogSignal::paint_fore(QPainter &p, ViewItemPaintParams &pp)
{
	if (!enabled())
		return;

	if ((display_type_ == DisplayAnalog) || (display_type_ == DisplayBoth)) {
		const int y = get_visual_y();

		QString infotext;

		SIPrefix prefix;
		if (fabs(signal_max_) > fabs(signal_min_))
			prefix = determine_value_prefix(fabs(signal_max_));
		else
			prefix = determine_value_prefix(fabs(signal_min_));

		// Show the info section on the right side of the trace, including
		// the value at the hover point when the hover marker is enabled
		// and we have corresponding data available
		if (show_hover_marker_ && !std::isnan(value_at_hover_pos_)) {
			infotext = QString("[%1] %2 V/div")
				.arg(format_value_si(value_at_hover_pos_, prefix, 3, "V", false))
				.arg(resolution_);
		} else
			infotext = QString("%1 V/div").arg(resolution_);

		p.setPen(base_->color());
		p.setFont(QApplication::font());

		const QRectF bounding_rect = QRectF(pp.left(),
				y + v_extents().first,
				pp.width() - InfoTextMarginRight,
				v_extents().second - v_extents().first - InfoTextMarginBottom);

		p.drawText(bounding_rect, Qt::AlignRight | Qt::AlignBottom, infotext);

		if (show_hover_marker_)
			paint_hover_marker(p);
	}

	if ((display_type_ == DisplayConverted) || (display_type_ == DisplayBoth))
		LogicSignal::paint_fore(p, pp);
}

void AnalogSignal::paint_grid(QPainter &p, int y, int left, int right)
{
	bool was_antialiased = p.testRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::Antialiasing, false);

	if (pos_vdivs_ > 0) {
		p.setPen(QPen(GridMajorColor, 1, Qt::DashLine));
		for (int i = 1; i <= pos_vdivs_; i++) {
			const float dy = i * div_height_;
			p.drawLine(QLineF(left, y - dy, right, y - dy));
		}
	}

	if ((pos_vdivs_ > 0) && show_analog_minor_grid_) {
		p.setPen(QPen(GridMinorColor, 1, Qt::DashLine));
		for (int i = 0; i < pos_vdivs_; i++) {
			const float dy = i * div_height_;
			const float dy25 = dy + (0.25 * div_height_);
			const float dy50 = dy + (0.50 * div_height_);
			const float dy75 = dy + (0.75 * div_height_);
			p.drawLine(QLineF(left, y - dy25, right, y - dy25));
			p.drawLine(QLineF(left, y - dy50, right, y - dy50));
			p.drawLine(QLineF(left, y - dy75, right, y - dy75));
		}
	}

	if (neg_vdivs_ > 0) {
		p.setPen(QPen(GridMajorColor, 1, Qt::DashLine));
		for (int i = 1; i <= neg_vdivs_; i++) {
			const float dy = i * div_height_;
			p.drawLine(QLineF(left, y + dy, right, y + dy));
		}
	}

	if ((pos_vdivs_ > 0) && show_analog_minor_grid_) {
		p.setPen(QPen(GridMinorColor, 1, Qt::DashLine));
		for (int i = 0; i < neg_vdivs_; i++) {
			const float dy = i * div_height_;
			const float dy25 = dy + (0.25 * div_height_);
			const float dy50 = dy + (0.50 * div_height_);
			const float dy75 = dy + (0.75 * div_height_);
			p.drawLine(QLineF(left, y + dy25, right, y + dy25));
			p.drawLine(QLineF(left, y + dy50, right, y + dy50));
			p.drawLine(QLineF(left, y + dy75, right, y + dy75));
		}
	}

	p.setRenderHint(QPainter::Antialiasing, was_antialiased);
}

void AnalogSignal::paint_trace(QPainter &p,
	const shared_ptr<pv::data::AnalogSegment> &segment,
	int y, int left, const int64_t start, const int64_t end,
	const double pixels_offset, const double samples_per_pixel)
{
	if (end <= start)
		return;

	bool paint_thr_dots =
		(base_->get_conversion_type() != data::SignalBase::NoConversion) &&
		(conversion_threshold_disp_mode_ == GlobalSettings::ConvThrDispMode_Dots);

	vector<double> thresholds;
	if (paint_thr_dots)
		thresholds = base_->get_conversion_thresholds();

	// Calculate and paint the sampling points if enabled and useful
	GlobalSettings settings;
	const bool show_sampling_points =
		(show_sampling_points_ || paint_thr_dots) && (samples_per_pixel < 0.25);

	p.setPen(base_->color());

	const int64_t points_count = end - start + 1;

	QPointF *points = new QPointF[points_count];
	QPointF *point = points;

	vector<QRectF> sampling_points[3];

	int64_t sample_count = min(points_count, TracePaintBlockSize);
	int64_t block_sample = 0;
	float *sample_block = new float[TracePaintBlockSize];
	segment->get_samples(start, start + sample_count, sample_block);

	if (show_hover_marker_)
		reset_pixel_values();

	const int w = 2;
	for (int64_t sample = start; sample <= end; sample++, block_sample++) {

		// Fetch next block of samples if we finished the current one
		if (block_sample == TracePaintBlockSize) {
			block_sample = 0;
			sample_count = min(points_count - sample, TracePaintBlockSize);
			segment->get_samples(sample, sample + sample_count, sample_block);
		}

		const float abs_x = sample / samples_per_pixel - pixels_offset;
		const float x = left + abs_x;

		*point++ = QPointF(x, y - sample_block[block_sample] * scale_);

		// Generate the pixel<->value lookup table for the mouse hover
		if (show_hover_marker_)
			process_next_sample_value(abs_x, sample_block[block_sample]);

		// Create the sampling points if needed
		if (show_sampling_points) {
			int idx = 0;  // Neutral

			if (paint_thr_dots) {
				if (thresholds.size() == 1)
					idx = (sample_block[block_sample] >= thresholds[0]) ? 2 : 1;
				else if (thresholds.size() == 2) {
					if (sample_block[block_sample] > thresholds[1])
						idx = 2;  // High
					else if (sample_block[block_sample] < thresholds[0])
						idx = 1;  // Low
				}
			}

			sampling_points[idx].emplace_back(x - (w / 2), y - sample_block[block_sample] * scale_ - (w / 2), w, w);
		}
	}
	delete[] sample_block;

	// QPainter::drawPolyline() is slow, let's paint the lines ourselves
	for (int64_t i = 1; i < points_count; i++)
		p.drawLine(points[i - 1], points[i]);

	if (show_sampling_points) {
		if (paint_thr_dots) {
			p.setPen(SamplingPointColorNe);
			p.drawRects(sampling_points[0].data(), sampling_points[0].size());
			p.setPen(SamplingPointColorLo);
			p.drawRects(sampling_points[1].data(), sampling_points[1].size());
			p.setPen(SamplingPointColorHi);
			p.drawRects(sampling_points[2].data(), sampling_points[2].size());
		} else {
			p.setPen(SamplingPointColor);
			p.drawRects(sampling_points[0].data(), sampling_points[0].size());
		}
	}

	delete[] points;
}

void AnalogSignal::paint_envelope(QPainter &p,
	const shared_ptr<pv::data::AnalogSegment> &segment,
	int y, int left, const int64_t start, const int64_t end,
	const double pixels_offset, const double samples_per_pixel)
{
	using pv::data::AnalogSegment;

	// Note: Envelope painting currently doesn't generate a pixel<->value lookup table
	if (show_hover_marker_)
		reset_pixel_values();

	AnalogSegment::EnvelopeSection e;
	segment->get_envelope_section(e, start, end, samples_per_pixel);

	if (e.length < 2)
		return;

	p.setPen(QPen(Qt::NoPen));
	p.setBrush(base_->color());

	QRectF *const rects = new QRectF[e.length];
	QRectF *rect = rects;

	for (uint64_t sample = 0; sample < e.length - 1; sample++) {
		const float x = ((e.scale * sample + e.start) /
			samples_per_pixel - pixels_offset) + left;

		const AnalogSegment::EnvelopeSample *const s = e.samples + sample;

		// We overlap this sample with the next so that vertical
		// gaps do not appear during steep rising or falling edges
		const float b = y - max(s->max, (s + 1)->min) * scale_;
		const float t = y - min(s->min, (s + 1)->max) * scale_;

		float h = b - t;
		if (h >= 0.0f && h <= 1.0f)
			h = 1.0f;
		if (h <= 0.0f && h >= -1.0f)
			h = -1.0f;

		*rect++ = QRectF(x, t, 1.0f, h);
	}

	p.drawRects(rects, e.length);

	delete[] rects;
	delete[] e.samples;
}

shared_ptr<pv::data::AnalogSegment> AnalogSignal::get_analog_segment_to_paint() const
{
	shared_ptr<pv::data::AnalogSegment> segment;

	const deque< shared_ptr<pv::data::AnalogSegment> > &segments =
		base_->analog_data()->analog_segments();

	if (!segments.empty()) {
		if (segment_display_mode_ == ShowLastSegmentOnly)
			segment = segments.back();

		if ((segment_display_mode_ == ShowSingleSegmentOnly) ||
			(segment_display_mode_ == ShowLastCompleteSegmentOnly)) {
			try {
				segment = segments.at(current_segment_);
			} catch (out_of_range&) {
				qDebug() << "Current analog segment out of range for signal" << base_->name() << ":" << current_segment_;
			}
		}
	}

	return segment;
}

float AnalogSignal::get_resolution(int scale_index)
{
	const float seq[] = {1.0f, 2.0f, 5.0f};

	const int offset = numeric_limits<int>::max() / (2 * countof(seq));
	const div_t d = div((int)(scale_index + countof(seq) * offset),
		countof(seq));

	return powf(10.0f, d.quot - offset) * seq[d.rem];
}

void AnalogSignal::update_scale()
{
	resolution_ = get_resolution(scale_index_);
	scale_ = div_height_ / resolution_;
}

void AnalogSignal::update_logic_level_offsets()
{
	const int signal_margin = QFontMetrics(QApplication::font()).height() / 2;

	const int ph = min(pos_vdivs_, 1) * div_height_;
	const int nh = min(neg_vdivs_, 1) * div_height_;

	high_level_offset_ = -ph + signal_margin + 0.5f;
	low_level_offset_  =  nh - signal_margin - 0.5f;
}

void AnalogSignal::update_conversion_widgets()
{
	SignalBase::ConversionType conv_type = base_->get_conversion_type();

	// Enable or disable widgets depending on conversion state
	conv_threshold_cb_->setEnabled(conv_type != SignalBase::NoConversion);
	display_type_cb_->setEnabled(conv_type != SignalBase::NoConversion);

	conv_threshold_cb_->clear();

	vector < pair<QString, int> > presets = base_->get_conversion_presets();

	// Prevent the combo box from firing the "edit text changed" signal
	// as that would involuntarily select the first entry
	conv_threshold_cb_->blockSignals(true);

	// Set available options depending on chosen conversion
	for (pair<QString, int>& preset : presets)
		conv_threshold_cb_->addItem(preset.first, preset.second);

	map < QString, QVariant > options = base_->get_conversion_options();

	if (conv_type == SignalBase::A2LConversionByThreshold) {
		const vector<double> thresholds = base_->get_conversion_thresholds(
				SignalBase::A2LConversionByThreshold, true);
		conv_threshold_cb_->addItem(
				QString("%1V").arg(QString::number(thresholds[0], 'f', 1)), -1);
	}

	if (conv_type == SignalBase::A2LConversionBySchmittTrigger) {
		const vector<double> thresholds = base_->get_conversion_thresholds(
				SignalBase::A2LConversionBySchmittTrigger, true);
		conv_threshold_cb_->addItem(QString("%1V/%2V").arg(
				QString::number(thresholds[0], 'f', 1),
				QString::number(thresholds[1], 'f', 1)), -1);
	}

	int preset_id = base_->get_current_conversion_preset();
	conv_threshold_cb_->setCurrentIndex(
			conv_threshold_cb_->findData(preset_id));

	conv_threshold_cb_->blockSignals(false);
}

vector<data::LogicSegment::EdgePair> AnalogSignal::get_nearest_level_changes(uint64_t sample_pos)
{
	assert(base_);
	assert(owner_);

	// Return if there's no logic data or we're showing only the analog trace
	if (!base_->logic_data() || (display_type_ == DisplayAnalog))
		return vector<data::LogicSegment::EdgePair>();

	if (sample_pos == 0)
		return vector<LogicSegment::EdgePair>();

	shared_ptr<LogicSegment> segment = get_logic_segment_to_paint();
	if (!segment || (segment->get_sample_count() == 0))
		return vector<LogicSegment::EdgePair>();

	const View *view = owner_->view();
	assert(view);
	const double samples_per_pixel = base_->get_samplerate() * view->scale();

	vector<LogicSegment::EdgePair> edges;

	segment->get_surrounding_edges(edges, sample_pos,
		samples_per_pixel / LogicSignal::Oversampling, 0);

	if (edges.empty())
		return vector<LogicSegment::EdgePair>();

	return edges;
}

void AnalogSignal::perform_autoranging(bool keep_divs, bool force_update)
{
	const deque< shared_ptr<pv::data::AnalogSegment> > &segments =
		base_->analog_data()->analog_segments();

	if (segments.empty())
		return;

	double min = 0, max = 0;

	for (const shared_ptr<pv::data::AnalogSegment>& segment : segments) {
		pair<double, double> mm = segment->get_min_max();
		min = std::min(min, mm.first);
		max = std::max(max, mm.second);
	}

	if ((min == signal_min_) && (max == signal_max_) && !force_update)
		return;

	signal_min_ = min;
	signal_max_ = max;

	// If we're allowed to alter the div assignment...
	if (!keep_divs) {
		// Use all divs for the positive range if there are no negative values
		if ((min == 0) && (neg_vdivs_ > 0)) {
			pos_vdivs_ += neg_vdivs_;
			neg_vdivs_ = 0;
		}

		// Split up the divs if there are negative values but no negative divs
		if ((min < 0) && (neg_vdivs_ == 0)) {
			neg_vdivs_ = pos_vdivs_ / 2;
			pos_vdivs_ -= neg_vdivs_;
		}
	}

	const bool showing_logic = (display_type_ == DisplayConverted) || (display_type_ == DisplayBoth);

 	// If there is still no positive div when we need it, add one
	// (this can happen when pos_vdivs==neg_vdivs==0)
	if (((max > 0) && (pos_vdivs_ == 0)) || showing_logic) {
		pos_vdivs_ = 1;
		owner_->extents_changed(false, true);
	}

	// If there is still no negative div when we need it, add one
	// (this can happen when pos_vdivs was 0 or 1 when trying to split)
	if (((min < 0) && (neg_vdivs_ == 0)) || showing_logic) {
		neg_vdivs_ = 1;
		owner_->extents_changed(false, true);
	}

	double min_value_per_div;
	if ((pos_vdivs_ > 0) && (neg_vdivs_ >  0))
		min_value_per_div = std::max(max / pos_vdivs_, -min / neg_vdivs_);
	else if (pos_vdivs_ > 0)
		min_value_per_div = max / pos_vdivs_;
	else
		min_value_per_div = -min / neg_vdivs_;

	// Find first scale value that is bigger than the value we need
	for (int i = MinScaleIndex; i < MaxScaleIndex; i++)
		if (get_resolution(i) > min_value_per_div) {
			scale_index_ = i;
			break;
		}

	update_scale();
}

void AnalogSignal::reset_pixel_values()
{
	value_at_pixel_pos_.clear();
	current_pixel_pos_ = -1;
	prev_value_at_pixel_ = std::numeric_limits<float>::quiet_NaN();
}

void AnalogSignal::process_next_sample_value(float x, float value)
{
	// Note: NAN is used to indicate the non-existance of a value at this pixel

	if (std::isnan(prev_value_at_pixel_)) {
		if (x < 0) {
			min_value_at_pixel_ = value;
			max_value_at_pixel_ = value;
			prev_value_at_pixel_ = value;
			current_pixel_pos_ = x;
		} else
			prev_value_at_pixel_ = std::numeric_limits<float>::quiet_NaN();
	}

	const int pixel_pos = (int)(x + 0.5);

	if (pixel_pos > current_pixel_pos_) {
		if (pixel_pos - current_pixel_pos_ == 1) {
			if (std::isnan(prev_value_at_pixel_)) {
				value_at_pixel_pos_.push_back(prev_value_at_pixel_);
			} else {
				// Average the min/max range to create one value for the previous pixel
				const float avg = (min_value_at_pixel_ + max_value_at_pixel_) / 2;
				value_at_pixel_pos_.push_back(avg);
			}
		} else {
			// Interpolate values to create values for the intermediate pixels
			const float start_value = prev_value_at_pixel_;
			const float end_value = value;
			const int steps = abs(pixel_pos - current_pixel_pos_);
			const double gradient = (end_value - start_value) / steps;
			for (int i = 0; i < steps; i++) {
				if (current_pixel_pos_ + i < 0)
					continue;
				value_at_pixel_pos_.push_back(start_value + i * gradient);
			}
		}

		min_value_at_pixel_ = value;
		max_value_at_pixel_ = value;
		prev_value_at_pixel_ = value;
		current_pixel_pos_ = pixel_pos;
	} else {
		// Another sample for the same pixel
		if (value < min_value_at_pixel_)
			min_value_at_pixel_ = value;
		if (value > max_value_at_pixel_)
			max_value_at_pixel_ = value;
	}
}

void AnalogSignal::populate_popup_form(QWidget *parent, QFormLayout *form)
{
	// Add the standard options
	Signal::populate_popup_form(parent, form);

	// Add div-related settings
	pvdiv_sb_ = new QSpinBox(parent);
	pvdiv_sb_->setRange(0, MaximumVDivs);
	pvdiv_sb_->setValue(pos_vdivs_);
	pvdiv_sb_->setEnabled(!autoranging_);
	connect(pvdiv_sb_, SIGNAL(valueChanged(int)),
		this, SLOT(on_pos_vdivs_changed(int)));
	form->addRow(tr("Number of pos vertical divs"), pvdiv_sb_);

	nvdiv_sb_ = new QSpinBox(parent);
	nvdiv_sb_->setRange(0, MaximumVDivs);
	nvdiv_sb_->setValue(neg_vdivs_);
	nvdiv_sb_->setEnabled(!autoranging_);
	connect(nvdiv_sb_, SIGNAL(valueChanged(int)),
		this, SLOT(on_neg_vdivs_changed(int)));
	form->addRow(tr("Number of neg vertical divs"), nvdiv_sb_);

	div_height_sb_ = new QSpinBox(parent);
	div_height_sb_->setRange(20, 1000);
	div_height_sb_->setSingleStep(5);
	div_height_sb_->setSuffix(tr(" pixels"));
	div_height_sb_->setValue(div_height_);
	connect(div_height_sb_, SIGNAL(valueChanged(int)),
		this, SLOT(on_div_height_changed(int)));
	form->addRow(tr("Div height"), div_height_sb_);

	// Add the vertical resolution
	resolution_cb_ = new QComboBox(parent);
	resolution_cb_->setEnabled(!autoranging_);

	for (int i = MinScaleIndex; i < MaxScaleIndex; i++) {
		const QString label = QString("%1").arg(get_resolution(i));
		resolution_cb_->insertItem(0, label, QVariant(i));
	}

	int cur_idx = resolution_cb_->findData(QVariant(scale_index_));
	resolution_cb_->setCurrentIndex(cur_idx);

	connect(resolution_cb_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_resolution_changed(int)));

	QGridLayout *const vdiv_layout = new QGridLayout;
	QLabel *const vdiv_unit = new QLabel(tr("V/div"));
	vdiv_layout->addWidget(resolution_cb_, 0, 0);
	vdiv_layout->addWidget(vdiv_unit, 0, 1);

	form->addRow(tr("Vertical resolution"), vdiv_layout);

	// Add the autoranging checkbox
	QCheckBox* autoranging_cb = new QCheckBox();
	autoranging_cb->setCheckState(autoranging_ ? Qt::Checked : Qt::Unchecked);

	connect(autoranging_cb, SIGNAL(stateChanged(int)),
		this, SLOT(on_autoranging_changed(int)));

	form->addRow(tr("Autoranging"), autoranging_cb);

	// Add the conversion type dropdown
	conversion_cb_ = new QComboBox();

	conversion_cb_->addItem(tr("none"),
		SignalBase::NoConversion);
	conversion_cb_->addItem(tr("to logic via threshold"),
		SignalBase::A2LConversionByThreshold);
	conversion_cb_->addItem(tr("to logic via schmitt-trigger"),
		SignalBase::A2LConversionBySchmittTrigger);

	cur_idx = conversion_cb_->findData(QVariant(base_->get_conversion_type()));
	conversion_cb_->setCurrentIndex(cur_idx);

	form->addRow(tr("Conversion"), conversion_cb_);

	connect(conversion_cb_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_conversion_changed(int)));

    // Add the conversion threshold settings
    conv_threshold_cb_ = new QComboBox();
    conv_threshold_cb_->setEditable(true);

    form->addRow(tr("Conversion threshold(s)"), conv_threshold_cb_);

    connect(conv_threshold_cb_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_conv_threshold_changed(int)));
    connect(conv_threshold_cb_, SIGNAL(editTextChanged(const QString&)),
            this, SLOT(on_conv_threshold_changed()));  // index will be -1

	// Add the display type dropdown
	display_type_cb_ = new QComboBox();

	display_type_cb_->addItem(tr("analog"), DisplayAnalog);
	display_type_cb_->addItem(tr("converted"), DisplayConverted);
	display_type_cb_->addItem(tr("analog+converted"), DisplayBoth);

	cur_idx = display_type_cb_->findData(QVariant(display_type_));
	display_type_cb_->setCurrentIndex(cur_idx);

	form->addRow(tr("Show traces for"), display_type_cb_);

	connect(display_type_cb_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_display_type_changed(int)));

	// Update the conversion widget contents and states
	update_conversion_widgets();
}

void AnalogSignal::hover_point_changed(const QPoint &hp)
{
	Signal::hover_point_changed(hp);

	// Note: Even though the view area begins at 0, we exclude 0 because
	// that's also the value given when the cursor is over the header to the
	// left of the trace paint area
	if (hp.x() <= 0) {
		value_at_hover_pos_ = std::numeric_limits<float>::quiet_NaN();
	} else {
		if ((size_t)hp.x() < value_at_pixel_pos_.size())
			value_at_hover_pos_ = value_at_pixel_pos_.at(hp.x());
		else
			value_at_hover_pos_ = std::numeric_limits<float>::quiet_NaN();
	}
}

void AnalogSignal::on_setting_changed(const QString &key, const QVariant &value)
{
	Signal::on_setting_changed(key, value);

	if (key == GlobalSettings::Key_View_ShowSamplingPoints)
		show_sampling_points_ = value.toBool();

	if (key == GlobalSettings::Key_View_FillSignalHighAreas)
		fill_high_areas_ = value.toBool();

	if (key == GlobalSettings::Key_View_FillSignalHighAreaColor)
		high_fill_color_ = QColor::fromRgba(value.value<uint32_t>());

	if (key == GlobalSettings::Key_View_ShowAnalogMinorGrid)
		show_analog_minor_grid_ = value.toBool();

	if (key == GlobalSettings::Key_View_ConversionThresholdDispMode) {
		conversion_threshold_disp_mode_ = value.toInt();

		if (owner_)
			owner_->row_item_appearance_changed(false, true);
	}
}

void AnalogSignal::on_min_max_changed(float min, float max)
{
	if (autoranging_)
		perform_autoranging(false, false);
	else {
		if (min < signal_min_) signal_min_ = min;
		if (max > signal_max_) signal_max_ = max;
	}
}

void AnalogSignal::on_pos_vdivs_changed(int vdivs)
{
	if (vdivs == pos_vdivs_)
		return;

	pos_vdivs_ = vdivs;

	// There has to be at least one div, positive or negative
	if ((neg_vdivs_ == 0) && (pos_vdivs_ == 0)) {
		pos_vdivs_ = 1;
		if (pvdiv_sb_)
			pvdiv_sb_->setValue(pos_vdivs_);
	}

	if (autoranging_) {
		perform_autoranging(true, true);

		// It could be that a positive or negative div was added, so update
		if (pvdiv_sb_) {
			pvdiv_sb_->setValue(pos_vdivs_);
			nvdiv_sb_->setValue(neg_vdivs_);
		}
	}

	update_logic_level_offsets();

	if (owner_) {
		// Call order is important, otherwise the lazy event handler won't work
		owner_->extents_changed(false, true);
		owner_->row_item_appearance_changed(false, true);
	}
}

void AnalogSignal::on_neg_vdivs_changed(int vdivs)
{
	if (vdivs == neg_vdivs_)
		return;

	neg_vdivs_ = vdivs;

	// There has to be at least one div, positive or negative
	if ((neg_vdivs_ == 0) && (pos_vdivs_ == 0)) {
		pos_vdivs_ = 1;
		if (pvdiv_sb_)
			pvdiv_sb_->setValue(pos_vdivs_);
	}

	if (autoranging_) {
		perform_autoranging(true, true);

		// It could be that a positive or negative div was added, so update
		if (pvdiv_sb_) {
			pvdiv_sb_->setValue(pos_vdivs_);
			nvdiv_sb_->setValue(neg_vdivs_);
		}
	}

	update_logic_level_offsets();

	if (owner_) {
		// Call order is important, otherwise the lazy event handler won't work
		owner_->extents_changed(false, true);
		owner_->row_item_appearance_changed(false, true);
	}
}

void AnalogSignal::on_div_height_changed(int height)
{
	div_height_ = height;
	update_logic_level_offsets();
	update_scale();

	if (owner_) {
		// Call order is important, otherwise the lazy event handler won't work
		owner_->extents_changed(false, true);
		owner_->row_item_appearance_changed(false, true);
	}
}

void AnalogSignal::on_resolution_changed(int index)
{
	scale_index_ = resolution_cb_->itemData(index).toInt();
	update_scale();

	if (owner_)
		owner_->row_item_appearance_changed(false, true);
}

void AnalogSignal::on_autoranging_changed(int state)
{
	autoranging_ = (state == Qt::Checked);

	if (pvdiv_sb_)
		pvdiv_sb_->setEnabled(!autoranging_);
	if (nvdiv_sb_)
		nvdiv_sb_->setEnabled(!autoranging_);
	if (resolution_cb_)
		resolution_cb_->setEnabled(!autoranging_);

	if (autoranging_)
		perform_autoranging(false, true);

	if (owner_) {
		// Call order is important, otherwise the lazy event handler won't work
		owner_->extents_changed(false, true);
		owner_->row_item_appearance_changed(false, true);
	}
}

void AnalogSignal::on_conversion_changed(int index)
{
	SignalBase::ConversionType old_conv_type = base_->get_conversion_type();

	SignalBase::ConversionType conv_type =
		(SignalBase::ConversionType)(conversion_cb_->itemData(index).toInt());

	if (conv_type != old_conv_type) {
		base_->set_conversion_type(conv_type);
		update_conversion_widgets();

		if (conv_type == SignalBase::ConversionType::NoConversion)
			on_display_type_changed(DisplayType::DisplayAnalog);
		else
			on_display_type_changed(DisplayType::DisplayBoth);

		if (owner_)
			owner_->row_item_appearance_changed(false, true);
	}
}

void AnalogSignal::on_conv_threshold_changed(int index)
{
	SignalBase::ConversionType conv_type = base_->get_conversion_type();

	// Note: index is set to -1 if the text in the combo box matches none of
	// the entries in the combo box

	if ((index == -1) && (conv_threshold_cb_->currentText().length() == 0))
		return;

	// The combo box entry with the custom value has user_data set to -1
	const int user_data = conv_threshold_cb_->findText(
			conv_threshold_cb_->currentText());

	const bool use_custom_thr = (index == -1) || (user_data == -1);

	if (conv_type == SignalBase::A2LConversionByThreshold && use_custom_thr) {
		// Not one of the preset values, try to parse the combo box text
		// Note: Regex loosely based on
		// https://txt2re.com/index-c++.php3?s=0.1V&1&-13
		QString re1 = "([+-]?\\d*[\\.,]?\\d*)"; // Float value
		QString re2 = "([a-zA-Z]*)"; // SI unit
		const QString text = conv_threshold_cb_->currentText();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		QRegularExpression regex(re1 + re2);
		if (!regex.match(text).hasMatch())
			return;  // String doesn't match the regex

		QStringList tokens = regex.match(text).capturedTexts();
#else
		QRegExp regex(re1 + re2);
		if (!regex.exactMatch(text))
			return;  // String doesn't match the regex

		QStringList tokens = regex.capturedTexts();
#endif

		// For now, we simply assume that the unit is volt without modifiers
		const double thr = tokens.at(1).toDouble();

		// Only restart the conversion if the threshold was updated.
		// We're starting a delayed conversion because the user may still be
		// typing and the UI would lag if we kept on restarting it immediately
		if (base_->set_conversion_option("threshold_value", thr))
			base_->start_conversion(true);
	}

	if (conv_type == SignalBase::A2LConversionBySchmittTrigger && use_custom_thr) {
		// Not one of the preset values, try to parse the combo box text
		// Note: Regex loosely based on
		// https://txt2re.com/index-c++.php3?s=0.1V/0.2V&2&14&-22&3&15
		QString re1 = "([+-]?\\d*[\\.,]?\\d*)"; // Float value
		QString re2 = "([a-zA-Z]*)"; // SI unit
		QString re3 = "\\/"; // Forward slash, not captured
		QString re4 = "([+-]?\\d*[\\.,]?\\d*)"; // Float value
		QString re5 = "([a-zA-Z]*)"; // SI unit
		const QString text = conv_threshold_cb_->currentText();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		QRegularExpression regex(re1 + re2 + re3 + re4 + re5);

		if (!regex.match(text).hasMatch())
			return;  // String doesn't match the regex

		QStringList tokens = regex.match(text).capturedTexts();
#else
		QRegExp regex(re1 + re2 + re3 + re4 + re5);

		if (!regex.exactMatch(text))
			return;  // String doesn't match the regex

		QStringList tokens = regex.capturedTexts();
#endif

		// For now, we simply assume that the unit is volt without modifiers
		const double low_thr = tokens.at(1).toDouble();
		const double high_thr = tokens.at(3).toDouble();

		// Only restart the conversion if one of the options was updated.
		// We're starting a delayed conversion because the user may still be
		// typing and the UI would lag if we kept on restarting it immediately
		bool o1 = base_->set_conversion_option("threshold_value_low", low_thr);
		bool o2 = base_->set_conversion_option("threshold_value_high", high_thr);
		if (o1 || o2)
			base_->start_conversion(true);  // Start delayed conversion
	}

	base_->set_conversion_preset((SignalBase::ConversionPreset)index);

	// Immediately start the conversion if we're not using custom values
	// (i.e. we're using one of the presets)
	if (!use_custom_thr)
		base_->start_conversion();
}

void AnalogSignal::on_delayed_conversion_starter()
{
	base_->start_conversion();
}

void AnalogSignal::on_display_type_changed(int index)
{
	const bool prev_showing_logic = (display_type_ == DisplayConverted) || (display_type_ == DisplayBoth);

	display_type_ = (DisplayType)(display_type_cb_->itemData(index).toInt());

	const bool showing_logic = (display_type_ == DisplayConverted) || (display_type_ == DisplayBoth);

	// If we show a logic trace, make sure we have at least one div for each
	// polarity as that's where we paint it
	if (showing_logic && !prev_showing_logic) {
		if (pos_vdivs_ == 0)
			on_pos_vdivs_changed(1);
		if (neg_vdivs_ == 0)
			on_neg_vdivs_changed(1);
	}

	if (owner_)
		owner_->row_item_appearance_changed(false, true);
}

} // namespace trace
} // namespace views
} // namespace pv
