from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPainter, QPen, QFont, QColor, QConicalGradient
from PyQt5.QtCore import Qt
import math

class RPMWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.rpm = 0  # valor inicial

    def setRPM(self, valor):
        self.rpm = max(0, min(valor, 8000))  # limita entre 0 e 8000 RPM
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        centro_x = self.width() // 2
        centro_y = self.height() // 2
        raio = min(self.width(), self.height()) // 2 - 20

        # Fundo preto sólido
        painter.setBrush(QColor(0, 0, 0))
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Círculo externo
        painter.setPen(QPen(Qt.black, 3))
        painter.setBrush(Qt.NoBrush)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Marcadores da escala RPM
        steps = 16  # a cada 500 RPM, total 8000 RPM
        start_angle = -210  # começo do ponteiro (esquerda)
        span_angle = 240    # arco no sentido horário

        painter.setPen(QPen(QColor(255, 165, 0), 2))  # laranja para marcadores

        for i in range(steps + 1):
            angle = start_angle + (span_angle / steps) * i
            rad = math.radians(angle)

            x1 = centro_x + (raio - 10) * math.cos(rad)
            y1 = centro_y + (raio - 10) * math.sin(rad)
            x2 = centro_x + raio * math.cos(rad)
            y2 = centro_y + raio * math.sin(rad)
            painter.drawLine(int(x1), int(y1), int(x2), int(y2))

            # Números a cada 2 marcadores (1000 RPM)
            if i % 2 == 0:
                valor = i * 500
                x_text = centro_x + (raio - 30) * math.cos(rad)
                y_text = centro_y + (raio - 30) * math.sin(rad)

                font = QFont("Arial", 10, QFont.Bold)
                painter.setFont(font)
                rect_x = int(x_text - 15)
                rect_y = int(y_text - 10)
                painter.drawText(rect_x, rect_y, 30, 20, Qt.AlignCenter, str(valor))

        # Ângulo do ponteiro baseado no RPM
        angulo_ponteiro = start_angle + (span_angle * self.rpm / 8000)
        rad_ponteiro = math.radians(angulo_ponteiro)
        ponteiro_x = centro_x + (raio - 40) * math.cos(rad_ponteiro)
        ponteiro_y = centro_y + (raio - 40) * math.sin(rad_ponteiro)

        # Desenha ponteiro vermelho
        painter.setPen(QPen(QColor(255, 0, 0), 4))
        painter.drawLine(centro_x, centro_y, int(ponteiro_x), int(ponteiro_y))

        # Centro do ponteiro
        painter.setBrush(QColor(200, 0, 0))
        painter.setPen(QPen(Qt.black, 1))
        painter.drawEllipse(centro_x - 10, centro_y - 10, 20, 20)

        # Texto RPM no centro inferior
        painter.setPen(QColor(255, 165, 0))  # laranja
        font_text = QFont("Arial", 15, QFont.Bold)
        painter.setFont(font_text)
        painter.drawText(centro_x - 50, centro_y + raio // 2, 100, 40, Qt.AlignCenter, f"{self.rpm} RPM")
