#include <QPainter>
#include <QApplication>
#include "arcana.h"

Arcana::Arcana(QWidget *parent)
    : QWidget(parent) {

  x = 0;
  gameOver = false;
  gameWon = false;
  paused = false;
  gameStarted = false;
  ball = new Ball();
  platform = new Platform();

  int k = 0;

  for (int i=0; i<5; i++) {
    for (int j=0; j<6; j++) {
      bricks[k] = new Brick(j*40+30, i*10+50);
      k++;
    }
  }
}

Arcana::~Arcana() {

 delete ball;
 delete platform;

 for (int i=0; i<N_OF_BRICKS; i++) {
   delete bricks[i];
 }
}

void Arcana::paintEvent(QPaintEvent *e) {

  Q_UNUSED(e);

  QPainter painter(this);

  if (gameOver) {

    finishGame(&painter, "Game lost");

  } else if(gameWon) {

    finishGame(&painter, "Victory");
  }
  else {

    drawObjects(&painter);
  }
}

void Arcana::finishGame(QPainter *painter, QString message) {

  QFont font("Courier", 15, QFont::DemiBold);
  QFontMetrics fm(font);
  int textWidth = fm.width(message);

  painter->setFont(font);
  int h = height();
  int w = width();

  painter->translate(QPoint(w/2, h/2));
  painter->drawText(-textWidth/2, 0, message);
}

void Arcana::drawObjects(QPainter *painter) {

  painter->drawImage(ball->getRect(), ball->getImage());
  painter->drawImage(platform->getRect(), platform->getImage());

  for (int i=0; i<N_OF_BRICKS; i++) {
    if (!bricks[i]->isDestroyed()) {
      painter->drawImage(bricks[i]->getRect(), bricks[i]->getImage());
    }
  }
}

void Arcana::timerEvent(QTimerEvent *e) {

  Q_UNUSED(e);

  moveObjects();
  checkCollision();
  repaint();
}

void Arcana::moveObjects() {

  ball->autoMove();
  platform->move();
}

void Arcana::keyReleaseEvent(QKeyEvent *e) {

    int dx = 0;

    switch (e->key()) {
        case Qt::Key_Left:
            dx = 0;
            platform->setDx(dx);
            break;

        case Qt::Key_Right:
            dx = 0;
            platform->setDx(dx);
            break;
    }
}

void Arcana::keyPressEvent(QKeyEvent *e) {

    int dx = 0;

    switch (e->key()) {
    case Qt::Key_Left:

        dx = -1;
        platform->setDx(dx);

        break;

    case Qt::Key_Right:

        dx = 1;
        platform->setDx(dx);
        break;

    case Qt::Key_P:

        pauseGame();
        break;

    case Qt::Key_Space:

        startGame();
        break;

    case Qt::Key_Escape:

        qApp->exit();
        break;

    default:
        QWidget::keyPressEvent(e);
    }
}

void Arcana::startGame() {

  if (!gameStarted) {
    ball->resetState();
    platform->resetState();

    for (int i=0; i<N_OF_BRICKS; i++) {
      bricks[i]->setDestroyed(false);
    }

    gameOver = false;
    gameWon = false;
    gameStarted = true;
    timerId = startTimer(DELAY);
  }
}

void Arcana::pauseGame() {

  if (paused) {

    timerId = startTimer(DELAY);
    paused = false;
  } else {

    paused = true;
    killTimer(timerId);
  }
}

void Arcana::stopGame() {

  killTimer(timerId);
  gameOver = true;
  gameStarted = false;
}

void Arcana::victory() {

  killTimer(timerId);
  gameWon = true;
  gameStarted = false;
}

void Arcana::checkCollision() {

  if (ball->getRect().bottom() > BOTTOM_EDGE) {
    stopGame();
  }

  for (int i=0, j=0; i<N_OF_BRICKS; i++) {

    if (bricks[i]->isDestroyed()) {
      j++;
    }

    if (j == N_OF_BRICKS) {
      victory();
    }
  }

  if ((ball->getRect()).intersects(platform->getRect())) {

    int platformLPos = platform->getRect().left();
    int ballLPos = ball->getRect().left();

    int first = platformLPos + 8;
    int second = platformLPos + 16;
    int third = platformLPos + 24;
    int fourth = platformLPos + 32;

    if (ballLPos < first) {
      ball->setXDir(-1);
      ball->setYDir(-1);
    }

    if (ballLPos >= first && ballLPos < second) {
      ball->setXDir(-1);
      ball->setYDir(-1*ball->getYDir());
    }

    if (ballLPos >= second && ballLPos < third) {
       ball->setXDir(0);
       ball->setYDir(-1);
    }

    if (ballLPos >= third && ballLPos < fourth) {
       ball->setXDir(1);
       ball->setYDir(-1*ball->getYDir());
    }

    if (ballLPos > fourth) {
      ball->setXDir(1);
      ball->setYDir(-1);
    }
  }

  for (int i=0; i<N_OF_BRICKS; i++) {

    if ((ball->getRect()).intersects(bricks[i]->getRect())) {

      int ballLeft = ball->getRect().left();
      int ballHeight = ball->getRect().height();
      int ballWidth = ball->getRect().width();
      int ballTop = ball->getRect().top();

      QPoint pointRight(ballLeft + ballWidth + 1, ballTop);
      QPoint pointLeft(ballLeft - 1, ballTop);
      QPoint pointTop(ballLeft, ballTop -1);
      QPoint pointBottom(ballLeft, ballTop + ballHeight + 1);

      if (!bricks[i]->isDestroyed()) {
        if(bricks[i]->getRect().contains(pointRight)) {
           ball->setXDir(-1);
        }

        else if(bricks[i]->getRect().contains(pointLeft)) {
           ball->setXDir(1);
        }

        if(bricks[i]->getRect().contains(pointTop)) {
           ball->setYDir(1);
        }

        else if(bricks[i]->getRect().contains(pointBottom)) {
           ball->setYDir(-1);
        }

        bricks[i]->setDestroyed(true);
      }
    }
  }
}
