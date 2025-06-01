from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPainter, QPen, QFont, QColor, QConicalGradient, QRadialGradient, QPainterPath
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
        raio = min(self.width(), self.height()) // 2 - 40

        # Fundo com gradiente radial
        grad_fundo = QRadialGradient(centro_x, centro_y, raio)
        grad_fundo.setColorAt(0, QColor(40, 40, 40))
        grad_fundo.setColorAt(1, QColor(10, 10, 10))
        painter.setBrush(grad_fundo)
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Anel externo
        grad_anel = QConicalGradient(centro_x, centro_y, -90)
        grad_anel.setColorAt(0, QColor(180, 180, 180))
        grad_anel.setColorAt(0.5, QColor(80, 80, 80))
        grad_anel.setColorAt(1, QColor(180, 180, 180))
        pen_anel = QPen()
        pen_anel.setWidth(6)
        pen_anel.setBrush(grad_anel)
        painter.setPen(pen_anel)
        painter.setBrush(Qt.NoBrush)
        painter.drawEllipse(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)

        # Marcadores da escala
        steps = 30  # 10 em 10 km/h
        start_angle = -210
        span_angle = 240

        painter.setPen(QPen(QColor(200, 200, 200), 2))

        for i in range(steps + 1):
            angle = start_angle + (span_angle / steps) * i
            rad = math.radians(angle)

            x1 = centro_x + (raio - 10) * math.cos(rad)
            y1 = centro_y + (raio - 10) * math.sin(rad)
            x2 = centro_x + raio * math.cos(rad)
            y2 = centro_y + raio * math.sin(rad)
            painter.drawLine(int(x1), int(y1), int(x2), int(y2))

            # Ajuste de alinhamento do texto com os traços
            if i % 2 == 0:
                valor = i * 10
                x_text = centro_x + (raio - 25) * math.cos(rad)
                y_text = centro_y + (raio - 25) * math.sin(rad)

                font = QFont("Arial", 9, QFont.Bold)  # tamanho levemente menor
                painter.setFont(font)

                texto = str(valor)
                # Ajuste fino para centralizar o texto
                rect_w = 20
                rect_h = 12
                rect_x = int(x_text - rect_w / 2)
                rect_y = int(y_text - rect_h / 2)

                # Sombra
                painter.setPen(QColor(30, 30, 30, 180))
                painter.drawText(rect_x + 1, rect_y + 1, rect_w, rect_h, Qt.AlignCenter, texto)

                # Texto
                painter.setPen(QColor(230, 230, 230))
                painter.drawText(rect_x, rect_y, rect_w, rect_h, Qt.AlignCenter, texto)

        # Ponteiro
        angulo_ponteiro = start_angle + (span_angle * self.velocidade / 300)
        rad_ponteiro = math.radians(angulo_ponteiro)

        ponteiro_path = QPainterPath()
        ponteiro_path.moveTo(centro_x, centro_y)
        ponteiro_x = centro_x + (raio - 50) * math.cos(rad_ponteiro)
        ponteiro_y = centro_y + (raio - 50) * math.sin(rad_ponteiro)

        angulo_perp = rad_ponteiro + math.pi / 2
        largura_ponteiro = 8
        x1 = centro_x + largura_ponteiro * math.cos(angulo_perp)
        y1 = centro_y + largura_ponteiro * math.sin(angulo_perp)
        x2 = centro_x - largura_ponteiro * math.cos(angulo_perp)
        y2 = centro_y - largura_ponteiro * math.sin(angulo_perp)

        ponteiro_path.lineTo(int(x1), int(y1))
        ponteiro_path.lineTo(int(ponteiro_x), int(ponteiro_y))
        ponteiro_path.lineTo(int(x2), int(y2))
        ponteiro_path.closeSubpath()

        grad_ponteiro = QConicalGradient(centro_x, centro_y, -angulo_ponteiro * 180 / math.pi)
        grad_ponteiro.setColorAt(0, QColor(255, 50, 50))
        grad_ponteiro.setColorAt(1, QColor(180, 0, 0))
        painter.setBrush(grad_ponteiro)
        painter.setPen(QPen(QColor(100, 0, 0), 1))
        painter.drawPath(ponteiro_path)

        # Centro da agulha
        sombra_centro = QRadialGradient(centro_x, centro_y, 15)
        sombra_centro.setColorAt(0, QColor(220, 0, 0))
        sombra_centro.setColorAt(1, QColor(70, 0, 0))
        painter.setBrush(sombra_centro)
        painter.setPen(QPen(QColor(40, 0, 0), 1))
        painter.drawEllipse(centro_x - 15, centro_y - 15, 30, 30)

        # Texto da velocidade
        texto = f"{self.velocidade:.0f} km/h"
        font_text = QFont("Arial", 18, QFont.Bold)  # Tamanho ajustado
        painter.setFont(font_text)

        text_rect_w = 100
        text_rect_h = 40
        text_x = centro_x - text_rect_w // 2
        text_y = centro_y + raio // 2

        # Sombra do texto
        painter.setPen(QColor(0, 0, 0, 150))
        painter.drawText(text_x + 2, text_y + 2, text_rect_w, text_rect_h, Qt.AlignCenter, texto)

        # Texto principal
        painter.setPen(QColor(255, 255, 255))
        painter.drawText(text_x, text_y, text_rect_w, text_rect_h, Qt.AlignCenter, texto)
