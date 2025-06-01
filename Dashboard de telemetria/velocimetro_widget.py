from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPainter, QPen, QFont, QColor, QConicalGradient
from PyQt5.QtCore import Qt
import math

class VelocimetroWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.velocidade = 0  # valor inicial

    def setVelocidade(self, valor):
        self.velocidade = max(0, min(valor, 300))  # limita entre 0 e 300
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        centro_x = self.width() // 2
        centro_y = self.height() // 2
        raio = min(self.width(), self.height()) // 2 - 20

        # Fundo com gradiente radial clássico
        gradient = QConicalGradient(centro_x, centro_y, 0)
        gradient.setColorAt(0, QColor(0, 0, 0))
        gradient.setColorAt(0.5, QColor(0, 0, 0))
        gradient.setColorAt(1, QColor(0, 0, 0))
        painter.setBrush(gradient)
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Desenha o círculo externo
        painter.setPen(QPen(Qt.black, 3))
        painter.setBrush(Qt.NoBrush)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Desenha marcadores da escala
        steps = 30  # para 300 km/h, marcador a cada 10 km/h
        start_angle = -210  # começa à esquerda
        span_angle = 240   # arco total no sentido horário

        painter.setPen(QPen(Qt.white, 2))
        for i in range(steps + 1):
            angle = start_angle + (span_angle / steps) * i  # percorre do -135 ao +135 graus
            rad = math.radians(angle)

            # Marcador maior
            x1 = centro_x + (raio - 10) * math.cos(rad)
            y1 = centro_y + (raio - 10) * math.sin(rad)
            x2 = centro_x + raio * math.cos(rad)
            y2 = centro_y + raio * math.sin(rad)
            painter.drawLine(int(x1), int(y1), int(x2), int(y2))

            # Números da escala a cada 2 marcadores (20 km/h)
            if i % 2 == 0:
                valor = i * 10
                x_text = centro_x + (raio - 25) * math.cos(rad)
                y_text = centro_y + (raio - 25) * math.sin(rad)

                font = QFont("Arial", 10, QFont.Bold)
                painter.setFont(font)
                rect_x = int(x_text - 15)
                rect_y = int(y_text - 10)
                painter.drawText(rect_x, rect_y, 30, 20, Qt.AlignCenter, str(valor))

        # Calcula o ângulo do ponteiro baseado na velocidade
        angulo_ponteiro = start_angle + (span_angle * self.velocidade / 300)
        rad_ponteiro = math.radians(angulo_ponteiro)
        ponteiro_x = centro_x + (raio - 40) * math.cos(rad_ponteiro)
        ponteiro_y = centro_y + (raio - 40) * math.sin(rad_ponteiro)

        # Desenha a agulha
        painter.setPen(QPen(QColor(255, 0, 0), 4))
        painter.drawLine(centro_x, centro_y, int(ponteiro_x), int(ponteiro_y))

        # Centro da agulha com círculo
        painter.setBrush(QColor(200, 0, 0))
        painter.setPen(QPen(Qt.black, 1))
        painter.drawEllipse(centro_x - 10, centro_y - 10, 20, 20)

        # Texto da velocidade
        painter.setPen(Qt.white)
        font_text = QFont("Arial", 15, QFont.Bold)
        painter.setFont(font_text)
        painter.drawText(centro_x - 50, centro_y + raio // 2, 100, 40, Qt.AlignCenter, f"{self.velocidade:.0f} km/h")
