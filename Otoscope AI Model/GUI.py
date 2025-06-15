import sys
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QFileDialog,
    QSpacerItem, QSizePolicy, QHBoxLayout
)
from PyQt5.QtGui import QPixmap, QFont
from PyQt5.QtCore import Qt
from tensorflow.keras.preprocessing import image
from tensorflow.keras.models import load_model

# VTK imports
from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor
import vtk


class ModelViewer(QWidget):
    def __init__(self, model_path, parent=None):
        super().__init__(parent)

        self.vtk_widget = QVTKRenderWindowInteractor(self)
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.vtk_widget)
        self.setLayout(self.layout)

        self.renderer = vtk.vtkRenderer()
        self.vtk_widget.GetRenderWindow().AddRenderer(self.renderer)
        self.interactor = self.vtk_widget.GetRenderWindow().GetInteractor()

        self.actor = None
        self.load_model(model_path)

    def load_model(self, file_path):
        if file_path.lower().endswith(".obj"):
            reader = vtk.vtkOBJReader()
        else:
            print("Only .obj models are supported in this example.")
            return

        reader.SetFileName(file_path)
        reader.Update()

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputConnection(reader.GetOutputPort())

        self.actor = vtk.vtkActor()
        self.actor.SetMapper(mapper)

        self.renderer.AddActor(self.actor)
        self.renderer.SetBackground(0.9, 0.9, 1.0)
        self.renderer.ResetCamera()
        self.interactor.Initialize()
        self.interactor.Start()


class OtoscopeClassifier(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Otoscope Classifier & 3D Viewer")
        self.resize(900, 600)

        self.model = load_model("my_model.keras")
        self.class_names = ['Chronic otitis media', 'Earwax plug', 'Myringosclerosis', 'Normal']

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
        self.label = QLabel("Otoscope Image Classifier")
        self.label.setFont(QFont("Arial", 18, QFont.Bold))
        self.label.setAlignment(Qt.AlignCenter)

        # Image preview
        self.image_label = QLabel()
        self.image_label.setFixedSize(300, 300)
        self.image_label.setStyleSheet("border: 2px dashed #90caf9;")
        self.image_label.setAlignment(Qt.AlignCenter)

        # Result
        self.result_label = QLabel("")
        self.result_label.setAlignment(Qt.AlignCenter)
        self.result_label.setFont(QFont("Arial", 14))

        # Upload button
        self.button = QPushButton("Upload Image")
        self.button.setFixedWidth(200)
        self.button.clicked.connect(self.load_image)

        # 3D Viewer (load static model)
        self.model_viewer = ModelViewer(r"C:\Users\monae\OneDrive\Documents\Untitled.obj")

        # Left layout (Classifier)
        left_layout = QVBoxLayout()
        left_layout.addSpacerItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))
        left_layout.addWidget(self.label)
        left_layout.addWidget(self.image_label, alignment=Qt.AlignCenter)
        left_layout.addWidget(self.result_label)
        left_layout.addWidget(self.button, alignment=Qt.AlignCenter)
        left_layout.addSpacerItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))

        # Right layout (3D viewer)
        right_layout = QVBoxLayout()
        viewer_title = QLabel("Otoscope 3D Model")
        viewer_title.setFont(QFont("Arial", 18, QFont.Bold))
        viewer_title.setAlignment(Qt.AlignCenter)
        right_layout.addWidget(viewer_title)
        right_layout.addWidget(self.model_viewer)

        # Combine layouts
        main_layout = QHBoxLayout()
        main_layout.addLayout(left_layout, 1)
        main_layout.addLayout(right_layout, 2)
        self.setLayout(main_layout)

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
