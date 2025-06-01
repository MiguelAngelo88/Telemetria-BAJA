from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel
from PyQt5.QtGui import QPainter, QPen, QFont, QColor, QRadialGradient, QConicalGradient, QPainterPath
from PyQt5.QtCore import Qt, QRectF
import math

class TemperaturaFreioWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.temperatura = 30
        self.setMinimumSize(150, 150)
        self.setMaximumSize(250, 250)

        self.internal_layout = QVBoxLayout(self)
        self.internal_layout.setAlignment(Qt.AlignCenter)

        self.temp_label = QLabel(f"{self.temperatura}°C")
        self.temp_label.setAlignment(Qt.AlignCenter)
        # Mais espaçamento à esquerda, cor e sombra leve no texto
        self.temp_label.setStyleSheet("""
            font-size: 18px; 
            font-weight: bold; 
            color: #8B0000; 
            padding-left: 120px;
            text-shadow: 1px 1px 2px #330000;
        """)
        self.internal_layout.addWidget(self.temp_label)

    def setTemperatura(self, valor):
        self.temperatura = max(0, min(valor, 120))
        self.temp_label.setText(f"Freio: {self.temperatura}°C")
        self.update()

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
        spacing = self.internal_layout.spacing() * 2
        available_dial_height = widget_height - label_height - margin_top - margin_bottom - spacing
        raio = min(widget_width, available_dial_height) // 2 - 20

        if raio <= 0:
            return

        centro_x = widget_width // 2
        centro_y = margin_top + label_height + (available_dial_height // 2)
        rect = QRectF(centro_x - raio, centro_y - raio, 2 * raio, 2 * raio)
        largura_barra = 20
        start_angle_drawing_deg = 90
        span_angle_drawing_deg = 180
        temp_min_scale = 0
        temp_max_scale = 120
        temp_range_scale = temp_max_scale - temp_min_scale

        # Fundo gradiente radial (fundo escuro moderno)
        grad_fundo = QRadialGradient(centro_x, centro_y, raio)
        grad_fundo.setColorAt(0, QColor(30, 30, 30))
        grad_fundo.setColorAt(1, QColor(10, 10, 10))
        painter.setBrush(grad_fundo)
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(rect)

        # Anel externo com gradiente metálico
        grad_anel = QConicalGradient(centro_x, centro_y, -90)
        grad_anel.setColorAt(0, QColor(150, 150, 150))
        grad_anel.setColorAt(0.5, QColor(70, 70, 70))
        grad_anel.setColorAt(1, QColor(150, 150, 150))
        pen_anel = QPen()
        pen_anel.setWidth(6)
        pen_anel.setBrush(grad_anel)
        painter.setPen(pen_anel)
        painter.setBrush(Qt.NoBrush)
        painter.drawEllipse(rect)

        # Zonas coloridas suavizadas (menos saturado)
        zonas = [
            (0, 50, QColor(0, 120, 170)),  # azul
            (50, 90, QColor(0, 150, 0)),   # verde
            (90, 120, QColor(170, 0, 0))   # vermelho
        ]
        for zona_min, zona_max, cor in zonas:
            proporcao_min = (zona_min - temp_min_scale) / temp_range_scale
            proporcao_max = (zona_max - temp_min_scale) / temp_range_scale
            start_zona_deg_invertido = start_angle_drawing_deg + (1 - proporcao_max) * span_angle_drawing_deg
            end_zona_deg_invertido = start_angle_drawing_deg + (1 - proporcao_min) * span_angle_drawing_deg
            span_zona_deg_invertido = end_zona_deg_invertido - start_zona_deg_invertido
            span_zona_16deg = int(span_zona_deg_invertido * 16)
            start_arc_angle_16deg = int(start_zona_deg_invertido * 16)
            pen_zona = QPen(cor, largura_barra, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin)
            painter.setPen(pen_zona)
            painter.drawArc(rect, start_arc_angle_16deg, span_zona_16deg)

        # Marcadores com sombra e cor clara
        font_numbers = QFont("Arial", 10, QFont.Bold)
        painter.setFont(font_numbers)
        for temp_value in range(temp_min_scale, temp_max_scale + 1, 20):
            proporcao_no_arco = (temp_value - temp_min_scale) / temp_range_scale
            angulo_num_deg = start_angle_drawing_deg + (1 - proporcao_no_arco) * span_angle_drawing_deg
            angulo_num_rad = math.radians(angulo_num_deg)
            raio_numeros_pos = raio + 5
            x_num = centro_x + raio_numeros_pos * math.cos(angulo_num_rad)
            y_num = centro_y - raio_numeros_pos * math.sin(angulo_num_rad)
            text_str = str(temp_value)
            text_width = painter.fontMetrics().horizontalAdvance(text_str)
            text_height = painter.fontMetrics().height()
            text_rect = QRectF(x_num - text_width / 2, y_num - text_height / 2, text_width, text_height)

            # sombra do texto para destacar
            painter.setPen(QColor(0, 0, 0, 160))
            painter.drawText(text_rect.translated(1, 1), Qt.AlignCenter, text_str)

            painter.setPen(QColor(230, 230, 230))
            painter.drawText(text_rect, Qt.AlignCenter, text_str)

        # Ticks (marcadores) mais finos e com sombra
        tick_length = 8
        tick_start_radius = raio - largura_barra + 2
        tick_end_radius = tick_start_radius + tick_length
        for temp_value in range(temp_min_scale, temp_max_scale + 1, 20):
            proporcao_no_arco = (temp_value - temp_min_scale) / temp_range_scale
            angulo_tick_deg = start_angle_drawing_deg + (1 - proporcao_no_arco) * span_angle_drawing_deg
            angulo_tick_rad = math.radians(angulo_tick_deg)
            x_tick_start = centro_x + tick_start_radius * math.cos(angulo_tick_rad)
            y_tick_start = centro_y - tick_start_radius * math.sin(angulo_tick_rad)
            x_tick_end = centro_x + tick_end_radius * math.cos(angulo_tick_rad)
            y_tick_end = centro_y - tick_end_radius * math.sin(angulo_tick_rad)

            # sombra da linha
            painter.setPen(QPen(QColor(0, 0, 0, 150), 3))
            painter.drawLine(int(x_tick_start)+1, int(y_tick_start)+1, int(x_tick_end)+1, int(y_tick_end)+1)

            painter.setPen(QPen(QColor(230, 230, 230), 2))
            painter.drawLine(int(x_tick_start), int(y_tick_start), int(x_tick_end), int(y_tick_end))

        # Ponteiro com gradiente e ponta arredondada
        proporcao_ponteiro = (self.temperatura - temp_min_scale) / temp_range_scale
        angulo_ponteiro_deg = start_angle_drawing_deg + (1 - proporcao_ponteiro) * span_angle_drawing_deg
        rad_ponteiro = math.radians(angulo_ponteiro_deg)

        ponteiro_path = QPainterPath()
        ponteiro_path.moveTo(centro_x, centro_y)

        ponteiro_x = centro_x + (raio - 40) * math.cos(rad_ponteiro)
        ponteiro_y = centro_y - (raio - 40) * math.sin(rad_ponteiro)

        largura_ponteiro = 10
        angulo_perp = rad_ponteiro + math.pi / 2

        x1 = centro_x + largura_ponteiro * math.cos(angulo_perp)
        y1 = centro_y - largura_ponteiro * math.sin(angulo_perp)
        x2 = centro_x - largura_ponteiro * math.cos(angulo_perp)
        y2 = centro_y + largura_ponteiro * math.sin(angulo_perp)

        ponteiro_path.lineTo(x1, y1)
        ponteiro_path.lineTo(ponteiro_x, ponteiro_y)
        ponteiro_path.lineTo(x2, y2)
        ponteiro_path.closeSubpath()

        grad_ponteiro = QConicalGradient(ponteiro_x, ponteiro_y, 0)
        grad_ponteiro.setColorAt(0, QColor(255, 70, 70))
        grad_ponteiro.setColorAt(1, QColor(180, 0, 0))

        painter.setBrush(grad_ponteiro)
        painter.setPen(Qt.NoPen)
        painter.drawPath(ponteiro_path)

        # Centro do ponteiro (círculo branco com sombra)
        sombra_centro = QRadialGradient(centro_x, centro_y, 8)
        sombra_centro.setColorAt(0, QColor(255, 255, 255))
        sombra_centro.setColorAt(1, QColor(150, 150, 150))
        painter.setBrush(sombra_centro)
        painter.drawEllipse(centro_x - 8, centro_y - 8, 16, 16)

        # Letras C e H com sombra
        font_label = QFont("Arial", 12, QFont.Bold)
        painter.setFont(font_label)
        distancia_offset_CH = raio + 20

        # C - Azul suave
        painter.setPen(QColor(0, 100, 255, 220))
        rad_c = math.radians(start_angle_drawing_deg + (1 - (0 - temp_min_scale) / temp_range_scale) * span_angle_drawing_deg)
        x_c = centro_x + distancia_offset_CH * math.cos(rad_c)
        y_c = centro_y - distancia_offset_CH * math.sin(rad_c)
        text_c_width = painter.fontMetrics().horizontalAdvance("C")
        text_c_height = painter.fontMetrics().height()
        text_rect_c = QRectF(x_c - text_c_width / 2, y_c - text_c_height / 2, text_c_width, text_c_height)

        # sombra sutil
        painter.setPen(QColor(0, 0, 0, 140))
        painter.drawText(text_rect_c.translated(1, 1), Qt.AlignCenter, "C")
        painter.setPen(QColor(0, 100, 255))
        painter.drawText(text_rect_c, Qt.AlignCenter, "C")

        # H - Vermelho suave
        painter.setPen(QColor(180, 0, 0, 220))
        rad_h = math.radians(start_angle_drawing_deg + (1 - (120 - temp_min_scale) / temp_range_scale) * span_angle_drawing_deg)
        x_h = centro_x + distancia_offset_CH * math.cos(rad_h)
        y_h = centro_y - distancia_offset_CH * math.sin(rad_h)
        text_h_width = painter.fontMetrics().horizontalAdvance("H")
        text_h_height = painter.fontMetrics().height()
        text_rect_h = QRectF(x_h - text_h_width / 2, y_h - text_h_height / 2, text_h_width, text_h_height)

        painter.setPen(QColor(0, 0, 0, 140))
        painter.drawText(text_rect_h.translated(1, 1), Qt.AlignCenter, "H")
        painter.setPen(QColor(180, 0, 0))
        painter.drawText(text_rect_h, Qt.AlignCenter, "H")
