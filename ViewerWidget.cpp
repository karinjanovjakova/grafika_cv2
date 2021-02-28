#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) {
		data[x + y * img->width()] = color.rgb();
	}
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

//kresliace funkcie

void ViewerWidget::usecka_DDA(QPoint A, QPoint B, QColor color) {
	int x, y, x2, y2 ;
	double chyba = 0;
	if (B.y() == A.y()) {
		int delta = abs(A.x() - B.x()), x1;
		if (A.x() < B.x())
			x1 = A.x();
		else
			x1 = B.x();
		for (x = 0; x < delta; x++)
			setPixel(x+x1, A.y(), color);
	}
	else {
		double smernica = (B.y() - A.y()) / (double)(B.x() - A.x());
		double posun;
		if (abs(smernica) < 1) {											//riadiaca os x
			if (A.x() < B.x()) {
				x = A.x(); y = A.y(); x2 = B.x();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
			}
			else if (A.x() == B.x()) {
				int delta = abs(A.y() - B.y()), y1;
				if (A.y() < B.y())
					y1 = A.y();
				else
					y1 = B.y();
				for (y = 0; y < delta; y++)
					setPixel(y + y1, A.x(), color);
			}
				
			for (x; x < x2; x++) {
				setPixel(x, (int)y, color);
				posun += smernica;
				if (posun > 0.5) {
					y += (int)smernica;
					if ((int)smernica == 0)
						y++;
					posun += -1;
				}
				if (posun < -0.5) {
					y+=-(int) smernica;
					if ((int)smernica == 0)
						y--;
					posun += 1;
				}
			}
		}
		else {																//riadiaca os y
			if (A.y() > B.y())
				 y = A.y(), x = A.x(), y2 = B.y();
			else if (A.y() < B.y())
				 y = B.y(), x = B.x(), y2 = A.y();
			for (y; y > y2; y--) {
				setPixel(x, (int)y, color);
				posun += 1 / smernica;
				if (posun > 0.5) {
					x--;
					posun += -1;
				}
				if (posun < -0.5) {
					x++;
					posun += 1;
				}
			}
		}
	}
	update();
}

void ViewerWidget::usecka_Bresenham(QPoint A, QPoint B, QColor color) {
	int dx, dy, p, x, y, x2, y2;
	if (B.y() == A.y()) {
		int delta = abs(A.x() - B.x()), x1;
		if (A.x() < B.x())
			x1 = A.x();
		else
			x1 = B.x();
		for (x = 0; x < delta; x++)
			setPixel(x + x1, A.y(), color);
	}
	else {
		double smernica = (B.y() - A.y()) / (double)(B.x() - A.x());
		if (smernica < 1 && smernica >= 0) {
			if (A.x() < B.x()) {
				x = A.x();	y = A.y();	x2 = B.x();
				dx = B.x() - A.x();	dy = B.y() - A.y();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dy - dx;
			do {
				if (p > 0)
				{
					y++;
					p += 2 * dy - 2 * dx;
				}
				else
				{
					p = p + 2 * dy;
				}
				x++;
				setPixel(x, y, color);
			} while (x < x2);
		}
		else if (smernica > -1 && smernica < 0) {
			if (A.x() < B.x()) {
				x = A.x();	y = A.y();	x2 = B.x();
				dx = B.x() - A.x();	dy = B.y() - A.y();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dy + dx;
			do {
				if (p < 0)
				{
					y--;
					p += 2 * dy + 2 * dx;
				}
				else
				{
					p += 2 * dy;
				}
				x++;
				setPixel(x, y, color);
			} while (x < x2);
		}
		else if (smernica < -1) {
			if (A.y() < B.y()) {
				y = A.y();	x = A.x();	y2 = B.y();
				dy = B.y() - A.y();	dx = B.x() - A.x();
			}
			else if (A.y() > B.y()) {
				y = B.y(); x = B.x(); y2 = A.y();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dx + dy;
			do {
				if (p < 0)
				{
					x--;
					p += 2 * dx + 2 * dy;
				}
				else
				{
					p += 2 * dx;
				}
				y++;
				setPixel(x, y, color);
			} while (y < y2);
		}
		else if (smernica > 1) {
			if (A.y() < B.y()) {
				y = A.y();	x = A.x();	y2 = B.y();
				dx = B.x() - A.x();	dy = B.y() - A.y();
			}
			else if (A.y() > B.y()) {
				y = B.y(); x = B.x(); y2 = A.y();
				dx = A.x() - B.x();	dy = A.y() - B.y();
			}
			p = 2 * dx - dy;
			do {
				if (p > 0)
				{
					x++;
					p += 2 * dx - 2 * dy;
				}
				else
				{
					p += 2 * dx;
				}
				y++;
				setPixel(x, y, color);
			} while (y < y2);
		}
	}

	update();
}

void ViewerWidget::kruznica(QPoint A, QPoint B, QColor color) {
	int a = abs(A.x() - B.x()), b = abs(A.y() - B.y()), r = sqrt((a * a) + (b * b)),
		p = 1 - r, x = 0, y = r, dvaX = 3, dvaY = 2 * r + 2;
	do {
		setPixel(x+A.x(), y+A.y(), color);
		setPixel(-x + A.x(), y + A.y(), color);
		setPixel(-x + A.x(), -y + A.y(), color);
		setPixel(x + A.x(), -y + A.y(), color);
		setPixel(y + A.x(), x + A.y(), color);
		setPixel(-y + A.x(), x + A.y(), color);
		setPixel(-y + A.x(), -x + A.y(), color);
		setPixel(y + A.x(), -x + A.y(), color);
		if (p > 0) {
			p += -dvaY;
			y--;
			dvaY += -2;
		}
		p += dvaX;
		dvaX += 2;
		x++;
	} while (x < y);
	update();
}