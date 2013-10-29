#ifndef PLOTTER_H
#define PLOTTER_H

#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>

class QToolButton;
class PlotSettings;

class Plotter : public QWidget
	{
	Q_OBJECT

	public:
	Plotter (QWidget *parent = 0);

	void setPlotSettings (const PlotSettings &settings);
	void setCurveData (int id, const QVector<QPointF> &data);
	void addCurveData (int id, const QPointF &data);
	void clearCurve (int id);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	public slots:
	void zoomIn();
	void zoomOut();

	protected:
	void paintEvent (QPaintEvent *event);
	void resizeEvent (QResizeEvent *event);
	void mousePressEvent (QMouseEvent *event);
	void mouseMoveEvent (QMouseEvent *event);
	void mouseReleaseEvent (QMouseEvent *event);
	void keyPressEvent (QKeyEvent *event);
	void wheelEvent (QWheelEvent *event);

	private:
	void updateRubberBandRegion();
	void refreshPixmap();
	void drawGrid (QPainter *painter);
	void drawCurves (QPainter *painter);

	enum { Margin = 50 };

	QToolButton *zoomInButton;
	QToolButton *zoomOutButton;
	QMap<int, QVector<QPointF> > curveMap;
	QVector<PlotSettings> zoomStack;
	int curZoom;
	bool rubberBandIsShown;
	QRect rubberBandRect;
	QPixmap pixmap;
	};

class PlotSettings
	{
	public:
	PlotSettings();

	// Move the viewport by a given amount of ticks.
	void scrollTicks (int dx, int dy);

	// Move the viewport by a given amount of plot units.
	void scrollUnits (double dx, double dy);

	// Pan (move+scale) the viewport to a given amount of pixels.
	void adjustWindow (int width, int height,
	                   int wMinX, int wMaxX, int wMinY, int wMaxY);

	// Normalize the viewport's scale (recalculate min, max and number of ticks).
	void normalize();

	double spanX() const { return maxX - minX; }
	double spanY() const { return maxY - minY; }

	bool inRangeX (double x) const { return (x > minX) && (x < maxX); }
	bool inRangeY (double y) const { return (y > minY) && (y < maxY); }

	double minX;
	double maxX;
	int numXTicks;
	double minY;
	double maxY;
	int numYTicks;

	private:
	static void normalizeAxis (double &min, double &max, int &numTicks);
	};

#endif
