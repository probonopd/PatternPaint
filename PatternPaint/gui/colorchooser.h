/*
 * This source file is part of EasyPaint.
 *
 * Copyright (c) 2012 EasyPaint <https://github.com/Gr1N/EasyPaint>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

#include <QWidget>
#include <QColorDialog>
#include <QColor>
#include <QMouseEvent>

/// @brief Widget for selecting color.
class ColorChooser : public QWidget
{
    Q_OBJECT

public:
    /// Constructor
    /// @param color Default color
    /// @param parent Pointer for parent.
    explicit ColorChooser(QWidget *parent = 0);

private:
    void paintEvent(QPaintEvent *event);

    QColor currentColor;

    QColorDialog colorDialog;

    void mousePressEvent(QMouseEvent *event);

public slots:
    /// Set a new color for the widget
    /// @param color new color
    void setColor(const QColor &color);

    /// Handler for the color changed event from QColorDialog
    /// @param color new color
    void on_currentColorChanged(const QColor &color);

signals:
    /// Signal for sending choosen color
    /// @param Color to send
    void sendColor(const QColor &);
};

#endif // COLORCHOOSER_H
