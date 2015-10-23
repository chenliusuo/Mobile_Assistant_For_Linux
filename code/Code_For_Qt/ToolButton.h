#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QToolButton>

class CoolButton : public QToolButton
{
    Q_OBJECT

private:
    QIcon m_NormalIcon;
    QIcon m_HoverIcon;
    QIcon m_PressedIcon;

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public:
    CoolButton(QWidget *parent=0);
    virtual ~CoolButton();
    void SetButtonIcons(const QIcon &normal, const QIcon &hover =QIcon(),const QIcon &pressed = QIcon() );
};

#endif // TOOLBUTTON_H
