from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel
from PyQt5.QtGui import QPainter, QPen, QFont, QColor
from PyQt5.QtCore import Qt, QRectF
import math

class TemperaturaWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.temperatura = 70  # Valor inicial
        self.setMinimumSize(250, 250)
        self.setMaximumSize(350, 350)

        # Layout interno
        self.internal_layout = QVBoxLayout(self)
        self.internal_layout.setAlignment(Qt.AlignTop | Qt.AlignRight)

        # Texto de temperatura
        self.temp_label = QLabel(f"{self.temperatura}°C")
        self.temp_label.setAlignment(Qt.AlignRight)
        self.temp_label.setStyleSheet("font-size: 20px; font-weight: bold; color: #1E90FF;")  # Azul moderno
        self.internal_layout.addWidget(self.temp_label)

    def setTemperatura(self, valor):
        self.temperatura = max(0, min(valor, 120))
        self.temp_label.setText(f"CVT: {self.temperatura}°C")
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        widget_width = self.width()
        widget_height = self.height()

        if widget_width <= 0 or widget_height <= 0:
            return

        label_height = self.temp_label.sizeHint().height()
        available_dial_height = widget_height - label_height - 20  # Ajuste de margem

        raio = min(widget_width, available_dial_height) // 2 - 20
        if raio <= 0:
            return

        centro_x = widget_width // 2
        centro_y = label_height + (available_dial_height // 2)

        rect = QRectF(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)
        largura_barra = 20
        start_angle_drawing_deg = 270
        span_angle_drawing_deg = 180
        temp_min_scale = 0
        temp_max_scale = 121
        temp_range_scale = temp_max_scale - temp_min_scale

        # Zonas de cores
        zonas = [
            (0, 60, QColor(50, 205, 50)),  # Verde claro
            (60, 100, QColor(255, 140, 0)),  # Laranja escuro
            (100, 120, QColor(220, 20, 60))  # Vermelho carmim
        ]

        for zona_min, zona_max, cor in zonas:
            proporcao_zona = (zona_min - temp_min_scale) / temp_range_scale
            span_zona = (zona_max - zona_min) / temp_range_scale
            start_zona_deg = start_angle_drawing_deg + proporcao_zona * span_angle_drawing_deg
            span_zona_deg = span_zona * span_angle_drawing_deg

            painter.setPen(QPen(cor, largura_barra, Qt.SolidLine, Qt.RoundCap))
            painter.drawArc(rect, int(start_zona_deg * 16), int(span_zona_deg * 16))

        # Números
        painter.setFont(QFont("Arial", 10, QFont.Bold))
        painter.setPen(QColor(220, 220, 220))  # Cinza claro

        raio_numeros = raio + 2
        for temp_value in range(temp_min_scale, temp_max_scale + 1, 20):
            proporcao = (temp_value - temp_min_scale) / temp_range_scale
            angulo_deg = start_angle_drawing_deg + proporcao * span_angle_drawing_deg
            angulo_rad = math.radians(angulo_deg)

            x = centro_x + raio_numeros * math.cos(angulo_rad)
            y = centro_y - raio_numeros * math.sin(angulo_rad)

            text = str(temp_value)
            text_rect = QRectF(x - 10, y - 10, 20, 20)
            painter.drawText(text_rect, Qt.AlignCenter, text)

        # Ticks
        tick_start = raio - largura_barra + 2
        tick_end = tick_start + 8
        painter.setPen(QPen(QColor(200, 200, 200), 2))

        for temp_value in range(temp_min_scale, temp_max_scale + 1, 20):
            proporcao = (temp_value - temp_min_scale) / temp_range_scale
            angulo_deg = start_angle_drawing_deg + proporcao * span_angle_drawing_deg
            angulo_rad = math.radians(angulo_deg)

            x_start = centro_x + tick_start * math.cos(angulo_rad)
            y_start = centro_y - tick_start * math.sin(angulo_rad)
            x_end = centro_x + tick_end * math.cos(angulo_rad)
            y_end = centro_y - tick_end * math.sin(angulo_rad)

            painter.drawLine(int(x_start), int(y_start), int(x_end), int(y_end))

        # Ponteiro
        proporcao = (self.temperatura - temp_min_scale) / temp_range_scale
        angulo_ponteiro_deg = start_angle_drawing_deg + proporcao * span_angle_drawing_deg
        rad_ponteiro = math.radians(angulo_ponteiro_deg)

        ponteiro_x = centro_x + (raio - 25) * math.cos(rad_ponteiro)
        ponteiro_y = centro_y - (raio - 25) * math.sin(rad_ponteiro)

        painter.setPen(QPen(QColor(255, 69, 0), 4))  # Ponteiro laranja vibrante
        painter.drawLine(centro_x, centro_y, int(ponteiro_x), int(ponteiro_y))

        painter.setBrush(QColor(255, 69, 0))
        painter.drawEllipse(centro_x - 5, centro_y - 5, 10, 10)

        # Texto "C" e "H"
        font_label = QFont("Arial", 11, QFont.Bold)
        painter.setFont(font_label)

        offset_CH = raio_numeros + 20

        painter.setPen(QPen(QColor(30, 144, 255), 2))  # Azul royal
        rad_c = math.radians(start_angle_drawing_deg)
        x_c = centro_x + offset_CH * math.cos(rad_c)
        y_c = centro_y - offset_CH * math.sin(rad_c)
        painter.drawText(QRectF(x_c - 10, y_c - 10, 20, 20), Qt.AlignCenter, "C")

        painter.setPen(QPen(QColor(220, 20, 60), 2))  # Vermelho carmim
        rad_h = math.radians(start_angle_drawing_deg + span_angle_drawing_deg)
        x_h = centro_x + offset_CH * math.cos(rad_h)
        y_h = centro_y - offset_CH * math.sin(rad_h)
        painter.drawText(QRectF(x_h - 10, y_h - 10, 20, 20), Qt.AlignCenter, "H")
