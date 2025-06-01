from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel, QSizePolicy
from PyQt5.QtGui import QPainter, QPen, QFont, QColor
from PyQt5.QtCore import Qt, QRectF
import math


class TemperaturaWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.temperatura = 70  # Valor inicial
        self.setMinimumSize(250, 250)
        self.setMaximumSize(350, 350)
        self.internal_layout = QVBoxLayout(self)
        self.internal_layout.setAlignment(Qt.AlignCenter)  # Centraliza o conteúdo
        self.temp_label = QLabel(f"{self.temperatura}°C")
        self.temp_label.setAlignment(Qt.AlignCenter)
        self.temp_label.setStyleSheet("font-size: 18px; font-weight: bold; color: blue;")
        self.internal_layout.addWidget(self.temp_label)

    def setTemperatura(self, valor):
        """
        Define e atualiza a temperatura exibida no widget.
        Este método será chamado para atualizar o ponteiro e o texto.
        """
        self.temperatura = max(0, min(valor, 120))
        self.temp_label.setText(f"{self.temperatura}°C")
        self.update()  # Força o widget a redesenhar

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        widget_width = self.width()
        widget_height = self.height()

        if widget_width <= 0 or widget_height <= 0:
            return
        label_height = self.temp_label.sizeHint().height()
        margin_top = self.internal_layout.contentsMargins().top()
        margin_bottom = self.internal_layout.contentsMargins().bottom()
        spacing = self.internal_layout.spacing() * 2  # Considerando espaçamento acima e abaixo do label
        available_dial_height = widget_height - label_height - margin_top - margin_bottom - spacing
        raio = min(widget_width, available_dial_height) // 2 - 20

        if raio <= 0:
            return
        centro_x = widget_width // 2
        centro_y = margin_top + label_height + (available_dial_height // 2)
        rect = QRectF(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)
        largura_barra = 20  # Largura da barra colorida do medidor
        start_angle_drawing_deg = 270
        span_angle_drawing_deg = 180
        temp_min_scale = 0
        temp_max_scale = 120
        temp_range_scale = temp_max_scale - temp_min_scale

        # Medidor
        zonas = [
            (0, 60, QColor(0, 200, 0)),  # Verde
            (60, 100, QColor(255, 165, 0)),  # Laranja
            (100, 120, QColor(200, 0, 0))  # Vermelho
        ]

        for zona_min, zona_max, cor in zonas:
            proporcao_zona_no_range = (zona_min - temp_min_scale) / temp_range_scale
            span_zona_proporcional = (zona_max - zona_min) / temp_range_scale
            start_zona_deg = start_angle_drawing_deg + proporcao_zona_no_range * span_angle_drawing_deg
            span_zona_deg = span_zona_proporcional * span_angle_drawing_deg
            span_zona_16deg = int(span_zona_deg * 16)
            start_arc_angle_16deg = int(start_zona_deg * 16)
            painter.setPen(QPen(cor, largura_barra, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin))
            painter.drawArc(rect, start_arc_angle_16deg, span_zona_16deg)
        font_numbers = QFont("Arial", 9, QFont.Bold)
        painter.setFont(font_numbers)
        painter.setPen(Qt.white)  # Cor branca para os números
        raio_numeros_pos = raio + 1

        for temp_value in range(temp_min_scale, temp_max_scale + 1, 20):  # Números a cada 20
            proporcao_no_arco = (temp_value - temp_min_scale) / temp_range_scale
            angulo_num_deg = start_angle_drawing_deg + proporcao_no_arco * span_angle_drawing_deg
            angulo_num_rad = math.radians(angulo_num_deg)
            x_num = centro_x + raio_numeros_pos * math.cos(angulo_num_rad)
            y_num = centro_y - raio_numeros_pos * math.sin(angulo_num_rad)  # Note o '-' para coordenadas Y em Qt
            text_str = str(temp_value)
            text_width = painter.fontMetrics().horizontalAdvance(text_str)  # Método mais moderno
            text_height = painter.fontMetrics().height()
            text_rect = QRectF(x_num - text_width / 2, y_num - text_height / 2, text_width, text_height)
            painter.drawText(text_rect, Qt.AlignCenter, text_str)
        tick_length = 8
        tick_start_radius = raio - largura_barra + 2
        tick_end_radius = tick_start_radius + tick_length

        painter.setPen(QPen(Qt.white, 2))  # Cor branca e espessura 2 para os ticks

        for temp_value in range(temp_min_scale, temp_max_scale + 1, 20):  # Ticks para os valores 0, 20, 40...
            proporcao_no_arco = (temp_value - temp_min_scale) / temp_range_scale
            angulo_tick_deg = start_angle_drawing_deg + proporcao_no_arco * span_angle_drawing_deg
            angulo_tick_rad = math.radians(angulo_tick_deg)

            x_tick_start = centro_x + tick_start_radius * math.cos(angulo_tick_rad)
            y_tick_start = centro_y - tick_start_radius * math.sin(angulo_tick_rad)
            x_tick_end = centro_x + tick_end_radius * math.cos(angulo_tick_rad)
            y_tick_end = centro_y - tick_end_radius * math.sin(angulo_tick_rad)
            painter.drawLine(int(x_tick_start), int(y_tick_start), int(x_tick_end), int(y_tick_end))

        # Ponteiro
        proporcao_ponteiro = (self.temperatura - temp_min_scale) / temp_range_scale
        angulo_ponteiro_deg = start_angle_drawing_deg + proporcao_ponteiro * span_angle_drawing_deg
        rad_ponteiro = math.radians(angulo_ponteiro_deg)
        ponteiro_x = centro_x + (raio - 30) * math.cos(rad_ponteiro)  # Ponteiro menor que o raio total
        ponteiro_y = centro_y - (raio - 30) * math.sin(rad_ponteiro)
        painter.setPen(QPen(QColor(255, 255, 255), 4))  # Ponteiro branco e mais grosso
        painter.drawLine(centro_x, centro_y, int(ponteiro_x), int(ponteiro_y))
        painter.setBrush(QColor(255, 255, 255))
        painter.drawEllipse(centro_x - 5, centro_y - 5, 10, 10)

        # Texto "C" e "H"
        font_label = QFont("Arial", 10, QFont.Bold)
        painter.setFont(font_label)
        distancia_offset_CH = raio_numeros_pos + 15  # Afasta "C" e "H" dos números
        painter.setPen(QPen(QColor(0, 0, 255), 2))  # Azul
        rad_c = math.radians(start_angle_drawing_deg + (0 - temp_min_scale) / temp_range_scale * span_angle_drawing_deg)
        x_c = centro_x + distancia_offset_CH * math.cos(rad_c)
        y_c = centro_y - distancia_offset_CH * math.sin(rad_c)
        text_c_width = painter.fontMetrics().horizontalAdvance("C")
        text_c_height = painter.fontMetrics().height()
        text_rect_c = QRectF(x_c - text_c_width / 2, y_c - text_c_height / 2, text_c_width, text_c_height)
        painter.drawText(text_rect_c, Qt.AlignCenter, "C")
        painter.setPen(QPen(QColor(255, 0, 0), 2))  # Vermelho
        rad_h = math.radians(
            start_angle_drawing_deg + (120 - temp_min_scale) / temp_range_scale * span_angle_drawing_deg)
        x_h = centro_x + distancia_offset_CH * math.cos(rad_h)
        y_h = centro_y - distancia_offset_CH * math.sin(rad_h)
        text_h_width = painter.fontMetrics().horizontalAdvance("H")
        text_h_height = painter.fontMetrics().height()
        text_rect_h = QRectF(x_h - text_h_width / 2, y_h - text_h_height / 2, text_h_width, text_h_height)
        painter.drawText(text_rect_h, Qt.AlignCenter, "H")