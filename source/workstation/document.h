#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QUndoStack)
QT_FORWARD_DECLARE_CLASS(QTextStream)

class Shape
{
public:
    enum Type { Rectangle, Circle, Triangle };

    Shape(Type type = Rectangle, const QColor &color = Qt::red, const QRect &rect = QRect());

    Type type() const;
    QString name() const;
    QRect rect() const;
    QRect resizeHandle() const;
    QColor color() const;

    static QString typeToString(Type type);
    static Type stringToType(const QString &s, bool *ok = 0);

    static const QSize minSize;

private:
    Type m_type;
    QRect m_rect;
    QColor m_color;
    QString m_name;

    friend class Document;
};

class Document : public QWidget
{
    Q_OBJECT

public:
    Document(QWidget *parent = 0);

    QString addShape(const Shape &shape);
    void deleteShape(const QString &shapeName);
    Shape shape(const QString &shapeName) const;
    QString currentShapeName() const;

    void setShapeRect(const QString &shapeName, const QRect &rect);
    void setShapeColor(const QString &shapeName, const QColor &color);
	void toggleClippingState();

    bool load(QTextStream &stream);
    void save(QTextStream &stream);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QUndoStack *undoStack() const;

signals:
    void currentShapeChanged(const QString &shapeName);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void setCurrentShape(int index);
    int indexOf(const QString &shapeName) const;
    int indexAt(const QPoint &pos) const;
    QString uniqueName(const QString &name) const;

    QList<Shape> m_shapeList;
    int m_currentIndex;
    int m_mousePressIndex;
    QPoint m_mousePressOffset;
    bool m_resizeHandlePressed;
    QString m_fileName;

	bool m_clippingState;

    QUndoStack *m_undoStack;
};

#endif // DOCUMENT_H
