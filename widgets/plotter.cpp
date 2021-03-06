#include "plotter.h"

#include <QtGui/QtGui>
#include <cmath>

Plotter::Plotter (QWidget *parent):
	QWidget (parent)
	{
	setBackgroundRole (QPalette::Dark);
	setAutoFillBackground (true);
	setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy (Qt::StrongFocus);
	rubberBandIsShown = false;

	zoomInButton = new QToolButton (this);
	zoomInButton->setIcon (QIcon::fromTheme ("zoom-in", QIcon (":/images/zoomin.png")));
	zoomInButton->adjustSize();
	connect (zoomInButton, SIGNAL (clicked()), this, SLOT (zoomIn()));

	zoomOutButton = new QToolButton (this);
	zoomOutButton->setIcon (QIcon::fromTheme ("zoom-out", QIcon (":/images/zoomout.png")));
	zoomOutButton->adjustSize();
	connect (zoomOutButton, SIGNAL (clicked()), this, SLOT (zoomOut()));

	setPlotSettings (PlotSettings());
	}

void Plotter::setPlotSettings (const PlotSettings &settings)
	{
	zoomStack.clear();
	zoomStack.append (settings);
	curZoom = 0;
	zoomInButton->hide();
	zoomOutButton->hide();
	refreshPixmap();
	}

void Plotter::zoomOut()
	{
	if (curZoom > 0)
		{
		--curZoom;
		zoomOutButton->setEnabled (curZoom > 0);
		zoomInButton->setEnabled (true);
		zoomInButton->show();
		refreshPixmap();
		}
	}

void Plotter::zoomIn()
	{
	if (curZoom < zoomStack.count() - 1)
		{
		++curZoom;
		zoomInButton->setEnabled (curZoom < zoomStack.count() - 1);
		zoomOutButton->setEnabled (true);
		zoomOutButton->show();
		refreshPixmap();
		}
	}

void Plotter::setCurveData (int id, const QVector<QPointF> &data)
	{
	curveMap[id] = data;
	refreshPixmap();
	}

void Plotter::addCurveData (int id, const QPointF &data)
	{
	QVector<QPointF>& curve = curveMap[id];
	curve.append (data);

	autoScroll (id);

	refreshPixmap();
	}


void Plotter::clearCurve (int id)
	{
	curveMap.remove (id);
	refreshPixmap();
	}

QSize Plotter::minimumSizeHint() const
	{
	return QSize (6 * Margin, 4 * Margin);
	}

QSize Plotter::sizeHint() const
	{
	return QSize (12 * Margin, 8 * Margin);
	}

void Plotter::paintEvent (QPaintEvent * /* event */)
	{
	QStylePainter painter (this);
	painter.drawPixmap (0, 0, pixmap);

	if (rubberBandIsShown)
		{
		painter.setPen (palette().light().color());
		painter.drawRect (rubberBandRect.normalized().adjusted (0, 0, -1, -1));
		}

	if (hasFocus())
		{
		QStyleOptionFocusRect option;
		option.initFrom (this);
		option.backgroundColor = palette().dark().color();
		painter.drawPrimitive (QStyle::PE_FrameFocusRect, option);
		}
	}

void Plotter::resizeEvent (QResizeEvent * /* event */)
	{
	int x = width() - (zoomInButton->width() + zoomOutButton->width() + 10);
	zoomInButton->move (x, 5);
	zoomOutButton->move (x + zoomInButton->width() + 5, 5);
	refreshPixmap();
	}

void Plotter::mousePressEvent (QMouseEvent *event)
	{
	QRect rect (Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);

	if (event->button() == Qt::LeftButton && rect.contains (event->pos()))
		{
		rubberBandIsShown = true;
		rubberBandRect.setTopLeft (event->pos());
		rubberBandRect.setBottomRight (event->pos());
		updateRubberBandRegion();
		setCursor (Qt::CrossCursor);
		}
	}

void Plotter::mouseMoveEvent (QMouseEvent *event)
	{
	if (rubberBandIsShown)
		{
		updateRubberBandRegion();
		rubberBandRect.setBottomRight (event->pos());
		updateRubberBandRegion();
		}
	}

void Plotter::mouseReleaseEvent (QMouseEvent *event)
	{
	if ( (event->button() == Qt::LeftButton) && rubberBandIsShown)
		{
		rubberBandIsShown = false;
		updateRubberBandRegion();
		unsetCursor();

		QRect rect = rubberBandRect.normalized();

		if (rect.width() < 4 || rect.height() < 4) return;

		rect.translate (-Margin, -Margin);

		PlotSettings settings = zoomStack[curZoom];
		settings.adjustWindow (width() - 2 * Margin, height() - 2 * Margin,
		                       rect.left(), rect.right(), rect.top(), rect.bottom());
		settings.normalize();

		zoomStack.resize (curZoom + 1);
		zoomStack.append (settings);
		zoomIn();
		}
	}

void Plotter::keyPressEvent (QKeyEvent *event)
	{
	switch (event->key())
		{
		case Qt::Key_Plus:  zoomIn();  break;
		case Qt::Key_Minus: zoomOut(); break;

		case Qt::Key_Left:
			zoomStack[curZoom].scrollTicks (-1, 0);
			refreshPixmap();
			break;
		case Qt::Key_Right:
			zoomStack[curZoom].scrollTicks (+1, 0);
			refreshPixmap();
			break;
		case Qt::Key_Down:
			zoomStack[curZoom].scrollTicks (0, -1);
			refreshPixmap();
			break;
		case Qt::Key_Up:
			zoomStack[curZoom].scrollTicks (0, +1);
			refreshPixmap();
			break;

		default: QWidget::keyPressEvent (event);
		}
	}

void Plotter::wheelEvent (QWheelEvent *event)
	{
	int numDegrees = event->delta() / 8;
	int numTicks = numDegrees / 15;

	if (event->orientation() == Qt::Horizontal)
		zoomStack[curZoom].scrollTicks (numTicks, 0);
	else zoomStack[curZoom].scrollTicks (0, numTicks);

	refreshPixmap();
	}

void Plotter::updateRubberBandRegion()
	{
	QRect rect = rubberBandRect.normalized();
	update (rect.left(), rect.top(), rect.width(), 1);
	update (rect.left(), rect.top(), 1, rect.height());
	update (rect.left(), rect.bottom(), rect.width(), 1);
	update (rect.right(), rect.top(), 1, rect.height());
	}

void Plotter::refreshPixmap()
	{
	pixmap = QPixmap (size());
	pixmap.fill (palette().color (backgroundRole()));

	QPainter painter (&pixmap);
	painter.initFrom (this);
	drawGrid   (&painter);
	drawCurves (&painter);
	update();
	}

void Plotter::drawGrid (QPainter *painter)
	{
	QRect rect (Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);

	if (!rect.isValid()) return;

	PlotSettings settings = zoomStack[curZoom];
	QPen quiteDark = palette().dark().color().lighter();
	QPen light = palette().light().color();

	for (int i = 0; i <= settings.numXTicks; ++i)
		{
		int x = rect.left() + (i * (rect.width() - 1) / settings.numXTicks);
		double label = settings.minX + (i * settings.spanX() / settings.numXTicks);

		painter->setPen (quiteDark);
		painter->drawLine (x, rect.top(), x, rect.bottom());
		painter->setPen (light);
		painter->drawLine (x, rect.bottom(), x, rect.bottom() + 5);
		painter->drawText (x - 50, rect.bottom() + 5, 100, 20,
		                   Qt::AlignHCenter | Qt::AlignTop,
		                   QString::number (label));
		}

	for (int j = 0; j <= settings.numYTicks; ++j)
		{
		int y = rect.bottom() - (j * (rect.height() - 1) / settings.numYTicks);
		double label = settings.minY + (j * settings.spanY() / settings.numYTicks);

		painter->setPen (quiteDark);
		painter->drawLine (rect.left(), y, rect.right(), y);
		painter->setPen (light);
		painter->drawLine (rect.left() - 5, y, rect.left(), y);
		painter->drawText (rect.left() - Margin, y - 10, Margin - 5, 20,
		                   Qt::AlignRight | Qt::AlignVCenter,
		                   QString::number (label));
		}

	painter->setPen (light);
	painter->drawRect (rect.adjusted (0, 0, -1, -1));
	}

void Plotter::drawCurves (QPainter *painter)
	{
	static const QColor colorForIds[6] =
		{ Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow };

	PlotSettings settings = zoomStack[curZoom];
	QRect rect (Margin, Margin, width() - 2 * Margin, height() - 2 * Margin);

	if (!rect.isValid()) return;

	painter->setClipRect (rect.adjusted (+1, +1, -1, -1));

	QMapIterator<int, QVector<QPointF> > i (curveMap);

	while (i.hasNext())
		{
		i.next();

		int id = i.key();
		QVector<QPointF> data = i.value();
		QPolygonF polyline (data.count());

		for (int j = 0; j < data.count(); ++j)
			{
			double dx = data[j].x() - settings.minX;
			double dy = data[j].y() - settings.minY;
			double x = rect.left() + (dx * (rect.width() - 1) / settings.spanX());
			double y = rect.bottom() - (dy * (rect.height() - 1) / settings.spanY());
			polyline[j] = QPointF (x, y);
			}

		painter->setPen (colorForIds[uint (id) % 6]);
		painter->drawPolyline (polyline);
		}
	}

void Plotter::autoScroll (int id)
	{
	QVector<QPointF> curve = curveMap[id];

	if (curve.size() < 2) return;

	/*
	 * Logic: if the curve's old last point lies in the viewport (by X)
	 *        and the curve's new last (i. e. inserted) point lies outside of the viewport,
	 *        scroll the viewport by X to their delta.
	 */

	double newX = (curve.end() - 1)->x(),
	       oldX = (curve.end() - 2)->x(),
	       dX = newX - oldX;

	for (QVector<PlotSettings>::iterator it = zoomStack.begin(); it != zoomStack.end(); ++it)
		{
		// if a point is outside of N-th zoom level, then it's automatically outside of all next zoom levels.
		if (!it->inRangeX (oldX)) break;

		if (!it->inRangeX (newX)) it->scrollTicks (ceil (dX / (it->spanX() / it->numXTicks)), 0);
		}
	}

PlotSettings::PlotSettings()
	{
	minX = 0.0;
	maxX = 10.0;
	numXTicks = 5;

	minY = 0.0;
	maxY = 10.0;
	numYTicks = 5;
	}

void PlotSettings::scrollUnits(double dx, double dy)
	{
	minX += dx;
	maxX += dx;

	minY += dy;
	maxY += dy;
	}

void PlotSettings::scrollTicks (int dx, int dy)
	{
	double stepX = spanX() / numXTicks;
	minX += dx * stepX;
	maxX += dx * stepX;

	double stepY = spanY() / numYTicks;
	minY += dy * stepY;
	maxY += dy * stepY;
	}

void PlotSettings::adjustWindow (int width, int height, int wMinX, int wMaxX, int wMinY, int wMaxY)
	{
	double scaleX = spanX() / width;
	maxX = minX + scaleX * wMaxX;
	minX = minX + scaleX * wMinX;

	double scaleY = spanY() / height;
	minY = maxY - scaleY * wMaxY;
	maxY = maxY - scaleY * wMinY;
	}

void PlotSettings::normalize()
	{
	normalizeAxis (minX, maxX, numXTicks);
	normalizeAxis (minY, maxY, numYTicks);
	}

void PlotSettings::normalizeAxis (double &min, double &max, int &numTicks)
	{
	const int MinTicks = 4;
	double grossStep = (max - min) / MinTicks;
	double step = std::pow (10.0, std::floor (std::log10 (grossStep)));

	if (5 * step < grossStep)      step *= 5;
	else if (2 * step < grossStep) step *= 2;

	numTicks = int (std::ceil (max / step) - std::floor (min / step));

	if (numTicks < MinTicks) numTicks = MinTicks;

	min = std::floor (min / step) * step;
	max = std::ceil (max / step) * step;
	}

#include "plotter.moc"
