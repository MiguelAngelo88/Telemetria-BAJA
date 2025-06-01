from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPainter, QPen, QFont, QColor, QConicalGradient, QPainterPath
from PyQt5.QtCore import Qt, QRectF
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

        # Fundo com gradiente radial discreto
        grad_fundo = QConicalGradient(centro_x, centro_y, -90)
        grad_fundo.setColorAt(0, QColor(30, 30, 30))
        grad_fundo.setColorAt(0.5, QColor(10, 10, 10))
        grad_fundo.setColorAt(1, QColor(30, 30, 30))
        painter.setBrush(grad_fundo)
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Círculo externo
        pen_anel = QPen(QColor(255, 165, 0), 4)
        painter.setPen(pen_anel)
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

            # Números a cada 1000 RPM
            if i % 2 == 0:
                valor = i * 500
                x_text = centro_x + (raio - 30) * math.cos(rad)
                y_text = centro_y + (raio - 30) * math.sin(rad)

                font = QFont("Arial", 9, QFont.Bold)
                painter.setFont(font)

                rect = QRectF(x_text - 15, y_text - 10, 30, 20)

                # Sombra
                painter.setPen(QColor(30, 30, 30, 180))
                painter.drawText(rect.translated(1, 1), Qt.AlignCenter, str(valor))

                # Texto principal
                painter.setPen(QColor(255, 165, 0))
                painter.drawText(rect, Qt.AlignCenter, str(valor))

        # Ângulo do ponteiro baseado no RPM
        angulo_ponteiro = start_angle + (span_angle * self.rpm / 8000)
        rad_ponteiro = math.radians(angulo_ponteiro)

        # Ponteiro triangular
        ponteiro_path = QPainterPath()
        ponteiro_path.moveTo(centro_x, centro_y)

        comprimento = raio - 40
        ponteiro_x = centro_x + comprimento * math.cos(rad_ponteiro)
        ponteiro_y = centro_y + comprimento * math.sin(rad_ponteiro)

        largura = 6
        angulo_perp = rad_ponteiro + math.pi / 2
        x1 = centro_x + largura * math.cos(angulo_perp)
        y1 = centro_y + largura * math.sin(angulo_perp)
        x2 = centro_x - largura * math.cos(angulo_perp)
        y2 = centro_y - largura * math.sin(angulo_perp)

        ponteiro_path.lineTo(int(x1), int(y1))
        ponteiro_path.lineTo(int(ponteiro_x), int(ponteiro_y))
        ponteiro_path.lineTo(int(x2), int(y2))
        ponteiro_path.closeSubpath()

        grad_ponteiro = QConicalGradient(centro_x, centro_y, -angulo_ponteiro * 180 / math.pi)
        grad_ponteiro.setColorAt(0, QColor(255, 50, 50))
        grad_ponteiro.setColorAt(1, QColor(150, 0, 0))
        painter.setBrush(grad_ponteiro)
        painter.setPen(QPen(QColor(100, 0, 0), 1))
        painter.drawPath(ponteiro_path)

        # Centro do ponteiro
        painter.setBrush(QColor(200, 0, 0))
        painter.setPen(QPen(Qt.black, 1))
        painter.drawEllipse(centro_x - 10, centro_y - 10, 20, 20)

        # Texto RPM no centro inferior
        painter.setPen(QColor(255, 165, 0))  # laranja
        font_text = QFont("Arial", 14, QFont.Bold)
        painter.setFont(font_text)
        painter.drawText(centro_x - 50, centro_y + raio // 2, 100, 40, Qt.AlignCenter, f"{self.rpm} RPM")
