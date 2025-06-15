import sys
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QPushButton, QVBoxLayout,
    QFileDialog, QSpacerItem, QSizePolicy
)
from PyQt5.QtGui import QPixmap, QFont, QColor
from PyQt5.QtCore import Qt
from tensorflow.keras.preprocessing import image
from tensorflow.keras.models import load_model


class OtoscopeClassifier(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Otoscope Condition Classifier")
        self.resize(500, 600)

        # Load model and class names
        self.model = load_model("my_model.keras")  # Or "my_model.h5"
        self.class_names = ['Chronic otitis media', 'Earwax plug', 'Myringosclerosis', 'Normal']

        # Set blue theme stylesheet
        self.setStyleSheet("""
            QWidget {
                background-color: #e3f2fd;
                font-family: Arial;
            }
            QLabel {
                color: #0d47a1;
                font-size: 16px;
            }
            QPushButton {
                background-color: #2196f3;
                color: white;
                padding: 10px;
                font-size: 16px;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #1976d2;
            }
        """)

        # Title
        self.label = QLabel("Upload an Otoscope Image")
        self.label.setFont(QFont("Arial", 18, QFont.Bold))
        self.label.setAlignment(Qt.AlignCenter)

        # Image preview
        self.image_label = QLabel()
        self.image_label.setFixedSize(300, 300)
        self.image_label.setStyleSheet("border: 2px dashed #90caf9;")
        self.image_label.setAlignment(Qt.AlignCenter)

        # Result label
        self.result_label = QLabel("")
        self.result_label.setAlignment(Qt.AlignCenter)
        self.result_label.setFont(QFont("Arial", 14))

        # Upload button
        self.button = QPushButton("Upload Image")
        self.button.clicked.connect(self.load_image)
        self.button.setFixedWidth(200)

        # Layout setup
        layout = QVBoxLayout()
        layout.addSpacerItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding))
        layout.addWidget(self.label)
        layout.addWidget(self.image_label, alignment=Qt.AlignCenter)
        layout.addWidget(self.result_label)
        layout.addWidget(self.button, alignment=Qt.AlignCenter)
        layout.addSpacerItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding))
        self.setLayout(layout)

    def load_image(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Open Image", "", "Image Files (*.png *.jpg *.jpeg)"
        )
        if file_path:
            pixmap = QPixmap(file_path).scaled(300, 300, Qt.KeepAspectRatio)
            self.image_label.setPixmap(pixmap)
            self.predict_image(file_path)

    def predict_image(self, img_path):
        img = image.load_img(img_path, target_size=(224, 224))
        img_array = image.img_to_array(img) / 255.0
        img_array = np.expand_dims(img_array, axis=0)

        predictions = self.model.predict(img_array)
        predicted_index = np.argmax(predictions, axis=1)[0]
        confidence = np.max(predictions) * 100
        predicted_class = self.class_names[predicted_index]

        self.result_label.setText(
            f"Prediction: <b>{predicted_class}</b><br>Confidence: <b>{confidence:.2f}%</b>"
        )


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = OtoscopeClassifier()
    window.show()
    sys.exit(app.exec_())
