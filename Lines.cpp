#include <QPainter>
#include <iostream>
#include <QTimer>
#include <QKeyEvent>
#include <QTest>
#include <windows.h>
#include "Lines.h"


Lines::Lines(int width, int height, int count, vector<vector<int>> &arr, vector<vector<int>> &map)
        : size{0, 0, 0, 0, width, height, count}, points(arr), map(map) {
    int arrSize = (int) points.size();
    map.resize(count + 2);
    for (int i = 0; i <= count + 1; ++i) map[i].resize(count + 2);
    for (int i = 0; i < arrSize; ++i) {
        for (int j = 0; j < arrSize; ++j) {
            points[i][j] = arr[i][j];
        }
    }
    resize(width, height);


}

void Lines::mousePressEvent(QMouseEvent *ev) {
    const QPoint p = ev->pos();

    double dx = (float) size.width / (float) size.count;
    double dy = (float) size.height / (float) size.count;
    map[p.x() / dx][p.y() / dy] = 2;
    update();
}

void Lines::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Space) {
        fill();
        update();
    }
    if (event->key() == Qt::Key_Backspace) {
        mapClean();
        update();
    }
}

void Lines::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);

    QPainter painter(this);
    drawPoligon(painter);
    drawMap(painter);
    drawGrid(painter);
    drawAxes(painter);
}

void Lines::drawPoligon(QPainter &painter) {
    QColor color(0, 0, 0);
    painter.setBrush(QBrush(color));
    painter.setPen(color);
    for (int i = 0; i < points.size() - 1; ++i) {
        drawCDA(points[i][0], points[i + 1][0], points[i][1], points[i + 1][1], painter);
    }
    drawCDA(points[0][0], points[points.size() - 1][0], points[0][1], points[points.size() - 1][1], painter);
}

void Lines::drawMap(QPainter &painter) {

    QColor purple(255, 0, 255);
    QColor black(0, 0, 0);
    for (int i = 0; i < size.count; ++i) {
        for (int j = 0; j < size.count; ++j) {
            if (map[i][j] == 1) {
                painter.setBrush(QBrush(black));
                painter.setPen(black);
                drawMediumPixel(i, j, painter);
            }
            if (map[i][j] == 2) {
                painter.setBrush(QBrush(purple));
                painter.setPen(purple);
                drawMediumPixel(i, j, painter);

            }
        }

    }
}

/*
void Lines::fill() {
    int point;
    bool flag;
    for (int i = 1; i <= count; ++i) {
        flag = false;
        for (int j = 1; j <= size.count; ++j) {
            if (map[i][j] == 1) {
                if (map[i][j - 1] == 1) {
                    point = j + 1;
                } else {
                    if (flag) {
                        if (map[i - 1][point] != 0) {
                            for (int k = point; k < j; ++k) {
                                map[i][k] = 2;
                            }
                        } else {
                            point = j + 1;
                        }
                    } else {
                        point = j + 1;
                        flag = !flag;
                    }
                }
            }

        }
    }
}*/
void Lines::fillPixel(int x, int y) {
    if (x >= 0 && x < size.count && y >= 0 & y < size.count) {
        if (map[x][y] == 0) map[x][y] = 3;
    }
}

void Lines::fill() {

    for (int i = 0; i < size.count; ++i) {
        for (int j = 0; j < size.count; ++j) {
            if (map[i][j] == 2) {
                fillPixel(i - 1, j);
                fillPixel(i + 1, j);
                fillPixel(i, j + 1);
                fillPixel(i, j - 1);
            }
        }
    }
    for (int i = 0; i < size.count; ++i) {
        for (int j = 0; j < size.count; ++j) {
            if (map[i][j] == 3) map[i][j] = 2;
        }
    }
}

void Lines::mapClean() {
    for (int i = 0; i < size.count; ++i) {
        for (int j = 0; j < size.count; ++j) {
            map[i][j] = 0;
        }
    }
}

void Lines::drawGrid(QPainter &painter) const {
    painter.setPen(Qt::black);
    painter.setPen(Qt::DotLine);

    //int bigPixelCount = size.count / points.size();
    for (int i = 0; i < size.count; ++i) {
        painter.drawLine((int) ((float) i / (float) size.count * (float) size.width), 0,
                         (int) ((float) i / (float) size.count * (float) size.width),
                         size.height);
    }
    for (int i = 0; i < size.count; ++i) {
        painter.drawLine(0, (int) ((float) i / (float) size.count * (float) size.height), size.width,
                         (int) ((float) i / (float) size.count * (float) size.height));
    }
}

void Lines::drawAxes(QPainter &painter) const {
    QPen pen;
    pen.setColor("red");
    pen.setWidth(3);
    painter.setPen(pen);
    int dx = (int) ((float) size.width / 2);
    int dy = (int) ((float) size.height / 2);

    painter.drawLine(dx, 0, dx, size.height);
    painter.drawLine(0, dy, size.width, dy);
}

int sign(double x) {
    if (x > 0) return 1;
    else if (x == 0) return 0;
    else
        return -1;
}

void Lines::drawMediumPixel(int x, int y, QPainter &painter) {
    double dx = (float) size.width / (float) size.count;
    double dy = (float) size.height / (float) size.count;
    painter.drawRect(x * dx, y * dy, dx, dy);
}

void Lines::drawCDA(int X1, int X2, int Y1, int Y2, QPainter &painter) {
    double dx, dy, l, x, y;
    int _x, _y;
    if (abs(X2 - X1) >= abs(Y2 - Y1)) l = abs(X2 - X1);
    else {
        l = abs(Y2 - Y1);
    }
    dx = (X2 - X1) / l;
    dy = (Y2 - Y1) / l;

    x = X1 + 0.5 * sign(dx);
    y = Y1 + 0.5 * sign(dy);
    if (l == 0) return;
    for (int i = 0; i <= l; ++i) {
        decardToDigital(floor(x), floor(y), _x, _y);
        map[_x][_y] = 1;
        x = x + dx;
        y = y + dy;
    }
}

void Lines::decardToDigital(int x, int y, int &X, int &Y) const {
    X = (size.count / 2 + x);
    Y = (size.count / 2 - y);
}

